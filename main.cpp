// gcc -o app main.cpp -lboost_system -lpthread -stdc++ -lssl -lcrypto -ljson -lcurl -lcurlpp

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <algorithm>
#include <ctime>
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


#include "config.h"


// Defaults
#ifndef MESSAGE_FORMAT
#define MESSAGE_FORMAT "\033[36m" + formatTime(message.timestamp(), "%H:%M") + "\033[0m" + " | " + formatAuthor(message)+ " | " + formatContent(message);
#endif

#ifndef ATTACHMENT_LABEL
#define ATTACHMENT_LABEL "\033[104m(Attachment)\033[0m"
#endif

#ifndef MENTION_COLOR
#define MENTION_COLOR "\033[101m\033[30m"
#endif

#ifndef USERNAME_SIZE
#define USERNAME_SIZE 25
#endif

#ifndef COMMAND_DIVIDER_LENGTH
#define COMMAND_DIVIDER_LENGTH 40
#endif

#ifndef COMMAND_DIVIDER_CHAR
#define COMMAND_DIVIDER_CHAR '-'
#endif

#ifndef PROMPT
#define PROMPT "[%s%c]>"
#endif

#ifndef PROMPT_DIVIDER
#define PROMPT_DIVIDER "/"
#endif



class theClient: public discordClient, public Cli{
public:

  std::mutex m_messageQueueLock;
  std::thread *m_messageDemon;
  std::queue<std::string> m_messageQueue;
  std::vector<GuildMember> m_members;

  theClient(){
    m_promptTemplete = PROMPT;
    m_messageDemon = new std::thread(&theClient::messageDemon, this);
  }

  ~theClient(){
    m_messageDemon -> join();
    delete m_messageDemon;
  }

  std::string formatTime(std::string s, std::string format){
    std::tm time;
    std::stringstream ret;
    std::istringstream(s) >> std::get_time(&time, "%Y-%m-%dT%H:%M:%S");
    std::time_t UTCTime = timegm(&time);
    std::time_t localTime = mktime(&time);
    std::time_t t = localTime + (UTCTime - localTime);

    ret << std::put_time(std::localtime(&t), format.c_str());
    return ret.str();
  }

  std::string formatContent(Message message){
    std::string rawContent = message.content();
    std::string ret;
    int mentionCount = 0;
    // mentions
    if(rawContent == ""){
      return ATTACHMENT_LABEL;
    }

    for(int i = 0; i < rawContent.length(); i++){
      if(rawContent[i] == '\n'){
      }
      // Mention Handling
      else if(i + 20 <= rawContent.length() && rawContent[i] == '<' && rawContent[i + 1] == '@' && rawContent[i + 20] == '>'){
        for(auto mention : message.mentions()){
          if(mention.id().asString() == rawContent.substr(i + 2, 18)){
            ret += MENTION_COLOR + mention.username() + "\033[0m";
          }
        }
        i += 20;
      } else if(i + 21 <= rawContent.length() && rawContent[i] == '<' && rawContent[i + 1] == '@' && rawContent[i + 2] == '!' && rawContent[i + 21] == '>'){
        std::string userId = rawContent.substr(i + 3, 18);
        GuildMember mentionedMember;
        std::string nick;

        for(auto &member : m_members){
          if(member.user().id().asString() == userId){
            mentionedMember = member;
          }
        }

        nick = mentionedMember.nick();
        if(nick == "")mentionedMember.user().username();

        ret += MENTION_COLOR + nick + "\033[0m";

        i += 21;
      }
      // ``` handling
      else if(false && i + 2 <= rawContent.length() && rawContent[i] == '`' && rawContent[i + 1] == '`' && rawContent[i + 2] == '`'){
        i -= 1;
        ret += '\n';
        while(i + 2 <= rawContent.length() && rawContent[i] == '`' && rawContent[i + 1] == '`' && rawContent[i + 2] == '`'){
          i++;
          ret += rawContent[i];
        }
        i += 3;
      }else {
        ret += rawContent[i];
      }
    }
#ifdef DisableDeEmojize
    return ret;
#endif
    return EmojiTools::deEmojize(ret);
  }
  std::string formatAuthor(Message message){
    std::string buffer;
    std::string author;
    for(auto &member : m_members){
      if(member.user().id() == message.author().id()){
        if(message.member().nick() != "" && member.nick() != message.member().nick()){
          member.m_nick = message.member().nick();
        }
        author = member.nick();
      }
    }

    if(author == "")author = message.author().username();
    for(int i = 0; i < author.length(); i++){
      if(author[i] < 0 || author[i] > 127 || author[i] == '\n') continue;
      buffer += author[i];
    }

    // Space Calc
    author = "";
    for(int j = 0; j < USERNAME_SIZE; j++){
      if(buffer.length() <= j){
        author += " ";
        continue;
      }
      author += buffer[j];
    }
    return author;
  }

