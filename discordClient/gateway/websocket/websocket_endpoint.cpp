#include "websocket_endpoint.h"

websocket_endpoint::websocket_endpoint(){
	m_endpoint.clear_access_channels(websocketpp::log::alevel::all);
	m_endpoint.clear_error_channels(websocketpp::log::elevel::all);

	m_endpoint.init_asio();
	m_endpoint.start_perpetual();

	m_thread = 
		websocketpp::lib::make_shared<websocketpp::lib::thread>(&client::run, &m_endpoint);
}

websocket_endpoint::~websocket_endpoint(){
	m_endpoint.stop_perpetual();

	if(m_connection -> get_status() != "connected"){
		m_thread -> join();
		return;
	}

	websocketpp::lib::error_code ec;
	m_endpoint.close(m_connection -> get_hdl(), websocketpp::close::status::going_away, "", ec);

	if(ec){
		std::cout << "ERROR in destruction : " << ec.message() << std::endl;
	}

	m_thread -> join();
}

bool websocket_endpoint::connect(std::string const &uri, std::string *message){
	websocketpp::lib::error_code ec;

	m_endpoint.set_tls_init_handler(connection_metadata::on_tls_init);

	client::connection_ptr con = m_endpoint.get_connection(uri, ec);

	if(ec){
		return false;
	}

	connection_metadata::ptr metadata_ptr = 
		websocketpp::lib::make_shared<connection_metadata>
		(con -> get_handle(), uri, message);

	m_connection = metadata_ptr;

	con -> set_open_handler(websocketpp::lib::bind(
				&connection_metadata::on_open,
				metadata_ptr,
				&m_endpoint,
				websocketpp::lib::placeholders::_1
	));
	con -> set_fail_handler(websocketpp::lib::bind(
				&connection_metadata::on_fail,
				metadata_ptr,
				&m_endpoint,
				websocketpp::lib::placeholders::_1
	));
	con -> set_close_handler(websocketpp::lib::bind(
				&connection_metadata::on_close,
				metadata_ptr,
				&m_endpoint,
				websocketpp::lib::placeholders::_1
	));
	con -> set_message_handler(websocketpp::lib::bind(
				&connection_metadata::on_message,
				metadata_ptr,
				websocketpp::lib::placeholders::_1,
				websocketpp::lib::placeholders::_2
	));
	
	con -> set_tls_init_handler(websocketpp::lib::bind(
				&connection_metadata::on_tls_init,
				metadata_ptr
	));
	

	m_endpoint.connect(con);

	return true;
}

bool websocket_endpoint::close(websocketpp::close::status::value code, std::string reason){
	websocketpp::lib::error_code ec;

	m_endpoint.close(m_connection -> get_hdl(), code, reason, ec);
	if(ec){
		return false;
	}
	return true;
}

bool websocket_endpoint::send(std::string message){
	websocketpp::lib::error_code ec;

	m_endpoint.send(m_connection -> get_hdl(), message,
				websocketpp::frame::opcode::text, ec);

	if(ec){
		std::cout << ec.message() << std::endl;
		return false;
	}
	return true;
}

void websocket_endpoint::destruct(){
	websocketpp::lib::error_code ec;
	m_endpoint.close(m_connection -> get_hdl(), websocketpp::close::status::going_away, "", ec);

	if(ec){
		return;
	}

	m_thread -> join();
}

connection_metadata::ptr websocket_endpoint::get_metadata() const{
	return m_connection;
}
