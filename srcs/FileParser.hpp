#ifndef FILEPARSER_HPP
#define FILEPARSER_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <Server.hpp>

class FileParser {
	private:
		std::vector<Server>	_servers;
		void	_parse_server(std::ifstream &fs, std::string line);

	public:
		FileParser(void);
		FileParser(FileParser const &file_parser);
		FileParser& operator=(FileParser const &file_parser);
		~FileParser(void);

		std::vector<Server> parse(int argc, char **argv);

		class InvalidNumberOfArgs : public std::exception
		{
			public:
				const char* what() const throw(){ return "Invalid Number of Arguments - should be: ./webserv [config file]"; };
		};

		class InvalidConfigFile : public std::exception
		{
			public:
				const char* what() const throw(){ return "Invalid Config File"; };
		};
};

#endif