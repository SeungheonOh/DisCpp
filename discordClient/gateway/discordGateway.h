#ifndef DISCORDGATEWAY_H
#define DISCORDGATEWAY_H

#include "websocket/websocket_endpoint.h"
#include "../util/util.hpp"

class discordGateway{
public:
	discordGateway(std::string token, std::string gatewayLink, std::string *messagePtr) ;
	~discordGateway();

	bool start();
	bool stop();

	bool updateStatus(std::string stat);
	bool updateStatus(std::string stat, std::string gameName);

private:
  void handler();
	void heart();
	websocket_endpoint *m_connection;
  std::thread *m_handlerDemon;
	std::thread *m_refreshDemon;
	std::string m_gatewayLink;
	std::string m_token;
	std::string m_userStatus;
	std::string *m_messagePtr;
  std::string m_sessionId;
	int m_heartbeat_interval;
	int m_timeout = 1000;
};

#endif
