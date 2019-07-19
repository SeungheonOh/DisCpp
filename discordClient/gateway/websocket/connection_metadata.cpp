#include "connection_metadata.h"

connection_metadata::connection_metadata(websocketpp::connection_hdl hdl, std::string uri, std::string *message) 
: m_hdl(hdl)
, m_uri(uri)
, m_message(message)
, m_status("Connecting")
{}

void connection_metadata::on_open(client *c, websocketpp::connection_hdl hdl){
	m_status = "Connected";

	client::connection_ptr con = c -> get_con_from_hdl(hdl);
	m_server = con -> get_response_header("Server"); 
}

void connection_metadata::on_fail(client *c, websocketpp::connection_hdl hdl){
	m_status = "Failed";

	client::connection_ptr con = c -> get_con_from_hdl(hdl);
	m_server = con -> get_response_header("Server");
	m_error_reason = con -> get_ec().message();
}

void connection_metadata::on_close(client *c, websocketpp::connection_hdl hdl){
	m_status = "Closed";

	client::connection_ptr con = c -> get_con_from_hdl(hdl);
	std::stringstream s;
	s << "close code : " << con -> get_remote_close_code() << " ("
		<< websocketpp::close::status::get_string(con -> get_remote_close_code())
		<< "), close reason: " << con -> get_remote_close_reason();
	m_error_reason = s.str();
}

void connection_metadata::on_message(websocketpp::connection_hdl, client::message_ptr msg){
	*m_message = msg -> get_payload();
}

websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context> connection_metadata::on_tls_init(websocketpp::connection_hdl hdl){
	websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context> ctx =
		websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::sslv23);

	try{
		ctx -> set_options(boost::asio::ssl::context::default_workarounds |
				boost::asio::ssl::context::no_sslv2 |
				boost::asio::ssl::context::no_sslv3 |
				boost::asio::ssl::context::single_dh_use);

	} catch(std::exception& e){
		std::cout << e.what() << std::endl;
	}

	return ctx;
}


websocketpp::connection_hdl connection_metadata::get_hdl() const{
	return m_hdl;
}

std::string connection_metadata::get_status() const{
	return m_status;
}
