#include "guildmember.h"

GuildMember::GuildMember(){
}

GuildMember::GuildMember(std::string s){
  Json::Value root;
  Json::StreamWriterBuilder Writer;
  Writer["indentation"] = "";
  std::stringstream temp;
  temp << s;
  temp >> root;

  m_user = User(Json::writeString(Writer, root["user"]));
  m_nick = root["nick"].asString();
  m_joinedAt = root["joined_at"].asString();
  m_premiumSince = root["premium_since"].asString();
  m_deaf = root["deaf"].asBool();
  m_mute = root["mute"].asBool();
}

const User& GuildMember::user() const {
  return m_user;
}

std::string GuildMember::nick() const {
  return m_nick;
}
