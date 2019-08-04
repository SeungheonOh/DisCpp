#pragma once
#ifndef GUILDMEMBER_H
#define GUILDMEMBER_H

#include <string>
#include <sstream>
#include <json/json.h>
#include "snowflake.h"
#include "user.h"

class GuildMember{
public:
  User m_user;
  std::string m_nick;
  std::string m_roles;
  std::string m_joinedAt;
  std::string m_premiumSince;
  bool m_deaf;
  bool m_mute;

  GuildMember();
  GuildMember(std::string s);
  const User& user() const;
  std::string nick() const;
};

#endif
