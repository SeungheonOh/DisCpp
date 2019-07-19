#pragma once
#ifndef SNOWFLAKE_H
#define SNOWFLAKE_H

#include <string>
#include <cstdint>

class Snowflake{
public:
	uint64_t m_id;

	Snowflake();
	Snowflake(uint64_t id);
  Snowflake(std::string id);
	std::string asString() const;

	bool operator==(const Snowflake& varToCompare) const;
	bool operator!=(const Snowflake& varToCompare) const;
};

#endif
