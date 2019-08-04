#pragma once
#ifndef REACTION_H
#define REACTION_H

#include <string>
#include <sstream>
#include <json/json.h>

class Reaction{
public:
  unsigned int m_count;
  bool m_me;
  std::string m_emoji;

  Reaction(std::string s);
  unsigned int count() const;
  bool isMe() const;
  std::string emoji() const;
};

#endif
