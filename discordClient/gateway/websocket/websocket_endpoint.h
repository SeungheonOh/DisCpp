#ifndef WEBSOCKET_ENDPOINT_H
#define WEBSOCKET_ENDPOINT_H

#include <string>

#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>

#include <websocketpp/common/thread.hpp>
#include <websocketpp/common/memory.hpp>

#include "connection_metadata.h"

typedef websocketpp::client<websocketpp::config::asio_tls_client> client;


class websocket_endpoint {
public:
	websocket_endpoint();

	~websocket_endpoint();

	bool connect(std::string const &uri, std::string *message);

	bool close(websocketpp::close::status::value code, std::string reason);

	bool send(std::string message);

	void destruct();

	connection_metadata::ptr get_metadata() const;

private:
	client m_endpoint;
	websocketpp::lib::shared_ptr<websocketpp::lib::thread> m_thread;
	connection_metadata::ptr m_connection;

};

#endif
