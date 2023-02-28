#include <ServerLocation.hpp>


ServerLocation::ServerLocation(void) : _autoindex(false) {}

ServerLocation::ServerLocation(Server const &server_location) {
	_http_methods = server_location.http_methods();
	_http_redirect = server_location.http_redirect();
	_root = server_location.root();
	_autoindex = server_location.autoindex();
	_index = server_location.index();
	_cgi_extension = server_location.cgi_extension();
	_erros_pages = server_location.erros_pages();
	_body_size_limit = server_location.body_size_limit();
}

ServerLocation::ServerLocation(ServerLocation const &server_location) {
	_http_methods = server_location.http_methods();
	_http_redirect = server_location.http_redirect();
	_root = server_location.root();
	_autoindex = server_location.autoindex();
	_index = server_location.index();
	_cgi_extension = server_location.cgi_extension();
	_erros_pages = server_location.erros_pages();
	_body_size_limit = server_location.body_size_limit();
}

ServerLocation &ServerLocation::operator=(ServerLocation const &server_location) {
	if (this != &server_location) {
		_http_methods = server_location.http_methods();
		_http_redirect = server_location.http_redirect();
		_root = server_location.root();
		_autoindex = server_location.autoindex();
		_index = server_location.index();
		_cgi_extension = server_location.cgi_extension();
		_erros_pages = server_location.erros_pages();
		_body_size_limit = server_location.body_size_limit();
	}

	return *this;
}

ServerLocation::~ServerLocation(void) {}

void	ServerLocation::parse_location_attributes(std::ifstream &fs, std::string line) {
	std::vector<std::string> line_tokens;

	while (!fs.eof())
	{
		std::getline(fs, line);
		line_tokens = Utils::string_split(line, "\t ");
		if (line_tokens.size() >= 1 && (line_tokens[0] == "};" || line_tokens[0] == "}"))
			return ;
		else
			this->_set_location_attributes(line_tokens);
	}
}

std::map<int, std::string> ServerLocation::erros_pages(void) const { return _erros_pages; }
std::vector<std::string> ServerLocation::http_methods(void) const { return _http_methods; }
std::pair<int, std::string> ServerLocation::http_redirect(void) const { return _http_redirect; }
std::string ServerLocation::root(void) const { return _root; }
int ServerLocation::body_size_limit(void) const { return _body_size_limit; }
bool ServerLocation::autoindex(void) const { return _autoindex; }
std::vector<std::string> ServerLocation::index(void) const { return _index; }
std::string ServerLocation::cgi_extension(void) const { return _cgi_extension; }

void	ServerLocation::_set_location_attributes(std::vector<std::string> line_tokens) {
	if (line_tokens.size() <= 1) {
		std::cerr << "Param - " << line_tokens[0] << std::endl;
		throw MissingLocationArgs();
	}

	if (line_tokens[0] == "root")
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
	else if (line_tokens[0] == "index")
		_set_index_attribute(line_tokens);
	else if (line_tokens[0][0] != '#') {
		std::cerr << "Param - " << line_tokens[0] << std::endl;
		throw InvalidLocationParam();
	}
}

void	ServerLocation::_set_error_page_attribute(std::vector<std::string> line_tokens) {
	if (line_tokens.size() < 3) {
		std::cerr << "Param - " << line_tokens[0] << std::endl;
		throw MissingLocationArgs();
	}

	for(long unsigned int j = 1; j < line_tokens.size() - 1; j++)
		this->_erros_pages[std::atoi(line_tokens[j].c_str())] = line_tokens[line_tokens.size() - 1];
}

void	ServerLocation::_set_client_body_size_attribute(std::vector<std::string> line_tokens) {
	if (!Utils::is_number(line_tokens[1])) {
		std::cerr << "Body Size - " << line_tokens[1] << std::endl;
		throw InvalidBodySize();
	}

	this->_body_size_limit = std::atoi(line_tokens[1].c_str());
}

void	ServerLocation::_set_cgi_attribute(std::vector<std::string> line_tokens) {
	if (line_tokens[1].size() < 2 || line_tokens[1][0] != '.') {
		std::cerr << "CGI - " << line_tokens[1] << std::endl;
		throw InvalidCGIExtension();
	}

	this->_cgi_extension = line_tokens[1];
}

void	ServerLocation::_set_http_methods_attribute(std::vector<std::string> line_tokens) {
	// do not need validation - allow any http method name and then only verify it is included
  this->_http_methods.clear();

	for(long unsigned int j = 1; j < line_tokens.size(); j++)
		this->_http_methods.push_back(line_tokens[j]);
}

void	ServerLocation::_set_http_redirect_attribute(std::vector<std::string> line_tokens) {
	if (line_tokens.size() != 3 || !Utils::is_number(line_tokens[1].c_str())) {
		std::cerr << "Redirection error - " << line_tokens[1] << std::endl;
		throw InvalidRedirect();
	}

	this->_http_redirect = std::make_pair(std::atoi(line_tokens[1].c_str()), line_tokens[2]);
}

void	ServerLocation::_set_root_attribute(std::vector<std::string> line_tokens) {
	if (!Utils::is_valid_dir(line_tokens[1])) {
		std::cerr << "Error at Root - " << line_tokens[1] << std::endl;
		throw InvalidRoot();
	}

	this->_root = line_tokens[1];
}

void	ServerLocation::_set_autoindex_attribute(std::vector<std::string> line_tokens) {
	std::transform(line_tokens[1].begin(), line_tokens[1].end(), line_tokens[1].begin(), ::tolower);

	if (line_tokens[1] != "on" && line_tokens[1] != "off")
		throw InvalidAutoIndexParam();

	this->_autoindex = line_tokens[1] == "on";
}

void	ServerLocation::_set_index_attribute(std::vector<std::string> line_tokens) {
	// do not need validation - only try to find its location at response
	this->_index.clear();

	for(long unsigned int j = 1; j < line_tokens.size(); j++)
		this->_index.push_back(line_tokens[j]);
}

std::ostream &operator<<(std::ostream &out, ServerLocation const &server_location)
{
	out << "  Client max body size: " << server_location.body_size_limit() << std::endl;

	out << "  Cgi extension: " << server_location.cgi_extension() << std::endl;

	out << "  Root: " << server_location.root() << std::endl;

	out << "  Autoindex: " << server_location.autoindex() << std::endl;

	out << "  Indexes: " << std::endl;
	std::vector<std::string> index = server_location.index();
	for(std::vector<std::string>::const_iterator  it = index.begin(); it != index.end(); it++)
		out << "   " << *it << std::endl;

	out << "  Http allowed methods: " << std::endl;
	std::vector<std::string> http_methods = server_location.http_methods();
	for(std::vector<std::string>::const_iterator  it = http_methods.begin(); it != http_methods.end(); it++)
		out << "   " << *it << std::endl;

	out << "  Errors: " << std::endl;
	std::map<int, std::string> erros_pages = server_location.erros_pages();
	for (std::map<int, std::string>::const_iterator  it = erros_pages.begin(); it != erros_pages.end(); it++)
			out << "   " << "status-" << it->first << " file-" << it->second << std::endl;

	out << "  Http redirect: status-" << server_location.http_redirect().first << " destination-" << server_location.http_redirect().second << std::endl;

	return (out);
}