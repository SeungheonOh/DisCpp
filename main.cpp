// gcc -o app main.cpp -lboost_system -lpthread -stdc++ -lssl -lcrypto -ljson -lcurl -lcurlpp

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <algorithm>
#include <thread>
#include <mutex>

//#include <curlpp/cURLpp.hpp>
//#include <curlpp/Options.hpp>
//#include <curlpp/Easy.hpp>
//#include <json/json.h>

#include "discordClient/discordClient.h"
#include "discordClient/discord/discord.h"
#include "cli.h"

#include "EmojiTools/EmojiTools.h"

//#include "discordClient/gateway/discordGateway.h"


#include <typeinfo>


std::mutex mtx;
std::mutex inputLock;



void messageInput(discordClient *c){
	while(true){
		inputLock.lock();
		std::string mess = "";
		std::getline(std::cin, mess);
		c -> sendMessage(mess);
		inputLock.unlock();
	}
}

class theClient: public discordClient, public Cli{
public:
  int m_usernameLength;
  int m_commandDividerLength;
  char m_commandDivider;
  std::mutex m_messageQueueLock;
  std::thread *m_messageDemon;
  std::queue<std::string> m_messageQueue;
  std::map<std::string , std::string> m_nicknames;

  theClient() : m_usernameLength(25) , m_commandDividerLength(40) , m_commandDivider('-'){
    m_messageDemon = new std::thread(&theClient::messageDemon, this);
  }

  ~theClient(){
    m_messageDemon -> join();
    delete m_messageDemon;
  }
  // util
  std::string formatMessage(Message message){
    std::string buffer;
    std::string nick = m_nicknames.find(message.author().id().asString()) -> second;
    if(nick == "")nick = message.author().username();
    for(int i = 0; i < nick.length(); i++){
      if(nick[i] < 0 || nick[i] > 127 || nick[i] == '\n')continue;
      buffer += nick[i];
    }
    nick = buffer;
    buffer = "";
    for(int j = 0; j < m_usernameLength; j++){
      if(nick.length() <= j){
        buffer += " ";
        continue;
      }
      buffer += nick[j];
    }
    return buffer + " | " + EmojiTools::deEmojize(message.content());
  }
  std::string formatDivider(){
    std::string buffer;
    for(int i = 0; i < m_commandDividerLength; i++){
      buffer += m_commandDivider;
    }
    return buffer;
  }

  // discord event
  void onReady(std::string s) override{
  }
  void onMessageCreate(std::string s) override{
    if(Message(s).channelId() == m_currentChannel.id()){
      print(formatMessage(Message(s)));
    }
  }
  void onMessageDelete(std::string s) override{
  }
  void onHeartbeatAck() override{
  }

