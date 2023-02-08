#include <Request.hpp>

Request::Request(void) {}

Request::Request(pollfd const &pollfd): _pollfd(pollfd) {
	_buffer = new char[BUFFER_SIZE];
}

Request::Request(Request const &request) {
	_headers = request.headers();
	_body = request.body();
	_method = request.method();
	_path = request.path();
	_query = request.query();
	_protocol = request.protocol();
	_protocol_version = request.protocol_version();
	_buffer = new char[BUFFER_SIZE];
}

Request &Request::operator=(Request const &request) {
	_headers = request.headers();
	_body = request.body();
	_method = request.method();
	_path = request.path();
	_query = request.query();
	_protocol = request.protocol();
	_protocol_version = request.protocol_version();
	_buffer = new char[BUFFER_SIZE];
	return *this;
}

Request::~Request(void) {
	delete[] _buffer;
}

int	Request::handle(void) {
	_client_fd = accept(_pollfd.fd, NULL, NULL);

	if (_client_fd == -1)
		throw ClientConnectionError();

	_parse_first_line();
	if (!_total_buffer.compare("\n"))
		_parse_headers();
	if (!_headers["Transfer-Encoding"].compare("chunked"))
		_parse_chunked_body();
	else if (_headers["Content-Length"].c_str())
		_parse_full_body();

	return _client_fd;
}

std::string	Request::_get_line() {
	std::size_t finder;
	int bytes_read;
	std::string line;

	bytes_read = recv(_client_fd, _buffer, BUFFER_SIZE, 0);
	_total_buffer += *_buffer;
	if (!_total_buffer.compare("\r")) { //when reaches end of header parse
		recv(_client_fd, _buffer, BUFFER_SIZE, 0); // gets \r\n string
		_total_buffer += *_buffer;
		_total_buffer.erase(0, 2);
		return "";
	}
	while (bytes_read > 0) {
		bytes_read = recv(_client_fd, _buffer, BUFFER_SIZE, 0);
		_total_buffer += *_buffer;
		finder = _total_buffer.find("\n");
		if (finder != std::string::npos)
			break;
	}

	//addLog(logFile,"Request Total buffer:" + _total_buffer);
	line = _total_buffer;
	line.erase(finder);
	_total_buffer.erase(0, finder);
	return line;
}

std::string	Request::_get_chunked_size_line() {
	std::size_t finder;
	int bytes_read;
	std::string line;

	bytes_read = recv(_client_fd, _buffer, BUFFER_SIZE, 0);
	line += *_buffer;
	while (bytes_read > 0) {
		bytes_read = recv(_client_fd, _buffer, BUFFER_SIZE, 0);
		line += *_buffer;
		finder = line.find("\n");
		if (finder != std::string::npos)
			break;
	}

	return line;
}


void	Request::_get_chunked_body_line(std::size_t size) {
	if (size == 0)
		return ;
	char *body_buffer = new char[size + 1];

	recv(_client_fd, body_buffer, size, 0);
	body_buffer[size] = '\0';
	_body.append(body_buffer);


	recv(_client_fd, body_buffer, 1, 0); //read \r line
	recv(_client_fd, body_buffer, 1, 0); //read \n line

	delete[] body_buffer;
}

void	Request::_get_full_body(std::size_t size) {
	if (size == 0)
		return ;

	char *body_buffer = new char[size + 1];

	recv(_client_fd, body_buffer, size, 0);
	body_buffer[size] = '\0';
	_body.append(body_buffer);
	delete[] body_buffer;
}

void	Request::_parse_first_line() {
	std::vector<std::string> tokens;
	std::string							 line;

	line = _get_line();
	tokens = Utils::string_split(line, "\t ");
	_set_method(tokens[0]);
	addLog(logFile,"Request first line> Method:" + tokens[0]);
	_set_path(tokens[1]);
	addLog(logFile,"Request first line> Path:" + tokens[1]);
	_set_protocol_info(tokens[2]);
	addLog(logFile,"Request first line> Protocol:" + tokens[2]);
}

void	Request::_parse_headers() {
	std::string header_line;

	while (!_total_buffer.compare("\n")) {
		_total_buffer.erase(0, 1);
		header_line = _get_line();
		if (header_line.compare("")) {
			_set_headers(header_line);
			addLog(logFile,"Request parse headers> header_line:" + header_line);
		}
	}
}

void	Request::_parse_full_body() {
	int size = std::atoi(_headers["Content-Length"].c_str());
	_get_full_body(size);
}

void	Request::_parse_chunked_body() {
	std::size_t chunk_size;
	std::size_t chunk_total_size = 0;
	std::string chunk_size_str;

	chunk_size_str = _get_chunked_size_line();
	std::stringstream	s_stream(chunk_size_str);

	s_stream >> std::hex >> chunk_size;
	chunk_total_size += chunk_size;

	while (chunk_size > 0)
	{
		_get_chunked_body_line(chunk_size);
		chunk_size_str = _get_chunked_size_line();
		std::stringstream	s_stream(chunk_size_str);

		s_stream >> std::hex >> chunk_size;
		chunk_total_size += chunk_size;
	}

	_headers["Content-Length"] = chunk_total_size;
}

void	Request::_set_headers(std::string line) {
	std::size_t finder = line.find(":");
	std::string key = line.substr(0, finder);
	std::string value = line.substr(finder + 2, line.length() - finder - 3); // + 2 to igone : and space after host key
	_headers[key] = value;
}
void	Request::_set_body(std::string line) { (void)line; }
void	Request::_set_method(std::string line) { _method = line; }
void	Request::_set_path(std::string line) { _path = line; }
void	Request::_set_query(std::string line) { (void)line; }
void	Request::_set_protocol_info(std::string line) {
	std::vector<std::string> protocol_infos = Utils::string_split(line, "/");

	_protocol = protocol_infos[0];
	_protocol_version = protocol_infos[1];
}

std::map<std::string, std::string>	Request::headers(void) const { return _headers; }
std::string							Request::body(void) const { return _body; }
std::string							Request::method(void) const { return _method; }
std::string							Request::path(void) const { return _path; }
std::string							Request::query(void) const { return _query; }
std::string							Request::protocol(void) const { return _protocol; }
std::string							Request::protocol_version(void) const { return _protocol_version; }

std::ostream &operator<<(std::ostream &out, const Request &request) {
	out << "Request: " << std::endl;

	out << "Method: " << request.method() << std::endl;
	out << "Path: " << request.path() << std::endl;
	out << "Protocol: " << request.protocol() << "/" << request.protocol_version() << std::endl;

	out << "Headers: " << std::endl;
	std::map<std::string, std::string> headers = request.headers();
	for (std::map<std::string, std::string>::const_iterator  it = headers.begin(); it != headers.end(); it++)
			out << " " << it->first << ": " << it->second << std::endl;

	out << "Body: " << request.body() << std::endl;
	return out;
}
