#pragma once
#ifndef USER_H
#define USER_H

#include <string>
#include <sstream>
#include <json/json.h>
#include "snowflake.h"

class User{
public:
  Snowflake m_id;
  std::string m_username;
  std::string m_email;
  int m_discriminator;
  bool m_bot;

  User();
  User(std::string s);
  const Snowflake& id() const;
  std::string username() const;
};

#endif
