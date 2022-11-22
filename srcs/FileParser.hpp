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

		std::vector<Server> parse(void);
};

#endif