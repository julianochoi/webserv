#include <Server.hpp>


Server::Server(void) : _autoindex(false) {}

Server::Server(Server const &server) {
	_server_names = server.server_names();
	_host = server.host();
	_port = server.port();
	_erros_pages = server.erros_pages();
	_http_methods = server.http_methods();
	_http_redirect = server.http_redirect();
	_root = server.root();
	_body_size_limit = server.body_size_limit();
	_autoindex = server.autoindex();
	_index = server.index();
	_locations = server.locations();
	_cgi_extension = server.cgi_extension();
}

Server &Server::operator=(Server const &server) {
	_server_names = server.server_names();
	_host = server.host();
	_port = server.port();
	_erros_pages = server.erros_pages();
	_http_methods = server.http_methods();
	_http_redirect = server.http_redirect();
	_root = server.root();
	_body_size_limit = server.body_size_limit();
	_autoindex = server.autoindex();
	_index = server.index();
	_locations = server.locations();
	_cgi_extension = server.cgi_extension();
	return *this;
}

Server::~Server(void) {}

void	Server::parse_server_attributes(std::ifstream &fs, std::string line) {
	std::vector<std::string> line_tokens;

	while (!fs.eof())
	{
		std::getline(fs, line);
		line_tokens = Utils::string_split(line, ' ');
		if (line_tokens.empty())
			continue;
		if (line_tokens.size() >= 3 && line_tokens[0] == "location" && line_tokens[2] == "{")
			this->_parse_location_attributes(fs, line, line_tokens[1]);
		else if (line_tokens.size() >= 1 && line_tokens[0] == "};")
			return ;
		else
			this->_set_server_attributes(line_tokens);
	}
}

std::vector<std::string> Server::server_names(void) const { return _server_names; }
std::string Server::host(void) const { return _host; }
int Server::port(void) const { return _port; }
std::map<int, std::string> Server::erros_pages(void) const { return _erros_pages; }
std::vector<std::string> Server::http_methods(void) const { return _http_methods; }
std::pair<int, std::string> Server::http_redirect(void) const { return _http_redirect; }
std::string Server::root(void) const { return _root; }
int Server::body_size_limit(void) const { return _body_size_limit; }
bool Server::autoindex(void) const { return _autoindex; }
std::vector<std::string> Server::index(void) const { return _index; }
std::map<std::string, ServerLocation> Server::locations(void) const { return _locations; }
ServerLocation Server::location(std::string path) const { return _locations.at(path); }
std::string Server::cgi_extension(void) const { return _cgi_extension; }

void	Server::_parse_location_attributes(std::ifstream &fs, std::string line, std::string path) {
	ServerLocation location(*this);
	location.parse_location_attributes(fs, line);
	_locations[path] = location;
}

void	Server::_set_server_attributes(std::vector<std::string> line_tokens) {
	if (line_tokens[0] == "listen")
		_set_listen_attribute(line_tokens);
	else if (line_tokens[0] == "server_name")
		_set_server_name_attribute(line_tokens);
	else if (line_tokens[0] == "root")
		_set_root_attribute(line_tokens);
	else if (line_tokens[0] == "error_page")
		_set_error_page_attribute(line_tokens);
	else if (line_tokens[0] == "client_body_size")
		_set_client_body_size_attribute(line_tokens);
	else if (line_tokens[0] == "cgi")
		_set_cgi_attribute(line_tokens);
	else if (line_tokens[0] == "allowed_methods")
		_set_http_methods_attribute(line_tokens);
	else if (line_tokens[0] == "autoindex")
		_set_autoindex_attribute(line_tokens);
	else if (line_tokens[0] == "return")
		_set_http_redirect_attribute(line_tokens);
	else if (line_tokens[0] == "root")
		_set_root_attribute(line_tokens);
	else if (line_tokens[0] == "index")
		_set_index_attribute(line_tokens);
}

