#include "cli.h"

Cli::Cli(){
  //Cli::Cli() : m_promptTemplete("[%s%c]>"){
  std::cout << "\033[s";
}

Cli::~Cli(){
  m_interface -> join();
  delete m_interface;
}

void Cli::startInterface(){
  m_prompt = m_promptTemplete;
  m_prompt = repl(m_prompt, "%s", "");
  m_prompt = repl(m_prompt, "%c", "");
  std::cout << "\033[s" << m_prompt;
  m_interface = new std::thread(&Cli::interface, this);
  m_interface -> detach();
}

std::string Cli::repl(std::string s, std::string keyword, std::string replacement){
  for(int i = 0; i < s.length(); i++){
    int count = 0;
    for(int j = 0; j < keyword.length(); j++){
      if(s[i + j] == keyword[j]) count++;
    }
    if(count == keyword.length()){
      s.replace(i, keyword.length(), replacement);
      return s;
    }
  }
  return s;
}

int Cli::getch(){
  int ch;
  struct termios t_old, t_new;

  tcgetattr(STDIN_FILENO, &t_old);
  t_new = t_old;
  t_new.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &t_new);

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &t_old);
  return ch;
}

void Cli::print(std::string s){
  std::cout << "\033[u";
  for(int i = 0; i < m_buffer.length() + m_prompt.length(); i++){
    std::cout << " ";
  }
  std::cout << "\033[u";
  std::cout << s << std::endl << "\033[s";
  std::cout << repl(m_prompt, "%c", "") << m_buffer;
  std::flush(std::cout);
}


void Cli::interface(){
  int count = 1;
  while(1){
    char c;
    int cursor = 0;
    while((c = getch()) != 10){
      std::cout << "\033[u";
      for(int i = 0; i < m_buffer.length() + m_prompt.length(); i++){
        std::cout << " ";
      }
      std::cout << "\033[u";
      switch(c){
      case 127:
        if(cursor - 1 < 0)break;
        cursor--;
        m_buffer.pop_back();
        break;
      case 27:
        m_buffer.clear();
        cursor = 0;
        break;
      case 8: // Arrow left
        if(cursor - 1 < 0)break;
        cursor--;
        break;
      case 12: // Arrow right
        if(cursor + 1 > m_buffer.length())break;
        cursor++;
        break;
      default:
        m_buffer.insert(cursor++, 1, c);
        break;
      }

      std::cout << repl(m_prompt, "%c", ""); // << m_buffer;
      for(int i = 0; i < m_buffer.length(); i++){
        if(i == cursor){
          std::cout << "\033[101m" << m_buffer[i] << "\033[0m";
          continue;
        }
        std::cout << m_buffer[i];
      }
    }

    std::cout << "\033[u";
    for(int i = 0; i < m_buffer.length() + m_prompt.length(); i++){
      std::cout << " ";
    }
    std::cout << "\033[u";

    std::vector<std::string> userinput;
    std::string temp = "";

    for(int i = 0; i < m_buffer.length(); i++){
      if(m_buffer[i] == ' '){
        userinput.push_back(temp);
        temp = "";
        continue;
      }
      temp += m_buffer[i];
    }
    userinput.push_back(temp);

    temp = m_buffer;
    m_buffer = "";

    if(events.find(userinput[0]) == events.end()){
      onSend(temp);
    } else{
      switch(events.find(userinput[0]) -> second){
      case 0:
        if(userinput.size() != 3){
          onLogin("", "");
          break;
        }
        onLogin(userinput[1], userinput[2]);
        break;
      case 1:
        onQuit();
        std::exit(0);
        break;
      case 2:
        if(userinput.size() > 1){
          std::string temp;
          for(int i = 1; i < userinput.size(); i++){
            if(i != 1) temp += " ";
            temp += userinput[i];
          }
          onServers(temp);
        } else{
          onServers("");
        }
        break;
      case 3:
        if(userinput.size() > 1){
          std::string temp;
          for(int i = 1; i < userinput.size(); i++){
            if(i != 1) temp += " ";
            temp += userinput[i];
          }
          onChannels(temp);
        } else{
          onChannels("");
        }
        break;
      case 4:
        std::cout << "\033[2J";
        break;
      case 5:
        if(userinput.size() > 1){
          std::string temp;
          for(int i = 1; i < userinput.size(); i++){
            if(i != 1) temp += " ";
            temp += userinput[i];
          }
          onStatus(temp);
        } else{
          onStatus("");
        }
        break;
      case 6:
        onReconnect();
        break;
      case 7:
        onUsers();
        break;
      case 8:
        if(userinput.size() > 1){
          std::string temp;
          for(int i = 1; i < userinput.size(); i++){
            if(i != 1) temp += " ";
            temp += userinput[i];
          }
          onEdit(temp);
        } else{
          onEdit("");
        }
        break;
      case 9:
        if(userinput.size() > 1){
          bool hasAlphabet = false;
          for(auto c : userinput[1]){
            if(c > 57 || c < 48){
              hasAlphabet = true;
            }
          }
          if(!hasAlphabet)onDelete(std::stoi(userinput[1]));
          else onDelete(-1);
        } else{
          onDelete(-1);
        }
        break;
      case 10:
        onReload();
        break;
      }
    }

    std::cout << "\033[u";
    for(int i = 0; i < m_buffer.length() + m_prompt.length(); i++){
      std::cout << " ";
    }
    std::cout << "\033[u";
    std::cout << "\033[s";
    std::cout << repl(m_prompt, "%c", "") << m_buffer;

    std::flush(std::cout);
  }
}
