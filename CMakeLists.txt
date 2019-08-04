cmake_minimum_required(VERSION 3.0)

project(DisCpp)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_C_COMPILER "g++")

find_package(Boost 1.50 REQUIRED)

find_path(JsonCpp_INCLUDE_DIR
  NAMES json/json.h
  PATH_SUFFIXES jsoncpp
)
include_directories($(JsonCpp_INCLUDE_DIR))
include_directories($(Boost_INCLUDE_DIRS))


set(SOURCE_FILES
main.cpp cli.cpp discordClient/discord/channel.cpp discordClient/discord/guild.cpp discordClient/discord/message.cpp discordClient/discord/reaction.cpp discordClient/discord/snowflake.cpp discordClient/discord/guildmember.cpp discordClient/discord/user.cpp discordClient/gateway/websocket/connection_metadata.cpp discordClient/gateway/websocket/websocket_endpoint.cpp discordClient/discordClient.cpp
)

add_executable(
  ${CMAKE_PROJECT_NAME}
  ${SOURCE_FILES}
)

target_link_libraries(
  ${CMAKE_PROJECT_NAME}
  boost_system
  pthread
  jsoncpp
  ssl
  crypto
  curl
  curlpp
)
