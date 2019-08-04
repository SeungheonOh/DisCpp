#include "channel.h"

Channel::Channel(){
}

Channel::Channel(std::string s){
  Json::Value root;
  std::stringstream temp;
  temp << s;
  temp >> root;

  m_id = Snowflake(root["id"].asString());
  m_guildId = Snowflake(root["guild_id"].asString());
  m_lastMessageId = Snowflake(root["last_message_id"].asString());
  m_parentId = Snowflake(root["parent_id"].asString());
  m_type = root["type"].asInt();
  m_position = root["position"].asInt();
  m_rateLimitPerUser = root["rate_limit_per_user"].asInt();
  m_name = root["name"].asString();
  m_topic = root["topic"].asString();
  m_nsfw = root["nsfw"].asBool();
}

std::string Channel::name() const{
  return m_name;
}

std::string Channel::topic() const{
  return m_topic;
}

int Channel::position() const{
  return m_position;
}

const Snowflake& Channel::id() const{
  return m_id;
}