  // interface event
  void onServers(std::string s) override{
    loadServer();
    print(formatDivider());
    if(s != ""){
      if(!setServer(s)){
        print("Invalid Server");
        return;
      }
      Json::Value members = util::StringToJson(request("/guilds/" + m_currentServer.id().asString() + "/members?limit=1000",
                                                       util::generateHeader("application/json", token()), "GET"));
      m_nicknames.clear();
      for(auto m : members){
        m_nicknames.insert(std::pair<std::string, std::string>(User(util::JsonToString(m["user"])).id().asString(), m["nick"].asString()));
      }
      print("Task Complete");
      m_prompt = repl(m_promptTemplete, "%s", s);
      return;
    }
    for(int i = 0; i < m_servers.size(); i++){
      print(m_servers[i].name());
    }
  }
  void onChannels(std::string s) override{
    if(m_currentServer.name() == "")return;

    for(int i = 0; i < 1000; i++){
      if(m_messageQueueLock.try_lock())break;
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    while(!m_messageQueue.empty())m_messageQueue.pop();
    m_messageQueueLock.unlock();
    m_currentChannel = Channel();

    // Setting Channel
    loadChannel();
    print(formatDivider());
    if(s != ""){
      for(auto val : m_channels){
        if(s == val.name() && val.m_parentId.asString() == ""){
          print("Invalid Channel");
          return;
        }
      }
      if(!setChannel(s)){
        print("Invalid Channel");
        return;
      }
      print("Task Completed");
      m_prompt = repl(m_prompt, "%c", "/" + s);
      std::vector<Message> mess = getChannelMessages();
      std::reverse(mess.begin(), mess.end());
      for(auto i : mess){
        if(m_nicknames.find(i.author().id().asString()) == m_nicknames.end()){
          Json::Value member = util::StringToJson(request("/guilds/" + m_currentServer.id().asString() + "/members/" + i.author().id().asString(),
                                                           util::generateHeader("application/json", token()), "GET"));
          m_nicknames.insert(std::pair<std::string, std::string>(User(util::JsonToString(member["user"])).id().asString(), member["nick"].asString()));
        }
        print(formatMessage(i));
      }
      return;
    }

    // Printing channel
    std::sort(m_channels.begin(), m_channels.end(), [](Channel const &a, Channel const &b) -> bool{
                                                      return a.m_position < b.m_position;
                                                    });

    std::map<std::string, std::vector<Channel>> channelList;

    for(auto channel : m_channels){
      std::map<std::string, std::vector<Channel>>::iterator it;
      it = channelList.find(channel.m_parentId.asString());
      if(it == channelList.end()){
        channelList.insert(std::pair<std::string, std::vector<Channel>>(channel.m_parentId.asString(), std::vector<Channel>(1, channel)));
      } else{
        it -> second.push_back(channel);
      }
    }

    std::vector<Channel> cats;
    for(auto const &a : channelList){
      for(auto const &val : m_channels){
        if(val.id().asString() == a.first && val.m_parentId.asString() == "")cats.push_back(val);
      }
    }
    std::sort(cats.begin(), cats.end(), [](Channel const &a, Channel const &b) -> bool{
                                                      return a.m_position < b.m_position;
                                                    });

    int cataSize = 50;
    int cataTabSize = 2;
    char cataMat = '+';
    for(auto cat : cats){
      std::map<std::string, std::vector<Channel>>::iterator it;
      it = channelList.find(cat.id().asString());
      std::string buffer = "";
      for(auto val : m_channels){
        if(val.id().asString() == it->first){
          /*
          for(int i = 0; i < (cataSize - val.name().length()) / 2; i++){
            buffer += cataMat;
          }
          buffer += val.name();
          for(int i = 0; i < (cataSize - val.name().length()) / 2 + (cataSize - val.name().length()) % 2; i++){
            buffer += cataMat;
          }
          */
          buffer += val.name();
        }
      }
      std::string divider = "┌";
      for(int i = 0; i < buffer.length(); i++)divider += "─";
      divider += "┐";
      print(divider);

      print("│" + buffer + "│");

      divider = "└";
      for(int i = 0; i < buffer.length(); i++){
        if(i == cataTabSize - 1){
          divider += "┬";
          continue;
        }
        divider += "─";
      }
      divider += "┘";
      print(divider);

      for(int i = 0; i < it -> second.size(); i++){
        if(it -> second[i].m_parentId.asString() == "")continue;
        std::string space;
        for(int i = 0; i < cataTabSize; i++){
          space += " ";
        }
        if(i + 1 == it -> second.size())space += "└";
        else space += "├";
        print(space + it -> second[i].name());
      }
    }
    /*
    for(int i = 0; i < m_channels.size(); i++){
      print(m_channels[i].name() + " " + std::to_string(m_channels[i].m_type) + " " + std::to_string(m_channels[i].position()));
      if(std::to_string(m_channels[i].m_type) == "4"){
        print(m_channels[i].id().asString());
      } else {
        print(m_channels[i].m_parentId.asString());
      }
    }
    */

  }
  void onSend(std::string s) override{
    if(m_currentChannel.name() == ""){
      return;
    }
    for(int i = 0; i < 1000; i++){
      if(m_messageQueueLock.try_lock())break;
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    m_messageQueue.push(EmojiTools::emojize(s));
    m_messageQueueLock.unlock();
    //std::thread (&theClient::send, this, s).detach(); // <- not synced
  }
  void onLogin(std::string id, std::string pass) override{
    char c;
    int cursor = 0;
    std::string buffer;

    m_me = User();
    if(id == ""){
        while((c = getch()) != 10){
        std::cout << "\033[u";
        for(int i = 0; i < buffer.length(); i++){
          std::cout << " ";
        }
        std::cout << "\033[u";
        switch(c){
        case 127:
          if(cursor - 1 < 0)break;
          cursor--;
          buffer.pop_back();
          break;
        case 27:
          buffer.clear();
          cursor = 0;
          break;
        default:
          buffer.insert(cursor++, 1, c);
          break;
        }
        std::cout << buffer;
      }
      std::cout << "\033[u";
      for(int i = 0; i < buffer.length(); i++){
        std::cout << " ";
      }
      std::cout << "\033[u";
      id = buffer;
      buffer = "";
      cursor = 0;
      while((c = getch()) != 10){
        std::cout << "\033[u";
        for(int i = 0; i < buffer.length(); i++){
          std::cout << " ";
        }
        std::cout << "\033[u";
        switch(c){
        case 127:
          if(cursor - 1 < 0)break;
          cursor--;
          buffer.pop_back();
          break;
        case 27:
          buffer.clear();
          cursor = 0;
          break;
        default:
          buffer.insert(cursor++, 1, c);
          break;
        }

        for(int i = 0; i < buffer.length(); i++){
          std::cout << "*";
        }
      }
      pass = buffer;
      std::cout << "\033[u";
      for(int i = 0; i < buffer.length(); i++){
        std::cout << " ";
      }
      std::cout << "\033[u";
    }

    if(IsTwoFactorAccount(id, pass)){ 
      std::cout << "\033[u" << "Two Factor Auth code required" << std::endl << "\033[s";
      buffer = "";
      cursor = 0;
      while((c = getch()) != 10){
        std::cout << "\033[u";
        for(int i = 0; i < buffer.length(); i++){
          std::cout << " ";
        }
        std::cout << "\033[u";
        switch(c){
        case 127:
          if(cursor - 1 < 0)break;
          cursor--;
          buffer.pop_back();
          break;
        case 27:
          buffer.clear();
          cursor = 0;
          break;
        default:
          buffer.insert(cursor++, 1, c);
          break;
        }
        std::cout << buffer;
      }
      std::cout << "\033[u";
      for(int i = 0; i < buffer.length(); i++){
        std::cout << " ";
      }
      std::cout << "\033[u";
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
      login(id, pass, buffer);
    } else {
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
      login(id, pass);
    }
    print(formatDivider());
    if(m_me.username() != ""){
      print("Logged In");
      print("Welcom " + m_me.username());
      if(!connect()){
        print("Failed to open websocket connection, try :reconnect");
      }
    } else{
      print("Login Failed");
    }
  }
  void onQuit() override{
    if(m_me.username()!= "")disconnect();
  }
  void onStatus(std::string s) override{
    updateStatus("online", s);
  }
  void onReconnect() override{
    reconnect();
  }

  // Not in use
  void send(std::string s){
    while(1){
      if(createMessage(s, false))break;
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }

  void messageDemon(){
    int failCont;
    while(1){
      if(!m_messageQueue.empty()){
        int queueSize = m_messageQueue.size();
        if(m_messageQueueLock.try_lock()){
          if(createMessage(m_messageQueue.front(), false)){
            m_messageQueue.pop();
            failCont = 0;
          }
          m_messageQueueLock.unlock();
        }
        if(failCont > 5 && queueSize == m_messageQueue.size()){
          m_messageQueue.pop();
          failCont = 0;
        }else if(queueSize == m_messageQueue.size()){
          std::this_thread::sleep_for(std::chrono::milliseconds(1000));
          ++failCont;
        }
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }
};




int main(){

  theClient *client = new theClient();
  client -> startInterface();

  while(1){
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
}