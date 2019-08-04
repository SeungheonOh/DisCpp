#pragma once
#ifndef CHANNEL_H
#define CHANNEL_H

#include <string>
#include <sstream>
#include <json/json.h>
#include "snowflake.h"

class Channel{
public:
  Snowflake m_id;
  Snowflake m_guildId;
  Snowflake m_lastMessageId;
  Snowflake m_parentId;
  int m_type;
  int m_position;
  int m_rateLimitPerUser;
  std::string m_name;
  std::string m_topic;
  bool m_nsfw;

  Channel();
  Channel(std::string s);
  std::string name() const;
  std::string topic() const;
  int position() const;
  const Snowflake& id() const;
};

#endif
