#include <Request.hpp>

Request::Request(void) {}

Request::Request(pollfd const &pollfd, int client_fd): _pollfd(pollfd), _client_fd(client_fd), _headers_error(false), _is_complete(0), _parse_steps(0), _chunk_parse(0), _chunk_total_size(0) {
	// _buffer = new char[BUFFER_SIZE];
}

Request::Request(Request const &request) {
	_headers = request.headers();
	_body = request.body();
	_method = request.method();
	_path = request.path();
	_query = request.query();
	_query_map = request.query_map();
	_protocol = request.protocol();
	_protocol_version = request.protocol_version();
	_buffer = request._buffer;
	_pollfd = request._pollfd;
	_client_fd = request._client_fd;
	_total_buffer = request._total_buffer;
	_headers_error = request._headers_error;
	_is_complete = request._is_complete;
	_parse_steps = request._parse_steps;
	_chunk_parse = request._chunk_parse;
	_chunk_total_size = request._chunk_total_size;
	_chunk_size = request._chunk_size;
}

Request &Request::operator=(Request const &request) {
	_headers = request.headers();
	_body = request.body();
	_method = request.method();
	_path = request.path();
	_query = request.query();
	_query_map = request.query_map();
	_protocol = request.protocol();
	_protocol_version = request.protocol_version();
	_buffer = request._buffer;
	_pollfd = request._pollfd;
	_client_fd = request._client_fd;
	_total_buffer = request._total_buffer;
	_headers_error = request._headers_error;
	_is_complete = request._is_complete;
	_parse_steps = request._parse_steps;
	_chunk_parse = request._chunk_parse;
	_chunk_total_size = request._chunk_total_size;
	_chunk_size = request._chunk_size;
	return *this;
}

Request::~Request(void) {
	// delete[] _buffer;
}

void	Request::handle(void) {
	if (_parse_steps == 0) {
		_parse_first_line();
		return ;
	}

	if (_parse_steps == 1) {
		_parse_headers();
		if (_parse_steps != 1 && _headers["Transfer-Encoding"].compare("chunked") && !_headers["Content-Length"].length()) // This is to check if have body
			_parse_steps = 3;
		else {
			if (_parse_steps != 1 && _headers["Content-Length"].length())
				_validate_content_length();
			return ;
		}
	}

	if (_parse_steps == 2) {
		if (!_headers["Transfer-Encoding"].compare("chunked"))
			_parse_chunked_body();
		else if (_headers["Content-Length"].c_str())
			_parse_full_body();
		else
			_parse_steps = 3;
		if (_parse_steps == 2)
			return ;
	}

	_is_complete = 1;
}

std::string	Request::_get_line() {
	std::size_t finder;
	std::string line;

	_line_found = 0;
	_recv_safe(_client_fd, &_buffer, BUFFER_SIZE, 0);
	_total_buffer += _buffer;

	finder = _total_buffer.find("\n");
	if (finder != std::string::npos)
		_line_found = 1;
	else
		return "";

	line = _total_buffer;
	line.erase(finder);
	_total_buffer.erase(0, finder);
	return line;
}


std::string	Request::_get_chunked_size_line() {
	std::size_t finder;

	_line_found = 0;

	_recv_safe(_client_fd, &_buffer, BUFFER_SIZE, 0);
	_total_buffer += _buffer;

	finder = _total_buffer.find("\n");
	if (finder != std::string::npos)
		_line_found = 1;
	else
		return "";

	return _total_buffer;
}

void	Request::_get_chunked_body_line(std::size_t size) {
	if (size == 0)
		return ;
	char *body_buffer = new char[size + 2]; // 2 for \r\n

	_recv_safe(_client_fd, body_buffer, size + 2, 0);
	body_buffer[size + 1] = '\0'; // to cut \r\n
	_body.append(body_buffer);

	delete[] body_buffer;
}

void	Request::_get_full_body(std::size_t size) {
	if (size == 0) {
		_parse_steps = 3;
		return ;
	}

	char *body_buffer = new char[size + 1];

	_recv_safe(_client_fd, body_buffer, size, 0);
	body_buffer[size] = '\0';
	_body.append(body_buffer);
	delete[] body_buffer;
	_parse_steps = 3;
}

void	Request::_parse_first_line() {
	std::vector<std::string> tokens, path_tokens;
	std::string line;
	size_t question_pos;

	line = _get_line();
	if (!_line_found)
		return;

	if (line.length() > MAX_URI_LENGTH)
		throw URITooLongError();
	tokens = Utils::string_split(line, "\t ");
	_set_method(tokens[0]);
	question_pos = tokens[1].find("?");
	if (question_pos != std::string::npos)
		_set_query(tokens[1].substr(question_pos + 1));
	_set_path(tokens[1].substr(0, question_pos));
	_set_protocol_info(tokens[2]);
	// addLog(logFile,"Request first line> Protocol:" + tokens[2]);
	_total_buffer.erase(0, 1);
	_parse_steps = 1;
}