  std::string formatMessage(Message message){
    return MESSAGE_FORMAT;
  }

  std::string formatDivider(){
    std::string buffer;
    for(int i = 0; i < COMMAND_DIVIDER_LENGTH; i++){
      buffer += COMMAND_DIVIDER_CHAR;
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
  void onMessageUpdate(std::string s) override{
    if(Message(s).channelId() == m_currentChannel.id()){
      onReload();
    }
  }
  void onMessageDelete(std::string s) override{
    if(Message(s).channelId() == m_currentChannel.id()){
      onReload();
    }
  }
  void onHeartbeatAck() override{
  }

  // interface event
  void onServers(std::string s) override{
    if(token() == "")return;
    print(formatDivider());
    if(s == "DM"){
      m_prompt = repl(m_promptTemplete, "%s", "");
      m_currentServer = Guild();
      print("Task Completed");
      onChannels("");
      return;
    }
    loadServer();
    if(s != ""){
      if(!setServer(s)){
        print("Invalid Server");
        return;
      }
      m_members.clear();
      Json::Value members;
      members = util::StringToJson(request("/guilds/" + m_currentServer.id().asString() + "/members?limit=1000",
                                           util::generateHeader("application/json", token()), "GET"));

      while(members.size() > 0){
        for(auto m : members){
          m_members.push_back(GuildMember(util::JsonToString(m)));
        }
        members = util::StringToJson(request("/guilds/" + m_currentServer.id().asString() + "/members?limit=1000&after=" + m_members.back().user().id().asString(),
                                             util::generateHeader("application/json", token()), "GET"));
      }
      print("Task Complete");
      print(std::to_string(m_members.size()) + " users in this server");
      m_prompt = repl(m_promptTemplete, "%s", s);
      return;
    }
    for(int i = 0; i < m_servers.size(); i++){
      print(m_servers[i].name());
    }
  }
  void onChannels(std::string s) override{
    if(token() == "")return;
    for(int i = 0; i < 1000; i++){
      if(m_messageQueueLock.try_lock())break;
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    while(!m_messageQueue.empty())m_messageQueue.pop();
    m_messageQueueLock.unlock();
    m_currentChannel = Channel();

    // Setting Channel
    print(formatDivider());
    m_prompt = repl(m_promptTemplete, "%s", m_currentServer.name());
    m_prompt = repl(m_prompt, "%c", "");

    loadChannel();
    if(s != ""){
      for(auto val : m_channels){
        if(s == val.name() && val.m_type == 4){
          print("Invalid Channel");
          return;
        }
      }
      if(!setChannel(s)){
        print("Invalid Channel");
        return;
      }
      print("Task Completed");
      m_prompt = repl(m_promptTemplete, "%s", m_currentServer.name());
      m_prompt = repl(m_prompt, "%c", PROMPT_DIVIDER + s);
      std::vector<Message> mess = getChannelMessages();
      std::reverse(mess.begin(), mess.end());
      for(auto i : mess){
        print(formatMessage(i));
      }
      return;
    }

    // Printing channels
    std::sort(m_channels.begin(), m_channels.end(), [](Channel const &a, Channel const &b) -> bool{
                                                      return a.m_position < b.m_position;
                                                    });

    std::map<std::string, std::vector<Channel>> channelList;
    std::vector<Channel> categories;

    if(m_currentServer.name() == ""){
      channelList.insert(std::pair<std::string, std::vector<Channel>>("100000000000000000", m_channels));
      m_channels.push_back(Channel("{\"permission_overwrites\": [],\"name\": \"DM\",\"parent_id\": null,\"position\": 0,\"type\": 4,\"id\": \"100000000000000000\"}"));
      categories.push_back(Channel("{\"permission_overwrites\": [],\"name\": \"DM\",\"parent_id\": null,\"position\": 0,\"type\": 4,\"id\": \"100000000000000000\"}"));
    } else {
      for(auto channel : m_channels){
        std::map<std::string, std::vector<Channel>>::iterator it;
        it = channelList.find(channel.m_parentId.asString());
        if(it == channelList.end()){
          channelList.insert(std::pair<std::string, std::vector<Channel>>(channel.m_parentId.asString(), std::vector<Channel>(1, channel)));
        } else{
          it -> second.push_back(channel);
        }
      }

      for(auto const &a : channelList){
        for(auto const &val : m_channels){
          if(val.id().asString() == a.first && val.m_parentId.asString() == "")categories.push_back(val);
        }
      }
    }
    std::sort(categories.begin(), categories.end(), [](Channel const &a, Channel const &b) -> bool{
                                                      return a.m_position < b.m_position;
                                                    });

    int categoryChannelTabSpaces = 2;
    for(auto category : categories){
      std::map<std::string, std::vector<Channel>>::iterator it;
      it = channelList.find(category.id().asString());
      std::string buffer = category.name();

      std::string divider = "┌";
      for(int i = 0; i < buffer.length(); i++)divider += "─";
      divider += "┐";
      print(divider);

      print("│" + buffer + "│");

      divider = "└";
      for(int i = 0; i < buffer.length(); i++){
        if(i == categoryChannelTabSpaces - 1){
          divider += "┬";
          continue;
        }
        divider += "─";
      }
      divider += "┘";
      print(divider);

      for(int i = 0; i < it -> second.size(); i++){
        if(it -> first == it -> second[i].id().asString())continue;
        std::string space;
        for(int i = 0; i < categoryChannelTabSpaces; i++){
          space += " ";
        }
        if(i + 1 == it -> second.size())space += "└";
        else space += "├";
        print(space + it -> second[i].name());
      }
    }

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
      std::cout << "\033[u" << "Type your Email and Password" << std::endl << "\033[s";
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
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(400));
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
  void onUsers() override{
    print(formatDivider());
    for(auto member : m_members){
      std::string nick = member.nick();
      if(nick == "")nick = member.user().username();
      print(nick);
    }
  }
  void onEdit(std::string s) override{
    if(m_currentChannel.name() == "") return;
    if(s == "") return;
    std::vector<Message> messages = getChannelMessages(25);
    for(auto message : messages){
      if(message.author().id() == m_me.id()){
        editMessage(message, s);
        onReload();
        return;
      }
    }
  }
  void onDelete(int i) override{
    if(m_currentChannel.name() == "") return;
    if(i == -1)return;
    std::vector<Message> messages = getChannelMessages(25);
    for(auto message : messages){
      if(message.author().id() == m_me.id() && --i == 0){
        deleteMessage(message);
        onReload();
        return;
      }
    }
  }
  void onReload() override{
    if(m_currentChannel.name() == "") return;
    onChannels(m_currentChannel.name());
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

#if(defined EMAIL && defined PASSWORD)
  client -> onLogin(EMAIL, PASSWORD);
#endif

  client -> startInterface();

#if(defined EMAIL && defined PASSWORD)
#ifdef DEFAULT_SERVER
  client -> onServers(DefaultServer);
#endif
#ifdef DEFAULT_CHANNEL
  client -> onChannels(DefaultChannel);
#endif
#endif

  while(1){
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
}
