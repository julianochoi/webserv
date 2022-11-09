#include <FileParser.hpp>

FileParser::FileParser(void) {}

FileParser::FileParser(FileParser const &file_parser) {
	(void)file_parser;
}

FileParser &FileParser::operator=(FileParser const &file_parser) {
	(void)file_parser;
	return *this;
}

FileParser::~FileParser(void) {}

void FileParser::parse(void) {
	std::ifstream	fs;
	std::string		line;
	std::vector<Server> servers;

	fs.open("./default.conf", std::ifstream::in);

	if(!fs.is_open())
		throw std::exception();

	while (!fs.eof())
	{
		std::getline(fs, line);
		_parse_server(fs, line);
	}
	fs.close();
}

void FileParser::_parse_server(std::ifstream &fs, std::string line) {
		Server server;

		if (line == "server {") {
			server.parse_server_attributes(fs, line);
			std::cout << server << std::endl;
		}

}
