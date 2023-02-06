#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <vector>
#include <csignal>
#include <stdlib.h>
#include <sys/stat.h>

#include <fstream>
#include <string>

#include <ctime>


#define COLOR_OFF "\033[0m"
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define YELLOW "\033[0;33m"
#define BLUE "\033[0;34m"
#define PURPLE "\033[0;35m"
#define CYAN "\033[0;36m"
#define BRED "\033[1;31m"
#define BGREEN "\033[1;32m"
#define BYELLOW "\033[1;33m"
#define BBLUE "\033[1;34m"
#define BPURPLE "\033[1;35m"
#define BCYAN "\033[1;36m"
#define COLOR_OFF "\033[0m"

#define logFile	"log.log"


extern time_t TS;


namespace Utils {
	std::vector<std::string> string_split(const std::string str, std::string seperators);
	bool is_valid_file(const std::string str);
	bool is_valid_dir(const std::string str);
	bool is_number(const std::string str);
	void handle_signal(int signal);
	void listen_signals(void);
}


void addLog(const std::string& fileName, const std::string& line);




#endif