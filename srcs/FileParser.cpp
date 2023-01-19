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

std::vector<Server> FileParser::parse(int argc, char **argv) {
	std::ifstream	fs;
	std::string		line;

	if (argc != 2)
		throw InvalidNumberOfArgs();

	fs.open(argv[1], std::ifstream::in);

	if(!fs.is_open())
		throw InvalidConfigFile();

	while (!fs.eof())
	{
		std::getline(fs, line);
		_parse_server(fs, line);
	}
	fs.close();

	return _servers;
}

void FileParser::_parse_server(std::ifstream &fs, std::string line) {
		Server server;

		if (line == "server {") {
			server.parse_server_attributes(fs, line);
			_servers.push_back(server);
		}
}
