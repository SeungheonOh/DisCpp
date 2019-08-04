#include "snowflake.h"

Snowflake::Snowflake() : m_id(0){
}

Snowflake::Snowflake(uint64_t id) : m_id(id){
}

Snowflake::Snowflake(std::string id){
  if(id.length() != 18){
    m_id = 0;
    return;
  }
  m_id = std::stoull(id, nullptr, 10);
}

std::string Snowflake::asString() const{
  if(m_id == 0)return "";
  return std::to_string(m_id);
}

bool Snowflake::operator==(const Snowflake& varToCompare) const{
  return varToCompare.m_id == m_id;
}

bool Snowflake::operator!=(const Snowflake& varToCompare) const{
  return varToCompare.m_id != m_id;
}
