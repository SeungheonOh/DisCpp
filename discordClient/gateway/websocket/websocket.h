#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>

#include <websocketpp/common/thread.hpp>
#include <websocketpp/common/memory.hpp>

typedef websocketpp::client<websocketpp::config::asio_tls_client> client;

#include "connection_metadata.h"
#include "websocket_endpoint.h"

//-lstdc++ -lboost_system -lpthread -lssl -lcrypto
