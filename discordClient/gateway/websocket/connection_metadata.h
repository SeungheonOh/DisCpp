#ifndef CONNECTION_METADATA_H
#define CONNECTION_METADATA_H

#include <string>

#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>

#include <websocketpp/common/thread.hpp>
#include <websocketpp/common/memory.hpp>

typedef websocketpp::client<websocketpp::config::asio_tls_client> client;

class connection_metadata{
public:
    typedef websocketpp::lib::shared_ptr<connection_metadata> ptr;

	connection_metadata(websocketpp::connection_hdl hdl, std::string uri, std::string *message);

	void on_open(client *c, websocketpp::connection_hdl hdl);

	void on_fail(client *c, websocketpp::connection_hdl hdl);

	void on_close(client *c, websocketpp::connection_hdl hdl);

	void on_message(websocketpp::connection_hdl, client::message_ptr msg);

	static websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context> on_tls_init(websocketpp::connection_hdl hdl);

	websocketpp::connection_hdl get_hdl() const;

	std::string get_status() const;

private:
	websocketpp::connection_hdl m_hdl;
	std::string m_status;
	std::string m_uri;
	std::string m_server;
	std::string m_error_reason;

	std::string *m_message;
};

#endif
