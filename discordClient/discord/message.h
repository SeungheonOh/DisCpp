#pragma once
#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <sstream>
#include <json/json.h>
#include "snowflake.h"
#include "reaction.h"
#include "guildmember.h"
#include "user.h"

class Message{
public:
  Snowflake m_id;
	Snowflake m_channelId;
	Snowflake m_guildId;
	User m_author;
  GuildMember m_member;
	std::string m_content;
  std::string m_timestamp;
	std::vector<User> m_mentions;
  std::vector<Reaction> m_reactions;
  int m_mentionCount;
  int m_reactionCount;
	bool m_tts;
	bool m_mentionEveryone;
	bool m_pinned;

	Message(std::string s);
	std::string content() const;
  std::string timestamp() const;
  const Snowflake& id() const;
  const Snowflake& channelId() const;
  const Snowflake& guildId() const;
	const User& author() const;
  const GuildMember& member() const;
	const std::vector<User>& mentions() const;
	const std::vector<Reaction>& reactions() const;
};

#endif
