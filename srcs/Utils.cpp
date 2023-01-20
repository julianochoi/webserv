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

	void handle_signal(int signal) {
		exit(signal);
	}

	void listen_signals(void) {
		signal(SIGINT, handle_signal);
		signal(SIGQUIT, handle_signal);
	}

}