#include <Utils.hpp>
#include <fstream>
#include <string>
#include <cstdio>
#include <ctime>


/*In this code, std::ofstream is used to open the file for writing
and the std::ios::out and std::ios::app flags are used to open the
file in append mode. The std::endl is used to add a newline character
 to the end of the line being written.*/
void addLog(const std::string& fileName, const std::string& line) {
	// Get the current time
	time_t t = time(0);
  	struct tm * now = localtime(&t);

	// Create a string with the current time and date
	char buf[100];
	strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", now);
	std::string nowStr = buf;

    std::ofstream outFile;
    outFile.open(fileName.c_str(), std::ios::out | std::ios::app);
    if (outFile.is_open()) {
        outFile << "[" << nowStr << "] " << line << std::endl;
        outFile.close();
    }
}






void createAutoIndex(const std::string& fileName, const std::string& line) {
	std::ofstream outFile;

    outFile.open(fileName.c_str(), std::ios::out | std::ios::app);
    if (outFile.is_open()) {
        outFile << line << std::endl;
        outFile.close();
    }
}



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

	bool file_exists(const std::string filename)
	{
		return access(filename.c_str(), F_OK) == 0;
	}

	void handle_signal(int signal) {
		exit(signal);
	}

	void listen_signals(void) {
		signal(SIGINT, handle_signal);
		signal(SIGQUIT, handle_signal);
	}

}