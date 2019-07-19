#include "guild.h"

Guild::Guild(){
}

Guild::Guild(std::string s){
  Json::Value root;
  std::stringstream temp;
  temp << s;
  temp >> root;

  m_id = Snowflake(root["id"].asString());
  m_ownerId = Snowflake(root["owner_id"].asString());
  m_name = root["name"].asString();
}

const Snowflake& Guild::id() const{
  return m_id;
}

const Snowflake& Guild::owner() const{
  return m_ownerId;
}

std::string Guild::name() const{
  return m_name;
}
