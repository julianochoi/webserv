#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <vector>
#include <csignal>
#include <stdlib.h>

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

namespace Utils {
	std::vector<std::string> string_split(const std::string str, char seperator);
	void handle_signal(int signal);
	void listen_signals(void);
}

#endif