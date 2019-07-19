#ifndef DISCORDCLIENT_H
#define DISCORDCLIENT_H

#include <map>
#include <chrono>
#include <vector>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Easy.hpp>
#include <json/json.h>

#include "gateway/discordGateway.h"
#include "gateway/websocket/websocket_endpoint.h"
#include "util/util.hpp"
#include "discord/discord.h"

enum Events{
            HELLO,
            READY,
            RESUMED,
            INVALID_SESSION,
            CHANNEL_CREATE,
            CHANNEL_UPDATE,
            CHANNEL_DELETE,
            CHANNEL_PINS_UPDATE,
            GUILD_CREATE,
            GUILD_UPDATE,
            GUILD_DELETE,
            GUILD_BAN_ADD,
            GUILD_BAN_REMOVE,
            GUILD_EMOJI_UPDATE,
            GUILD_INTEGRATIONS_UPDATE,
            GUILD_MEMBER_ADD,
            GUILD_MEMBER_REMOVE,
            GUILD_MEMBER_UPDATE,
            GUILD_ROLE_CREATE,
            GUILD_ROLE_UPDATE,
            GUILD_ROLE_DELETE,
            MESSAGE_CREATE,
            MESSAGE_UPDATE,
            MESSAGE_DELETE,
            MESSAGE_DELETE_BULK,
            MESSAGE_REACTION_ADD,
            MESSAGE_REACTION_REMOVE,
            MESSAGE_REACTION_REMOVE_ALL,
            PRESENCE_UPDATE,
            TYPING_START,
            USER_UPDATE,
            VOICE_STATE_UPDATE,
            VOICE_SERVER_UPDATE,
            WEBHOOKS_UPDATE
};

static const std::map<std::string, int> gatewayEvent = {
            {"HELLO", HELLO},                                                 // 0
            {"READY", READY},
            {"RESUMED", RESUMED},
            {"INVALID_SESSION", INVALID_SESSION},
            {"CHANNEL_CREATE", CHANNEL_CREATE},
            {"CHANNEL_UPDATE", CHANNEL_UPDATE},
            {"CHANNEL_DELETE", CHANNEL_DELETE},
            {"CHANNEL_PINS_UPDATE", CHANNEL_PINS_UPDATE},
            {"GUILD_CREATE", GUILD_CREATE},
            {"GUILD_UPDATE", GUILD_UPDATE},                                    // 9
            {"GUILD_DELETE", GUILD_DELETE},
            {"GUILD_BAN_ADD", GUILD_BAN_ADD},
            {"GUILD_BAN_REMOVE", GUILD_BAN_REMOVE},
            {"GUILD_EMOJI_UPDATE", GUILD_EMOJI_UPDATE},
            {"GUILD_INTEGRATIONS_UPDATE", GUILD_INTEGRATIONS_UPDATE},
            {"GUILD_MEMBER_ADD", GUILD_MEMBER_ADD},
            {"GUILD_MEMBER_REMOVE", GUILD_MEMBER_REMOVE},
            {"GUILD_MEMBER_UPDATE", GUILD_MEMBER_UPDATE},
            {"GUILD_ROLE_CREATE", GUILD_ROLE_CREATE},
            {"GUILD_ROLE_UPDATE", GUILD_ROLE_UPDATE},                          // 19
            {"GUILD_ROLE_DELETE", GUILD_ROLE_DELETE},
            {"MESSAGE_CREATE", MESSAGE_CREATE},
            {"MESSAGE_UPDATE", MESSAGE_UPDATE},
            {"MESSAGE_DELETE", MESSAGE_DELETE},
            {"MESSAGE_DELETE_BULK", MESSAGE_DELETE_BULK},
            {"MESSAGE_REACTION_ADD", MESSAGE_REACTION_ADD},
            {"MESSAGE_REACTION_REMOVE", MESSAGE_REACTION_REMOVE},
            {"MESSAGE_REACTION_REMOVE_ALL", MESSAGE_REACTION_REMOVE_ALL},
            {"PRESENCE_UPDATE", PRESENCE_UPDATE},
            {"TYPING_START", TYPING_START},                                    // 29
            {"USER_UPDATE", USER_UPDATE},
            {"VOICE_STATE_UPDATE", VOICE_STATE_UPDATE},
            {"VOICE_SERVER_UPDATE", VOICE_SERVER_UPDATE},
            {"WEBHOOKS_UPDATE", WEBHOOKS_UPDATE}
};

class discordClient{
public:
  std::vector<Guild> m_servers;
  std::vector<Channel> m_channels;

  User m_me;
	Guild m_currentServer;
	Channel m_currentChannel;

	discordClient();
  ~discordClient();

  std::string request(std::string endpoint, std::list<std::string> header, std::string method);
  std::string request(std::string endpoint, std::list<std::string> header, std::string body, std::string method);

	bool login(std::string email, std::string password);
	bool login(std::string email, std::string password, std::string code);
  bool IsTwoFactorAccount(std::string email, std::string password);
  bool connect();
  bool reconnect();
  bool disconnect();

