#pragma once
#ifndef GUILD_H
#define GUILD_H

#include <string>
#include <sstream>
#include <json/json.h>
#include "snowflake.h"

class Guild{
public:
  Snowflake m_id;
  Snowflake m_ownerId;
  std::string m_name;

  Guild();
  Guild(std::string s);
  const Snowflake& id() const;
  const Snowflake& owner() const;
  std::string name() const;
};

#endif