void	Server::_set_listen_attribute(std::vector<std::string> line_tokens) {
	std::vector<std::string> listen_atributes = Utils::string_split(line_tokens[1], ':');

	this->_host = listen_atributes[0];
	this->_port = std::atoi(listen_atributes[1].c_str());
}

void	Server::_set_server_name_attribute(std::vector<std::string> line_tokens) {
	for(long unsigned int j = 1; j < line_tokens.size(); j++)
		this->_server_names.push_back(line_tokens[j]);
}

void	Server::_set_error_page_attribute(std::vector<std::string> line_tokens) {
	for(long unsigned int j = 1; j < line_tokens.size() - 1; j++)
		this->_erros_pages[std::atoi(line_tokens[j].c_str())] = line_tokens[line_tokens.size() - 1];

}

void	Server::_set_client_body_size_attribute(std::vector<std::string> line_tokens) {
	this->_body_size_limit = std::atoi(line_tokens[1].c_str());
}

void	Server::_set_cgi_attribute(std::vector<std::string> line_tokens) {
	this->_cgi_extension = line_tokens[1];
}

void	Server::_set_http_methods_attribute(std::vector<std::string> line_tokens) {
	for(long unsigned int j = 1; j < line_tokens.size(); j++)
		this->_http_methods.push_back(line_tokens[j]);
}

void	Server::_set_http_redirect_attribute(std::vector<std::string> line_tokens) {
	this->_http_redirect = std::make_pair(std::atoi(line_tokens[1].c_str()), line_tokens[2]);
}

void	Server::_set_root_attribute(std::vector<std::string> line_tokens) {
	this->_root = line_tokens[1];
}

void	Server::_set_autoindex_attribute(std::vector<std::string> line_tokens) {
	this->_autoindex = line_tokens[1] == "on";
}

void	Server::_set_index_attribute(std::vector<std::string> line_tokens) {
	for(long unsigned int j = 1; j < line_tokens.size(); j++)
		this->_index.push_back(line_tokens[j]);
}

std::ostream &operator<<(std::ostream &out, const Server &server)
{
	out << "Server Names: " << std::endl;
	std::vector<std::string> server_names = server.server_names();
	for(std::vector<std::string>::const_iterator  it = server_names.begin(); it != server_names.end(); it++)
		out << " " << *it << std::endl;

	out << "Host: " << server.host() << std::endl;
	out << "Port: " << server.port() << std::endl;


	out << "Client max body size: " << server.body_size_limit() << std::endl;

	out << "Cgi extension: " << server.cgi_extension() << std::endl;

	out << "Root: " << server.root() << std::endl;

	out << "Autoindex: " << server.autoindex() << std::endl;

	out << "Indexes: " << std::endl;
	std::vector<std::string> index = server.index();
	for(std::vector<std::string>::const_iterator  it = index.begin(); it != index.end(); it++)
		out << " " << *it << std::endl;

	out << "Http allowed methods: " << std::endl;
	std::vector<std::string> http_methods = server.http_methods();
	for(std::vector<std::string>::const_iterator  it = http_methods.begin(); it != http_methods.end(); it++)
		out << " " << *it << std::endl;

	out << "Errors: " << std::endl;
	std::map<int, std::string> erros_pages = server.erros_pages();
	for (std::map<int, std::string>::const_iterator  it = erros_pages.begin(); it != erros_pages.end(); it++)
			out << " " << "status-" << it->first << " file-" << it->second << std::endl;

	out << "Http redirect: status-" << server.http_redirect().first << " destination-" << server.http_redirect().second << std::endl;

	out << "Locations: " << std::endl;
	std::map<std::string, ServerLocation> locations = server.locations();
	for (std::map<std::string, ServerLocation>::const_iterator  it = locations.begin(); it != locations.end(); it++)
		out << " " << it->first << ":" << std::endl << it->second << std::endl;

	return (out);
}