  bool updateStatus(std::string status);
  bool updateStatus(std::string status, std::string game);


	void loadServer();
	bool setServer(std::string serverName);
	void loadChannel();
	bool setChannel(std::string channelName);

  User getCurrentUser();
  User getUser();

  std::vector<Message> getChannelMessages();
  std::vector<Message> getChannelMessages(Snowflake messageId, int limit);
  Message getChannelMessage(Snowflake messageId);
  std::vector<Message> getPinnedMessages();

  bool createMessage(std::string content, bool tts);
  bool editMessage(Message message, std::string content);
  bool deleteMessage(Message message);
  bool createReaction(Message message, std::string emoji);
  bool deleteOwnReaction(Message message, std::string emoji);
  bool triggerTypingIndicator();

  std::vector<Guild> getCurrentUserGuilds();
  bool leaveGuild(Guild guild);

  /*
  --EVENTS--
  HELLO,
  READY,
  RESUMED,
  INVALIED_SESSION,
  CHANNEL_CREATE,
  CHANNEL_UPDATE,
  CHANNEL_DELETE,
  CHANNEL_PINS_UPDATE,
  GUILD_CREATE,
  GUILD_UPDATE,
  GUILD_DELETE,
  GUILD_BAN_ADD,
  GUILD_BAN_REMOVE,
  GUILD_EMOJI_UPDATE,
  GUILD_INTEGRATIONS_UPDATE,
  GUILD_MEMBER_ADD,
  GUILD_MEMBER_REMOVE,
  GUILD_MEMBER_UPDATE,
  GUILD_ROLE_CREATE,
  GUILD_ROLE_UPDATE,
  GUILD_ROLE_DELETE,
  MESSAGE_CREATE,
  MESSAGE_UPDATE,
  MESSAGE_DELETE,
  MESSAGE_DELETE_BULK,
  MESSAGE_REACTION_ADD,
  MESSAGE_REACTION_REMOVE,
  MESSAGE_REACTION_REMOVE_ALL,
  PRESENCE_UPDATE,
  TYPING_START,
  USER_UPDATE,
  VOICE_STATE_UPDATE,
  VOICE_SERVER_UPDATE,
  WEBHOOKS_UPDATE
  */

  virtual void onReady(const std::string s){};
  virtual void onResumed(const std::string s){};
  virtual void onInvalidSession(const std::string s){};
  virtual void onChannelCreate(const std::string s){};
  virtual void onChannelUpdate(const std::string s){};
  virtual void onChannelDelete(const std::string s){};
  virtual void onChannelPinsUpdate(const std::string s){};
  virtual void onGuildCreate(const std::string s){};
  virtual void onGuildUpdate(const std::string s){};
  virtual void onGuildDelete(const std::string s){};
  virtual void onGuildBanAdd(const std::string s){};
  virtual void onGuildBanRemove(const std::string s){};
  virtual void onGuildEmojiUpdate(const std::string s){};
  virtual void onGuildIntergrationsUpdate(const std::string s){};
  virtual void onGuildMemberAdd(const std::string s){};
  virtual void onGuildMemberRemove(const std::string s){};
  virtual void onGuildMemberUpdate(const std::string s){};
  virtual void onGuildRoleCreate(const std::string s){};
  virtual void onGuildRoleUpdate(const std::string s){};
  virtual void onGuildRoleDelete(const std::string s){};
  virtual void onMessageCreate(const std::string s){};
  virtual void onMessageUpdate(const std::string s){};
  virtual void onMessageDelete(const std::string s){};
  virtual void onMessageDeleteBulk(const std::string s){};
  virtual void onMessageReactionAdd(const std::string s){};
  virtual void onMessageReactionRemove(const std::string s){};
  virtual void onMessageReactionRemoveAll(const std::string s){};
  virtual void onPresenceUpdate(const std::string s){};
  virtual void onTypingStart(const std::string s){};
  virtual void onUserUpdate(const std::string s){};
  virtual void onVoiceStateUpdate(const std::string s){};
  virtual void onVoiceServerUpdate(const std::string s){};
  virtual void onWebhooksUpdate(const std::string s){};


  virtual void onHeartbeat(){};
  virtual void onReconnect(){};
  virtual void onInvalidSession(){};
  virtual void onHello(){};
  virtual void onHeartbeatAck(){};


	void loadMessages(std::string *ret);
	void loadMessage(std::string messageId, std::string *ret);

	bool sendMessage(std::string message);


  std::string token();


private:
  std::string m_accessToken;
	std::string m_api = "https://discordapp.com/api";
  discordGateway *m_gateway;

  bool m_heartbeatAcked;
  bool m_heartbeatOn;
  bool m_handlerOn;
  bool m_isConnected;
  int m_timeout;
  int m_heartbeatInterval;
  int m_lastSequenceNumber;
  std::thread *m_heartbeatDemon;
  std::thread *m_handlerDemon;
  std::string m_gatewayResponse;
  std::string m_sessionId;
  websocket_endpoint *m_connection;

  void heart();
  void handler();

};

#endif
