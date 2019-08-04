#include "user.h"

User::User(){
}

User::User(std::string s){
  Json::Value root;
  std::stringstream temp;
  temp << s;
  temp >> root;

  m_id = Snowflake(root["id"].asString());
  m_username = root["username"].asString();
  m_email = root["email"].asString();
  //m_discriminator = std::stoi(root["discriminator"].asString(), nullptr, 10);  TODO FIX this 
  m_bot = root["bot"].asBool();
}

const Snowflake& User::id() const{
  return m_id;
}

std::string User::username() const{
  return m_username;
}