void	Request::_parse_headers() {
	std::string header_line;

	header_line = _get_line();

	if (!_line_found)
		return;

	if (!header_line.compare("\r")) { //when reaches end of header parse
		_total_buffer.erase(0, 1);
		_parse_steps = 2;
		return ;
	}

	_set_headers(header_line);
	_total_buffer.erase(0, 1);

	if (_headers_error)
		throw BadRequestError();
}

void	Request::_parse_full_body() {
	int size = _validate_content_length();

	_get_full_body(size);
}

void	Request::_parse_chunked_body() {
	std::string chunk_size_str;

	if (_chunk_parse == 0 || _chunk_parse == 1) {
		chunk_size_str = _get_chunked_size_line();
		if (!_line_found)
			return;
		else
			_total_buffer.clear();

		std::stringstream	s_stream(chunk_size_str);
		s_stream >> std::hex >> _chunk_size;
		_chunk_total_size += _chunk_size;
		if (_chunk_size > 0)
			_chunk_parse = 2;
		else
			_chunk_parse = 3;
	}
	else if (_chunk_parse == 2) {
		_get_chunked_body_line(_chunk_size);
		_chunk_parse = 1;
	}

	if (_chunk_parse == 3) {
		// convert _chunk_total_size to string
		std::stringstream string_chunk_size_stream;
		string_chunk_size_stream << _chunk_total_size;
		std::string chunk_total_size_string;
		string_chunk_size_stream >> chunk_total_size_string;

		_headers["Content-Length"] = chunk_total_size_string;
		_parse_steps = 3;
	}
}

int	Request::_validate_content_length() {
	if (!Utils::is_number(_headers["Content-Length"].c_str()))
		throw BadRequestError();

	int size = std::atoi(_headers["Content-Length"].c_str());
	if (size < 0)
		throw BadRequestError();

	return size;
}

void	Request::_set_headers(std::string line) {
	std::size_t finder = line.find(":");
	std::size_t finder_spaces = line.find_first_of(" :");

	if (finder_spaces != finder || finder == 0)
		_headers_error = true;

	std::string key = line.substr(0, finder);
	std::string value = line.substr(finder + 2, line.length() - finder - 3); // + 2 to igone : and space after host key

	if (_headers.count(key))
		_headers_error = true;
	_headers[key] = value;
}
void	Request::_set_body(std::string line) { (void)line; }
void	Request::_set_method(std::string line) { _method = line; }
void	Request::_set_path(std::string line) { _path = line; }
void	Request::_set_query(std::string line) {
		std::vector<std::string> query_params, query_pair;
		std::vector<std::string>::iterator it;

		query_params = Utils::string_split(line, "&");
		for (it = query_params.begin(); it != query_params.end(); it++)
		{
			query_pair = Utils::string_split(*it, "=");
			if (query_pair.size() != 2)
				throw BadRequestError();
			_query_map[query_pair[0]] = query_pair[1];
		}
		_query = line;
	}
void	Request::_set_protocol_info(std::string line) {
	std::vector<std::string> protocol_infos = Utils::string_split(line, "/");

	if (protocol_infos.size() != 2) {
		_headers_error = true;
		return ;
	}
	_protocol = protocol_infos[0];
	_protocol_version = protocol_infos[1];

	size_t return_index = _protocol_version.find("\r");
	if (return_index != std::string::npos)
		_protocol_version.erase(return_index, 1);
	if (_protocol.compare("HTTP") || _protocol_version.compare("1.1"))
		_headers_error = true;
}

std::map<std::string, std::string>	Request::headers(void) const { return _headers; }
std::string							Request::body(void) const { return _body; }
std::string							Request::method(void) const { return _method; }
std::string							Request::path(void) const { return _path; }
std::string							Request::query(void) const { return _query; }
std::map<std::string, std::string>	Request::query_map(void) const { return _query_map; }
std::string							Request::protocol(void) const { return _protocol; }
std::string							Request::protocol_version(void) const { return _protocol_version; }

ssize_t Request::_recv_safe(int __fd, void *__buf, size_t __n, int __flags) {
	ssize_t bytes =	recv(__fd, __buf, __n, __flags);

	if (bytes == -1 || bytes == 0)
		throw RecvError();

	return bytes;
}

int Request::is_complete(void) const {
	return _is_complete;
}


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
	out << "Is completes: " << request.is_complete() << std::endl;
	return out;
}
