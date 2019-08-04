#include "message.h"

Message::Message(std::string s){
  Json::Value root;
  Json::StreamWriterBuilder Writer;
  Writer["indentation"] = "";
  std::stringstream temp;
  temp << s;
  temp >> root;

  m_id = Snowflake(root["id"].asString());
  m_channelId = Snowflake(root["channel_id"].asString());
  m_guildId = Snowflake(root["guild_id"].asString());
  m_author = User(Json::writeString(Writer, root["author"]));
  m_member = GuildMember(Json::writeString(Writer, root["member"]));
  m_content = root["content"].asString();
  m_timestamp = root["timestamp"].asString();

  m_mentionCount = root["mentions"].size();
  m_reactionCount = root["reactions"].size();

  if(root["mentions"].size() > 0){
    for(int i = 0; i < root["mentions"].size(); ++i){
      m_mentions.push_back(User(Json::writeString(Writer, root["mentions"][i])));
    }
  }
  if(root["reactions"].size() > 0){
    for(int i = 0; i < root["reactions"].size(); ++i){
      m_reactions.push_back(Reaction(Json::writeString(Writer, root["reactions"][i])));
    }
  }
}

std::string Message::content() const{
  return m_content;
}

std::string Message::timestamp() const{
  return m_timestamp;
}

const Snowflake& Message::id() const{
  return m_id;
}

const Snowflake& Message::channelId() const{
  return m_channelId;
}

const Snowflake& Message::guildId() const{
  return m_guildId;
}

const User& Message::author() const{
  return m_author;
}

const GuildMember& Message::member() const{
  return m_member;
}

const std::vector<User>& Message::mentions() const{
  return m_mentions;
}

const std::vector<Reaction>& Message::reactions() const{
  return m_reactions;
}
