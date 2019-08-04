#include "reaction.h"

Reaction::Reaction(std::string s){
  Json::Value root;
  std::stringstream temp;
  temp << s;
  temp >> root;

  m_count = root["count"].asUInt();
  m_me = root["me"].asBool();
  m_emoji = root["emoji"]["name"].asString();
}

unsigned int Reaction::count() const{
  return m_count;
}

bool Reaction::isMe() const{
  return m_me;
}

std::string Reaction::emoji() const{
  return m_emoji;
}
