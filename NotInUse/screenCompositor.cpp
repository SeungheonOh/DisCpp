#include <iostream>
#include <fstream>
#include <termios.h>
#include <unistd.h>
#include <cstring>

#include <sys/ioctl.h>
#include <unistd.h>
#include <chrono>
#include <thread>

class screenCompositor{
public:
	struct winsize m_winsize;

	int m_titleTabSize = 1;
	int m_channelsTabSize = 15;

	std::string m_channels[500];
	int m_channelTop = 0;
	int m_channelView = 0;

	std::string m_messages[100];
	int m_messageTop = 0;
	int m_messageView = 0;

	int m_bottomSpaceLeft = 1;
	std::string m_bottomLinePrompt = "↪";
	int m_unicodeCharacterInPrompt = 1;

	std::string m_bottomLine = "";


	screenCompositor(){
		m_windowSizeDemon = new std::thread(&screenCompositor::updateWindowSize, this);
	}

	void stop(){
		m_windowSizeDemon-> join();
	}

	void addChannel(std::string channelName){
		m_channels[++m_channelTop] = channelName;
	}

	void resetChannel(){
		for(int i = 0; i < m_channelTop; i++){
			m_channels[i].clear();
		}
		m_channelTop = 0;
	}

	void addMessage(std::string message){
		m_messages[++m_messageTop] = message;
	}

	void resetMessage(){
		for(int i = 0; i < m_messageTop; i++){
			m_messages[i].clear();
		}
		m_messageTop= 0;
	}

	void setBottomText(std::string text){
		m_bottomLine = text;
	}

	void render(){
		std::string buffer[m_winsize.ws_row];

		std::cout << std::endl;

		buffer[0] += "┏";
		for(int i = 0; i <  m_winsize.ws_col - 2; i++){
			buffer[0] += "━";
		}
		buffer[0] += "┓";

		std::string ToNextLine = "";
		int Nextline = 0;
		for(int i = 1; i <= m_winsize.ws_row - (2 + m_bottomSpaceLeft); i++){
			if(m_titleTabSize + 1 == i){
				buffer[i] += "┣";
				for (int j = 0; j < m_winsize.ws_col - 2; j++){
					if(m_channelsTabSize == j){
					//	buffer[i] += "╋";
						buffer[i] += "┳";
						continue;
					}
					buffer[i] += "━";
				}
				buffer[i] += "┫";
				continue;
			}

			buffer[i] += "┃";
			if(m_titleTabSize < i && m_channels[i - ( 1 + m_titleTabSize) + m_channelView] != ""){
				buffer[i] += m_channels[i - ( 1 + m_titleTabSize) + m_channelView].substr(0, m_channelsTabSize);
			}
			
			int blancks = m_channelsTabSize + 3 - buffer[i].length();
			for(int j = 0; j <= blancks; j++) buffer[i] += " ";

			if(ToNextLine != ""){
				buffer[i - ( 1 + m_titleTabSize) + m_messageView] += ToNextLine;
				ToNextLine = "";
			} else if(m_titleTabSize < i && m_messages[i - ( 1 + m_titleTabSize) + m_messageView] != ""){
				buffer[i] += m_messages[i - Nextline - ( 1 + m_titleTabSize) + m_messageView].substr(0, m_winsize.ws_col - 4 - m_channelsTabSize);
//				ToNextLine = m_messages[i - ( 1 + m_titleTabSize) + m_messageView].substr(m_winsize.ws_col - 4 - m_channelsTabSize, m_messages[i - (1 + m_titleTabSize) + m_messageView].length() - 1);

				ToNextLine = "A";
				Nextline++;
			}			

			blancks = (m_winsize.ws_col - buffer[i].length());
			for(int j = 0; j <= blancks; ++j) buffer[i] += " ";
			
			if(m_titleTabSize < i){
				buffer[i].replace(m_channelsTabSize + 3, 1, "┃");
			}
			buffer[i] += "┃";
		}

		buffer[m_winsize.ws_row - (1 + m_bottomSpaceLeft)] += "┗";
		for(int i = 0; i < m_winsize.ws_col - 2; i++){
			if(m_channelsTabSize == i){
				buffer[m_winsize.ws_row - (1 + m_bottomSpaceLeft)] += "┻";
				continue;
			}
			buffer[m_winsize.ws_row - (1 + m_bottomSpaceLeft)] += "━";
		}
		buffer[m_winsize.ws_row - (1 + m_bottomSpaceLeft)] += "┛";

		for(int i = 0; i < m_bottomSpaceLeft; i++){
			if(m_bottomLine != "" && i == 0){
				buffer[m_winsize.ws_row - (m_bottomSpaceLeft - i)] += m_bottomLinePrompt;
				buffer[m_winsize.ws_row - (m_bottomSpaceLeft - i)] += m_bottomLine;
				for(int j = 0; j < m_winsize.ws_col + m_unicodeCharacterInPrompt * 2 - (m_bottomLine.length() + m_bottomLinePrompt.length()); j++){
					buffer[m_winsize.ws_row - (m_bottomSpaceLeft - i)] += " ";
				}
				continue;
			}
			for(int j = 0; j < m_winsize.ws_col; j++){
				buffer[m_winsize.ws_row - (m_bottomSpaceLeft - i)] += " ";
			}
		}

		for(int i = 0; i < m_winsize.ws_row; i++){
			std::cout << buffer[i];
		}

		std::cout.flush();
	}

private:
	std::thread *m_windowSizeDemon;
	int m_windowSizeUpdateInterval = 50;

	void updateWindowSize(){
		while(1){
			ioctl(STDOUT_FILENO, TIOCGWINSZ, &m_winsize);
			std::this_thread::sleep_for(std::chrono::milliseconds(m_windowSizeUpdateInterval));
		}
	}
};

int getch(){
	struct termios oldt, newt;

	int ch;

	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	ch = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

	return ch;
}

void kbInput(int mode, std::string *kbBuffer){
	char keyVal;
	while(keyVal = getch()){
		switch(keyVal){
			case 27:
				*kbBuffer = "";
				break;
			case 10:
				*kbBuffer += ":!$e";
				break;
			default:
				if(keyVal == mode){
					*kbBuffer += ":!$";
					break;
				}
				*kbBuffer += keyVal;
				break;
		}
	}
}

int main(){
	std::string A = "";
	std::thread KB(kbInput, 0x003A, &A);


	screenCompositor a;
	for(int i = 0; i < 99; i++){
		a.addChannel("channel :" + std::to_string(i));
		a.addMessage("asdfasdpofiuqwerl,xn cvoiuaspoirqwem n:" + std::to_string(i));
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(100));


	while(1){
		//a.setBottomText(A);
		if(A.length() > 4 && A.substr(A.length() - 4, A.length()) == ":!$e"){
			if(A.length() > 4 && A.substr(0, 3) == ":!$"){
				std::cout << "Control- " + A.substr(3, A.length() - 7) << std::endl;
			} else{
				std::cout << A.substr(0, 4) << std::endl;
				a.m_channelView += a.m_winsize.ws_row - (a.m_titleTabSize + 3);
			}
			A = "";
		}
		a.render();
		if(a.m_channelView > a.m_channelTop)a.m_channelView = 0;
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	

	return 0;
}
