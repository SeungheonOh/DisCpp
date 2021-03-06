#include <json/json.h>
#include "discordClient/gateway/discordGateway.hpp"

#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <thread>


int main(){
	std::string tok = "";
	std::string message;
	discordGateway *gateway = new discordGateway(tok, "wss://gateway.discord.gg/?v=6&encoding=json", &message);
	
	if(!gateway -> start()){
		std::cout << "gateway failure" << std::endl;
		return 0;
	}

	if(!gateway -> updateStatus("online", "Nothing")){
		std::cout << "updateStatus failure" << std::endl;
		return 0;
	}

	std::cout << "started" << std::endl;


	while(1){
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
		if(message != ""){
			std::cout << util::StringToJson(message) << std::endl;
			message = "";
		}
	}
}

