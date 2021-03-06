#include "discordClient.h"

discordClient::discordClient() : m_timeout(2000){
  m_connection = new websocket_endpoint();
}
discordClient::~discordClient(){
  disconnect();
}
std::string discordClient::request(std::string endpoint, std::list<std::string> header, std::string method){
  Json::Value root;
  curlpp::Cleanup Clean;
  std::stringstream response;

  curlpp::Easy request;
  request.setOpt(new curlpp::options::Url(m_api + endpoint));

  request.setOpt(new curlpp::options::HttpHeader(header));

  request.setOpt(new curlpp::options::CustomRequest{method});

  response << request;

  return response.str();
}

std::string discordClient::request(std::string endpoint, std::list<std::string> header, std::string body, std::string method){
  curlpp::Cleanup Clean;
  std::stringstream response;

  curlpp::Easy request;
  request.setOpt(new curlpp::options::Url(m_api + endpoint));

  request.setOpt(new curlpp::options::HttpHeader(header));
  request.setOpt(new curlpp::options::PostFields(body));
  request.setOpt(new curlpp::options::PostFieldSize(body.length()));

  request.setOpt(new curlpp::options::CustomRequest{method});

  response << request;

  return response.str();
}

/*
Json::Value discordClient::request(std::string endpoint, std::list<std::string> header, std::string method){
  Json::Value root;
  curlpp::Cleanup Clean;
  std::stringstream response;

  curlpp::Easy request;
  request.setOpt(new curlpp::options::Url(m_api + endpoint));

  request.setOpt(new curlpp::options::HttpHeader(header));

  request.setOpt(new curlpp::options::CustomRequest{method});

  response << request;

  if(response.str() == "")return root;

  response >> root;

  return root;
}

Json::Value discordClient::request(std::string endpoint, std::list<std::string> header, std::string body, std::string method){
  Json::Value root;
  curlpp::Cleanup Clean;
  std::stringstream response;

  curlpp::Easy request;
  request.setOpt(new curlpp::options::Url(m_api + endpoint));

  request.setOpt(new curlpp::options::HttpHeader(header));
  request.setOpt(new curlpp::options::PostFields(body));
  request.setOpt(new curlpp::options::PostFieldSize(body.length()));

  request.setOpt(new curlpp::options::CustomRequest{method});

  response << request;

  if(response.str() == "")return root;

  response >> root;

  return root;
}
*/

bool discordClient::login(std::string email, std::string password){
  Json::Value root;

  std::list<std::string> header;
  header.push_back("Content-Type: application/json");

  std::string JsonPayload = "{\"email\":\"" + email +
      "\",\"password\":\"" + password + "\"}";

  root = util::StringToJson(this -> request("/auth/login", header, JsonPayload, "POST"));

  if(root["token"].asString() == ""){
    return false;
  }

  m_accessToken = root["token"].asString();

  root = util::StringToJson(this -> request("/users/@me", util::generateHeader("application/json", m_accessToken), "GET"));
  m_me = User(util::JsonToString(root));

  return true;
}

