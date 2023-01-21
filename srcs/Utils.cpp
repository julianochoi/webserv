#include <Utils.hpp>

namespace Utils {
	std::vector<std::string> string_split(const std::string str, std::string seperators)
	{
			std::vector<std::string> tokens;

			int len = str.length(), i = -1;
			int start_index = 0, size;

			while (++i < len)
			{
				while (seperators.find(str[i]) != std::string::npos)
				{
					i++;
					continue ;
				}

				start_index = i;

				while (i <= len && seperators.find(str[i]) == std::string::npos)
					i++;

				size = i - start_index;

				tokens.push_back(str.substr(start_index, size));
			}

			return tokens;
	}

	bool is_valid_file(const std::string str) {
		struct stat statbuf;
		if (stat(str.c_str(), &statbuf) == -1)
			return false;
		if (S_ISDIR(statbuf.st_mode | S_IRUSR))
			return false;
		return true;
	}

	bool is_valid_dir(const std::string str) {
		struct stat statbuf;
		if (stat(str.c_str(), &statbuf) == -1)
			return false;
		if (!S_ISDIR(statbuf.st_mode | S_IRUSR))
			return false;
		return true;
	}

	bool is_number(const std::string str) {
		if (str.find_first_not_of("0123456789") != std::string::npos)
			return false;
		return true;
	}

	void handle_signal(int signal) {
		exit(signal);
	}

	void listen_signals(void) {
		signal(SIGINT, handle_signal);
		signal(SIGQUIT, handle_signal);
	}

}