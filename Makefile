OBJECTS = main.o cli.o discordClient/discord/channel.o discordClient/discord/guild.o discordClient/discord/message.o discordClient/discord/reaction.o discordClient/discord/snowflake.o discordClient/discord/user.o discordClient/gateway/websocket/connection_metadata.o discordClient/gateway/websocket/websocket_endpoint.o discordClient/discordClient.o discordClient/gateway/discordGateway.o
SRC = main.cpp cli.cpp discordClient/discord/channel.cpp discordClient/discord/guild.cpp discordClient/discord/message.cpp discordClient/discord/reaction.cpp discordClient/discord/snowflake.cpp discordClient/discord/user.o discordClient/gateway/websocket/connection_metadata.cpp discordClient/gateway/websocket/websocket_endpoint.cpp discordClient/discordClient.cpp discordClient/gatway/discordGateway.cpp
LIB = -lboost_system -lpthread -lssl -lcrypto -ljsoncpp -lcurl -lcurlpp

CXX = g++
CFLAGS = -g
TARGET = test

$(TARGET) : $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS) $(LIB)

.cpp.o:
	$(CXX) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	$(RM) -f $(OBJECTS) $(TARGET) core

main.o : main.cpp
cli.o : cli.cpp
channel.o : channel.cpp
guild.o : guild.cpp
message.o : message.cpp
reaction.o : reaction.cpp
snowflake.o : snowflake.cpp
user.o : user.cpp
discordClient.o : discordClient.cpp
connection_metadata.o : connection_metadata.cpp
websocket_endpoint.o : websocket_endpoint.cpp
discordGateway.o : discordGateway.cpp