bool discordClient::login(std::string email, std::string password, std::string code){
  Json::Value root;

  std::list<std::string> header;
  header.push_back("Content-Type: application/json");

  std::string JsonPayload = "{\"email\":\"" + email +
    "\",\"password\":\"" + password + "\"}";

  root = util::StringToJson(this -> request("/auth/login", header, JsonPayload, "POST"));

  if(root["ticket"].asString() == ""){
    return false;
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  JsonPayload = "{\"code\":\"" + code +
    "\",\"ticket\":\"" + root["ticket"].asString() + "\"}";

  root = util::StringToJson(this -> request("/auth/mfa/totp", header, JsonPayload, "POST"));

  if(root["token"].asString() == ""){
    return false;
  }

  m_accessToken = root["token"].asString();

  root = util::StringToJson(this -> request("/users/@me", util::generateHeader("application/json", m_accessToken), "GET"));
  m_me = User(util::JsonToString(root));

  return true;
}

bool discordClient::IsTwoFactorAccount(std::string email, std::string password){
  Json::Value root;

  std::list<std::string> header;
  header.push_back("Content-Type: application/json");

  std::string JsonPayload = "{\"email\":\"" + email +
    "\",\"password\":\"" + password + "\"}";

  root = util::StringToJson(this -> request("/auth/login", header, JsonPayload, "POST"));

  if(root["ticket"].asString() == ""){
    if(root["token"].asString() == ""){
      return false;
    }

    m_accessToken = root["token"].asString();

    root = util::StringToJson(this -> request("/users/@me", util::generateHeader("application/json", m_accessToken), "GET"));
    m_me = User(util::JsonToString(root));
    return false;
  }
  return true;
}

std::vector<Message> discordClient::getChannelMessages(){
  std::vector<Message> ret;
  Json::Value root;

  if(m_accessToken == "")return ret;

  root = util::StringToJson(this -> request("/channels/" + m_currentChannel.id().asString() + "/messages",
                         util::generateHeader("application/json", m_accessToken), "GET"));
  if(root.size() <= 0)return ret;
  for(int i = 0; i < root.size(); i++){
    ret.push_back(Message(util::JsonToString(root[i])));
  }

  return ret;
}
std::vector<Message> discordClient::getChannelMessages(Snowflake messageId, int limit){
  std::vector<Message> ret;
  Json::Value root;

  if(m_accessToken == "")return ret;

  root = util::StringToJson(this -> request("/channels/" + m_currentChannel.id().asString() +
                         "messages?around=" + messageId.asString() + "&limit=" + std::to_string(limit),
                         util::generateHeader("application/json", m_accessToken), "GET"));
  if(root.size() <= 0)return ret;
  for(int i = 0; i < root.size(); i++){
    ret.push_back(Message(util::JsonToString(root[i])));
  }

  return ret;
}
std::vector<Message> discordClient::getChannelMessages(int limit){
  std::vector<Message> ret;
  Json::Value root;

  if(m_accessToken == "")return ret;

  root = util::StringToJson(this -> request("/channels/" + m_currentChannel.id().asString() +
                                            "/messages?&limit=" + std::to_string(limit),
                                            util::generateHeader("application/json", m_accessToken), "GET"));
  if(root.size() <= 0)return ret;
  for(int i = 0; i < root.size(); i++){
    ret.push_back(Message(util::JsonToString(root[i])));
  }

  return ret;
}
Message discordClient::getChannelMessage(Snowflake messageId){
  Json::Value root;

  if(m_accessToken == "")return Message("{}");

  root = util::StringToJson(this -> request("/channels/" + m_currentChannel.id().asString() +
                         "messages?around=" + messageId.asString() + "&limit=1",
                         util::generateHeader("application/json", m_accessToken), "GET"));

  return Message(root[0].asString());
}
std::vector<Message> discordClient::getPinnedMessages(){
  std::vector<Message> ret;
  Json::Value root;

  if(m_accessToken == "")return ret;

  root = util::StringToJson(this -> request("/channels/" + m_currentChannel.id().asString() +
                         "pins",
                         util::generateHeader("application/json", m_accessToken), "GET"));
  if(root.size() <= 0)return ret;
  for(int i = 0; i < root.size(); i++){
    ret.push_back(Message(util::JsonToString(root[i])));
  }

  return ret;
}

bool discordClient::createMessage(std::string content, bool tts){
  Json::Value root;

  if(m_accessToken == "")return false;

  std::string JsonPayload = "{\"content\":\"" + content +
    "\",\"tts\":\"" + std::to_string(tts) + "\"}";
  root = util::StringToJson(this -> request("/channels/" + m_currentChannel.id().asString() + "/messages",
                         util::generateHeader("application/json", m_accessToken),
                         JsonPayload, "POST"));
  if(root["id"].asString() == "")return false;
  return 1;
}
bool discordClient::editMessage(Message message, std::string content){
  Json::Value root;

  if(m_accessToken == "")return false;

  std::string JsonPayload = "{\"content\":\"" + content + "\"}";
  root = util::StringToJson(this -> request("/channels/" + message.channelId().asString() + "/messages/" + message.id().asString(),
                         util::generateHeader("application/json", m_accessToken),
                         JsonPayload, "PATCH"));
  if(root["id"].asString() == "")return 0;
  return 1;
}
bool discordClient::deleteMessage(Message message){
  if(m_accessToken == "")return false;
  this -> request("/channels/" + message.channelId().asString() + "/messages/" + message.id().asString(),
                  util::generateHeader("application/json", m_accessToken), "DELETE");
  return 1;
}
bool discordClient::createReaction(Message message, std::string emoji){
  if(m_accessToken == "")return false;
  this -> request("/channels/" + message.channelId().asString() + "/messages/" + message.id().asString() + "/reactions/" + emoji + "/@me",
                  util::generateHeader("application/json", m_accessToken), "", "PUT");
  return 1;
}
bool discordClient::deleteOwnReaction(Message message, std::string emoji){
  if(m_accessToken == "")return false;
  this -> request("/channels/" + message.channelId().asString() + "/messages/" + message.id().asString() + "/reactions/" + emoji + "/@me",
                  util::generateHeader("application/json", m_accessToken), "DELETE");
  return 1;
}
bool discordClient::triggerTypingIndicator(){
  if(m_accessToken == "")return false;
  this -> request("/channels/" + m_currentChannel.id().asString() + "/typing",
                  util::generateHeader("application/json", m_accessToken), "", "POST");
  return 1;
}

std::vector<Guild> discordClient::getCurrentUserGuilds(){
  std::vector<Guild> ret;
  Json::Value root;

  if(m_accessToken == "")return ret;

  std::list<std::string> header;
  header.push_back("Content-Type: application/x-www-form-urlencoded");
  header.push_back("Authorization: " + m_accessToken);

  root = util::StringToJson(this -> request("/users/@me/guilds", header, "GET"));
  if(root.size() <= 0) return ret;

  for(int i = 0; i < root.size(); i++){
    ret.push_back(Guild(util::JsonToString(root[i])));
  }
  return ret;
}
bool discordClient::leaveGuild(Guild guild){
  if(m_accessToken == "")return false;
  this -> request("/users/@me/guilds/" + guild.id().asString(),
                  util::generateHeader("application/json", m_accessToken), "DELETE");
  return 1;
}


bool discordClient::connect(){
  if(!m_connection -> connect("wss://gateway.discord.gg/?v=6&encoding=json", &m_gatewayResponse)){
    return 0;
  }

  while(m_connection -> get_metadata() -> get_status() != "Connected"){
    std::this_thread::sleep_for(std::chrono::milliseconds((5)));
  }

  while(m_gatewayResponse == ""){
    std::this_thread::sleep_for(std::chrono::milliseconds((5)));
  }

  m_heartbeatInterval = util::StringToJson(m_gatewayResponse)["d"]["heartbeat_interval"].asInt();

  m_heartbeatDemon = new std::thread(&discordClient::heart, this);
  m_handlerDemon = new std::thread(&discordClient::handler, this);
  m_heartbeatDemon -> detach();
  m_handlerDemon -> detach();

  m_gatewayResponse = "";
  if(!m_connection -> send(util::JsonToString(util::generateIdentificationPayload(m_accessToken)))){
    return 0;
  }
  while(m_gatewayResponse == ""){
    std::this_thread::sleep_for(std::chrono::milliseconds((5)));
  }

  m_heartbeatAcked = true;
  return 1;
}

bool discordClient::reconnect(){
  if(!m_connection -> close(websocketpp::close::status::going_away, "")){
    return 0;
  }
  while(m_connection -> get_metadata() -> get_status() != "Closed"){
    std::this_thread::sleep_for(std::chrono::milliseconds((5)));
  }

  delete m_connection;
  std::this_thread::sleep_for(std::chrono::milliseconds((100)));
  m_connection = new websocket_endpoint();

  if(!m_connection -> connect("wss://gateway.discord.gg/?v=6&encoding=json", &m_gatewayResponse)){
    return 0;
  }
  while(m_connection -> get_metadata() -> get_status() != "Connected"){
    std::this_thread::sleep_for(std::chrono::milliseconds((5)));
  }
  while(m_gatewayResponse == ""){
    std::this_thread::sleep_for(std::chrono::milliseconds((5)));
  }

  m_heartbeatInterval = util::StringToJson(m_gatewayResponse)["d"]["heartbeat_interval"].asInt();

  m_gatewayResponse = "";
  if(!m_connection -> send(util::JsonToString(util::generateIdentificationPayload((m_accessToken))))){
    return 0;
  }
  while(m_gatewayResponse == ""){
    std::this_thread::sleep_for(std::chrono::milliseconds((5)));
  }

  delete m_heartbeatDemon;
  delete m_handlerDemon;
  std::this_thread::sleep_for(std::chrono::milliseconds((100)));

  m_heartbeatDemon = new std::thread(&discordClient::heart, this);
  m_handlerDemon = new std::thread(&discordClient::handler, this);
  m_heartbeatDemon -> detach();
  m_handlerDemon -> detach();

  m_heartbeatAcked = true;
  return 1;
}

bool discordClient::disconnect(){
  if(!m_connection -> close(websocketpp::close::status::going_away, "")){
    return 0;
  }
  while(m_connection -> get_metadata() -> get_status() != "Closed"){
    std::this_thread::sleep_for(std::chrono::milliseconds((5)));
  }
  std::this_thread::sleep_for(std::chrono::milliseconds((100)));
  delete m_heartbeatDemon;
  delete m_handlerDemon;
  delete m_connection;
  return 1;
}

bool discordClient::updateStatus(std::string status){
  if(!m_connection -> send(util::JsonToString(util::generateUpdateStatusPayload(status)))){
    return 0;
  }
  return 1;
}

bool discordClient::updateStatus(std::string status, std::string game){
  if(!m_connection -> send(util::JsonToString(util::generateUpdateStatusPayload(status, game)))){
    return 0;
  }
  return 1;
}

void discordClient::handler(){
  while(m_connection -> get_metadata() -> get_status() == "Connected"){
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    if(m_gatewayResponse != ""){
      int op = util::StringToJson(m_gatewayResponse)["op"].asInt();
      std::string t = util::StringToJson(m_gatewayResponse)["t"].asString();
      std::string d = util::JsonToString(util::StringToJson(m_gatewayResponse)["d"]);

      switch(util::StringToJson(m_gatewayResponse)["op"].asInt()){
      case 0:  // Dispatch
        switch(gatewayEvent.find(t) -> second){
        case READY:
          onReady(d);
          break;
        case RESUMED:
          onResumed(d);
          break;
        case INVALID_SESSION:
          onInvalidSession(d);
          break;
        case CHANNEL_CREATE:
          onChannelCreate(d);
          break;
        case CHANNEL_UPDATE:
          onChannelUpdate(d);
          break;
        case CHANNEL_DELETE:
          onChannelDelete(d);
          break;
        case CHANNEL_PINS_UPDATE:
          onChannelPinsUpdate(d);
          break;
        case GUILD_CREATE:
          onGuildCreate(d);
          break;
        case GUILD_UPDATE:
          onGuildUpdate(d);
          break;
        case GUILD_DELETE:
          onGuildDelete(d);
          break;
        case GUILD_BAN_ADD:
          onGuildBanAdd(d);
          break;
        case GUILD_BAN_REMOVE:
          onGuildBanRemove(d);
          break;
        case GUILD_EMOJI_UPDATE:
          onGuildEmojiUpdate(d);
          break;
        case GUILD_INTEGRATIONS_UPDATE:
          onGuildIntergrationsUpdate(d);
          break;
        case GUILD_MEMBER_ADD:
          onGuildMemberAdd(d);
          break;
        case GUILD_MEMBER_REMOVE:
          onGuildMemberRemove(d);
          break;
        case GUILD_MEMBER_UPDATE:
          onGuildMemberUpdate(d);
          break;
        case GUILD_ROLE_CREATE:
          onGuildRoleCreate(d);
          break;
        case GUILD_ROLE_UPDATE:
          onGuildRoleUpdate(d);
          break;
        case GUILD_ROLE_DELETE:
          onGuildRoleDelete(d);
          break;
        case MESSAGE_CREATE:
          onMessageCreate(d);
          break;
        case MESSAGE_UPDATE:
          onMessageUpdate(d);
          break;
        case MESSAGE_DELETE:
          onMessageDelete(d);
          break;
        case MESSAGE_DELETE_BULK:
          onMessageDeleteBulk(d);
          break;
        case MESSAGE_REACTION_ADD:
          onMessageReactionAdd(d);
          break;
        case MESSAGE_REACTION_REMOVE:
          onMessageReactionRemove(d);
          break;
        case MESSAGE_REACTION_REMOVE_ALL:
          onMessageReactionRemoveAll(d);
          break;
        case PRESENCE_UPDATE:
          onPresenceUpdate(d);
          break;
        case TYPING_START:
          onTypingStart(d);
          break;
        case USER_UPDATE:
          onUserUpdate(d);
          break;
        case VOICE_STATE_UPDATE:
          onVoiceStateUpdate(d);
          break;
        case VOICE_SERVER_UPDATE:
          onVoiceServerUpdate(d);
          break;
        case WEBHOOKS_UPDATE:
          onWebhooksUpdate(d);
          break;
        }
        break;
      case 1:  // Heartbeat
        onHeartbeat();
        break;
      case 7:  // Reconncet
        onReconnect();
        break;
      case 9:  // Invalid Session
        onInvalidSession();
        break;
      case 10: // Hello
        onHello();
        break;
      case 11: // HeartBeat Ack
        onHeartbeatAck();
        m_heartbeatAcked = true;
        break;
      }
      m_gatewayResponse = "";
    }
  }
}
void discordClient::heart(){
  while(m_connection -> get_metadata() -> get_status() == "Connected"){
    for(int i = 0; i < m_heartbeatInterval; i++){
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
      if(m_connection -> get_metadata() -> get_status() != "Connected")break;
    }
    if(m_connection -> get_metadata() -> get_status() != "Connected")break;
    if(!m_heartbeatAcked){
      this -> reconnect();
    }
    m_connection -> send(util::JsonToString(util::generateHeartbeatPayload()));
    m_heartbeatAcked = 0;
  }
}



void discordClient::loadServer(){
  if(m_accessToken == "")return;
  Json::Value root;

  root = util::StringToJson(this -> request("/users/@me/guilds",
                         util::generateHeader("application/json", m_accessToken),
                         "GET"));
  if(root.size() <= 0) return;

  m_servers.clear();

  for(int i = 0; i < root.size(); i++){
    m_servers.push_back(Guild(util::JsonToString(root[i])));
  }
}

bool discordClient::setServer(std::string serverName){
  for(int i = 0; i < m_servers.size(); i++){
    if(m_servers[i].name() == serverName){
      m_currentServer = m_servers[i];
      return true;
    }
  }
  return false;
}

void discordClient::loadChannel(){
  if(m_accessToken == "")return;
  Json::Value root;

  if(m_currentServer.id().asString() == ""){
    root = util::StringToJson(this -> request("/users/@me/channels",
                                              util::generateHeader("application/json", m_accessToken),
                                              "GET"));
    if(root.size() <= 0) return;

    m_channels.clear();

    for(int i = 0; i < root.size(); i++){
      Json::Value temp = root[i];
      temp["name"] = temp["recipients"][0]["username"];
      m_channels.push_back(Channel(util::JsonToString(temp)));
    }
  } else {
    root = util::StringToJson(this -> request("/guilds/" + m_currentServer.id().asString() + "/channels",
                          util::generateHeader("application/json", m_accessToken),
                          "GET"));
    if(root.size() <= 0) return;

    m_channels.clear();

    for(int i = 0; i < root.size(); i++){
      m_channels.push_back(Channel(util::JsonToString(root[i])));
    }
  }
}

bool discordClient::setChannel(std::string channelName){
  for(int i = 0; i < m_channels.size(); i++){
      if(m_channels[i].name() == channelName){
          m_currentChannel = m_channels[i];
          return true;
      }
  }
  return false;
}

void discordClient::loadMessages(std::string *ret){
  std::list<std::string> header;
  header.push_back("Content-Type: appliaction/x-www-form-urlencoded");
  header.push_back("Authorization: " + m_accessToken);

  *ret = this -> request(
              "/channels/" + m_currentChannel.id().asString() + "/messages",
              header, "GET");
}

void discordClient::loadMessage(std::string messageId, std::string *ret){
	std::list<std::string> header;
  header.push_back("Content-Type: appliaction/x-www-form-urlencoded");
  header.push_back("Authorization: " + m_accessToken);

  *ret = this -> request(
              "/channels/" + m_currentChannel.id().asString()
              + "/messages?around=" + messageId + "&limit=1",
              header, "GET");
}

bool discordClient::sendMessage(std::string message){
  Json::Value root;

  std::list<std::string> header;
  header.push_back("Content-Type: application/json");
  header.push_back("Authorization: " + m_accessToken);

  std::string JsonPayload = "{\"content\": \"" + message + "\",\"tts\": false}";

  root = util::StringToJson(this -> request(
              "/channels/" + m_currentChannel.id().asString() + "/messages",
              header, JsonPayload, "POST"));

  if(root["code"].asString() != ""){
      return false;
  }

  return true;
}

std::string discordClient::token(){
  return m_accessToken;
}
