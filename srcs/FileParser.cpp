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
		std::vector<std::string> line_tokens;

		line_tokens = Utils::string_split(line, "\t ");
		if (line_tokens.size() == 2 && line_tokens[0] == "server" && line_tokens[1] == "{") {
			server.parse_server_attributes(fs, line);
			_servers.push_back(server);
		} else if (line_tokens.size() > 0 && line_tokens[0][0] != '#') {
			std::cerr << "Line - " << line << std::endl;
			throw InvalidConfigParam();
		}
}
