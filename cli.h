#ifndef cli_H
#define cli_H

#include <iostream>
#include <map>
#include <vector>
#include <thread>
#include <termios.h>
#include <unistd.h>


class Cli{
public:
  const std::map<std::string, int> events={
                                            {":login", 0},
                                            {":quit", 1},
                                            {":exit", 1},
                                            {":servers", 2},
                                            {":channels", 3},
                                            {":clear", 4},
                                            {":status", 5},
                                            {":reconnect", 6},
                                            {":users", 7},
                                            {":edit", 8},
                                            {":delete", 9},
                                            {":reload", 10},
  };
  std::string m_promptTemplete;
  std::string m_prompt;
  std::string m_buffer;
  std::thread *m_interface;

  Cli();
  ~Cli();

  void startInterface();

  int getch();
  std::string repl(std::string s, std::string keyword, std::string replacement);

  void print(std::string s);

  void interface();
  virtual void onLogin(std::string id, std::string pass){};
  virtual void onQuit(){};
  virtual void onServers(std::string s){};
  virtual void onChannels(std::string s){};
  virtual void onSend(std::string s){};
  virtual void onStatus(std::string s){};
  virtual void onReconnect(){};
  virtual void onUsers(){};
  virtual void onEdit(std::string s){};
  virtual void onDelete(int i){};
  virtual void onReload(){};
};

#endif
