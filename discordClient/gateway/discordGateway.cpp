#include "discordGateway.h"

discordGateway::discordGateway(std::string token, std::string gatewayLink, std::string *messagePtr) 
	: m_token(token), 
	m_gatewayLink(gatewayLink),
	m_messagePtr(messagePtr)
{
	m_connection = new websocket_endpoint();
}

discordGateway::~discordGateway(){
	m_connection -> destruct();
	m_refreshDemon -> join();	
  delete m_handlerDemon;
  delete m_refreshDemon;
}

bool discordGateway::start(){
	if(!m_connection -> connect(m_gatewayLink, m_messagePtr)){
		return false;
	}

	while(1){
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
		if(m_connection -> get_metadata() -> get_status() == "Connected")
			break;
	}

	while(*m_messagePtr == ""){
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}

	m_heartbeat_interval = util::StringToJson(*m_messagePtr)["d"]["heartbeat_interval"].asInt();

	if(!m_connection -> send(util::JsonToString(util::generateIdentificationPayload(m_token)))){
		std::cout << "identification failed" << std::endl;
		return false;
	}

	int i = m_timeout;
	*m_messagePtr = "";
	while(i){
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		if(*m_messagePtr != "") break;
		i--;
	}
  std::cout << m_messagePtr << std::endl;

	if(i == 0){
		return 0;
	}

  m_sessionId = util::StringToJson(*m_messagePtr)["d"]["session_id"].asString();

	m_refreshDemon = new std::thread(&discordGateway::heart, this);
	m_handlerDemon = new std::thread(&discordGateway::handler, this);
	return true;
}

bool discordGateway::stop(){
	m_refreshDemon -> join();
	return true;
}

bool discordGateway::updateStatus(std::string stat){
	if(!m_connection -> send(util::JsonToString(util::generateUpdateStatusPayload(stat)))){
		return false;
	}
	return true;
}

bool discordGateway::updateStatus(std::string stat, std::string gameName){
  if(!m_connection -> send(util::JsonToString(util::generateUpdateStatusPayload(stat, gameName)))){
		return false;
	}
	return true;
}

void discordGateway::handler(){
  while(1){
    if(*m_messagePtr != ""){
      std::cout << util::StringToJson(*m_messagePtr)["t"] << std::endl;
      *m_messagePtr = "";
    }
  }
}

void discordGateway::heart(){
	int d;
	while(1){
		std::this_thread::sleep_for(std::chrono::milliseconds(m_heartbeat_interval));
		m_connection -> send(util::JsonToString(util::generateHeartbeatPayload()));
	}
}
