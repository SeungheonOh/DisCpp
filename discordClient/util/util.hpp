#ifndef UTIL_H
#define UTIL_H

#include <list>
#include <json/json.h>

class util{
public:
    static Json::Value StringToJson(std::string strIn){
        Json::Value ret;
        std::stringstream temp;
        temp << strIn;
        temp >> ret;
        return ret;
    }
    static std::string JsonToString(Json::Value jsonIn){
        Json::StreamWriterBuilder builder;
        builder.settings_["indentation"] = "";

        return Json::writeString(builder, jsonIn);
    }
    static Json::Value generateHeartbeatPayload(){
        Json::Value heartbeat;
	    heartbeat["op"] = 1;
	    heartbeat["d"] = "null";

        return heartbeat;
    }
    static Json::Value generateHeartbeatPayload(int d){
        Json::Value heartbeat;
	    heartbeat["op"] = 1;
	    heartbeat["d"] = d;

        return heartbeat;
    }
    static Json::Value generateResumePayload(std::string token, std::string sessionId, int seq){
      Json::Value resume;
      resume["d"]["seq"] = seq;
      resume["d"]["session_id"] = sessionId;
      resume["d"]["token"] = token;
      resume["op"] = 6;

      return resume;
    }
    static Json::Value generateIdentificationPayload(std::string token){
        Json::Value identification;
	    identification["d"]["token"] = token;
	    identification["d"]["properties"]["$os"] = "linux";
	    identification["d"]["properties"]["$browser"] = "DisCpp";
	    identification["d"]["properties"]["$device"] = "DisCpp";
	    identification["op"] = 2;

	    return identification;
    }
  static Json::Value generateUpdateStatusPayload(std::string stat){
    Json::Value userStatus;
    userStatus["d"]["since"] = 91879201;
    userStatus["d"]["status"] = stat;
    userStatus["d"]["afk"] = false;
    userStatus["op"] = 3;

    return userStatus;
  }
  static Json::Value generateUpdateStatusPayload(std::string stat, std::string game){
    Json::Value userStatus;
    userStatus["d"]["since"] = 91879201;
    userStatus["d"]["game"]["name"] = game;
    userStatus["d"]["game"]["type"] = 0;
    userStatus["d"]["status"] = stat;
    userStatus["d"]["afk"] = false;
    userStatus["op"] = 3;

    return userStatus;
  }
  static std::list<std::string> generateHeader(std::string contentType, std::string token){
    std::list<std::string> header;
    header.push_back("Content-Type: " + contentType);
    header.push_back("Authorization: " + token);
    return header;
  }
};

#endif
