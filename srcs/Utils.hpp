#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <vector>
#include <csignal>
#include <stdlib.h>


namespace Utils {
	std::vector<std::string> string_split(const std::string str, char seperator);
	void handle_signal(int signal);
	void listen_signals(void);
}

#endif