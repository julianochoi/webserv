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
	std::string		config_filepath;

	addLog(logFile,"Detecting number of arguments");
	if (argc == 1)
		config_filepath = "./config_files/default.conf";
	else if (argc == 2)
		config_filepath = argv[1];
	else
		throw InvalidNumberOfArgs();

	addLog(logFile,"Opening file:");
	fs.open(config_filepath.c_str(), std::ifstream::in);

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
