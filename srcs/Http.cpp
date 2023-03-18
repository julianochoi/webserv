#include <Http.hpp>
#include <Utils.hpp>

#include <iostream>
#include <fstream>
#include <string>

#include <sys/stat.h>
#include <string>
using namespace std;


Http::Http(void) {}

Http::Http(pollfd const &pollfd, std::vector<Server> servers, int client_fd, std::time_t start_time): _pollfd(pollfd), _servers(servers), _has_location(false), _client_fd(client_fd), _is_complete(0), _start_time(start_time) {
	_request = Request(_pollfd, _client_fd);
	_response = Response(_pollfd, _client_fd);
}

Http::Http(Http const &http) {
	_pollfd = http._pollfd;
  _servers = http._servers;
  _http_server = http._http_server;
  _http_location = http._http_location;
  _has_location = http._has_location;
  _remaining_path = http._remaining_path;
  _request = http._request;
  _response = http._response;
  _client_fd = http._client_fd;
  _is_complete = http._is_complete;
  _start_time = http._start_time;
}

Http &Http::operator=(Http const &http) {
	if (this != &http) {
	_pollfd = http._pollfd;
  _servers = http._servers;
  _http_server = http._http_server;
  _http_location = http._http_location;
  _has_location = http._has_location;
  _remaining_path = http._remaining_path;
  _request = http._request;
  _response = http._response;
  _client_fd = http._client_fd;
  _is_complete = http._is_complete;
  _start_time = http._start_time;
	}
	return *this;
}

Http::~Http(void) {}

void Http::handle() {
	char temp[100];

	// std::cout << "FD " << _pollfd.fd << std::endl;
	// std::cout << _pollfd.revents << std::endl;

	sprintf(temp, "%d", _pollfd.fd);
	// addLog(logFile,"HTTP handle> FD: " + std::string(temp));

	sprintf(temp, "%d", _pollfd.revents);
	// addLog(logFile,"HTTP handle> REvents: " + std::string(temp));

	if (_client_fd == -1)
		throw ClientConnectionError();

	// std::cout << _request << std::endl;

	try {
		if (!_request.is_complete())
			_request.handle();
		if (!_request.is_complete())
			return;
	} catch (Request::BadRequestError& e) {
		addLog(logFile, "BadRequestError Catched");
		_response_handle_safe("400", "", false, "");
		return ;
	} catch (Request::URITooLongError& e) {
		addLog(logFile, "URITooLongError Catched");
		_response_handle_safe("414", "", false, "");
		return ;
	} catch (Request::InternalServerError& e) {
		addLog(logFile, "InternalServerError Catched");
		_response_handle_safe("500", "", false, "");
		return ;
	} catch (Request::RecvError& e) {
		addLog(logFile, "RecvError Catched");
		_response_handle_safe("500", "", false, "");
		return ;
	}

	// std::cout << _request << std::endl;


	// addLog(logFile,"HTTP handle> Client FD: " + std::string(temp));
	sprintf(temp, "%d", _client_fd);
	_set_http_server();
	// std::cout << _http_server << std::endl;
	_set_location();
	if (_validate_request())
		return;
	CgiHandler cgi_handler(this->_cgi_timeout());
	try {
		if (_check_cgi(cgi_handler)) {
			cgi_handler.handle(_client_fd, _request);
			_is_complete = 2;
			return;
		}
	}
	catch(const CgiHandler::UnsupportedCGI& e) {
		_response_handle_safe("500", "", false, "");
		return;
	}
	catch(const CgiHandler::CGIError& e) {
		_response_handle_safe("500", "", false, "");
		return;
	}
	catch(const CgiHandler::ChdirError& e) {
		_response_handle_safe("500", "", false, "");
		return;
	}
	_response_handler();
}

void Http::_set_http_server() {
	std::vector<std::string> server_names;
	std::string host = _request.headers()["Host"];

	_http_server = _servers[0];
	for (std::vector<Server>::const_iterator  sev = _servers.begin(); sev != _servers.end(); sev++) {
		server_names = sev->server_names();
		for (std::vector<std::string>::const_iterator  name = server_names.begin(); name != server_names.end(); name++) {
			if (!name->compare(host))
				_http_server = *sev;
		}
	}
}

void Http::_set_location() {
	std::vector<std::string> path_tokens = Utils::string_split(_request.path(), "/");
	std::string path_test = "";

	for(std::vector<std::string>::const_iterator  it = path_tokens.begin(); it != path_tokens.end(); it++) {
		path_test.append("/").append(*it);
		_remaining_path.append("/").append(*it);
		if (_http_server.locations().count(path_test)) {
			_http_location = _http_server.locations()[path_test];
			_has_location = true;
			_remaining_path = "";
		}
	}
}

bool Http::_has_cgi_extension(std::string filename, std::string cgi_extension)
{
	size_t ext_len = cgi_extension.length();
	size_t file_len = filename.length();
	std::string file_ext;

	try
	{
		file_ext = filename.substr(file_len - ext_len);
	}
	catch(const std::out_of_range& e)
	{
		return false;
	}
	return file_ext == cgi_extension;
}

bool Http::_check_cgi(CgiHandler &cgi_handler)
{
	std::vector<std::string> tokens;
	std::string cgi_extension, filename, filepath, full_path;

	cgi_extension = _cgi_extension();
	if (!cgi_extension.length() || !_remaining_path.length())
		return false;
	if (this->_request.method() != "GET" && this->_request.method() != "POST")
		return false;
	tokens = Utils::string_split(_remaining_path, "/");
	if (tokens.size() > 1 && tokens[0] == "cgi-bin")
		filename = tokens[1];
	else
		return false;
	if (_has_cgi_extension(filename, cgi_extension))
	{
		filepath = _root() + "/cgi-bin/" + filename;
		full_path = _root() + _remaining_path;
		cgi_handler.build(_http_server, _http_location, _request, filepath, full_path);
		return true;
	}
	return false;
}

void Http::_response_handler() {
	std::string response_file_path;

	if (_index().length() && (!_remaining_path.length() || !_remaining_path.compare("/")))
		response_file_path.append(_root()).append("/").append(_index());
	else
		response_file_path.append(_root()).append(_remaining_path);
	// addLog(logFile, "Response File Path: " + response_file_path);

	if (_http_redirect().first.length()) {
		_response_handle_safe(_http_redirect().first, response_file_path, false, _http_redirect().second);
	} else if (!_request.method().compare("GET"))
		_get_handler(response_file_path);
	else if (!_request.method().compare("POST"))
		_response_handle_safe("0", response_file_path, false, _request.body());
	else if (!_request.method().compare("DELETE"))
		_response_handle_safe("-1", response_file_path, false, "");
	else
		_response_handle_safe("500", "", false, "");
}

void Http::_get_handler(std::string response_file_path) {
	std::string prevStatusCode = "500";
	std::string prevPath = response_file_path;

	if (isDirectory(response_file_path) && !_autoindex()) {
		prevStatusCode = "200";
		prevPath = prevPath + "/" + _index();
	}

	if (Utils::file_exists(prevPath)) {
		prevStatusCode = "200";
	}
	else {
		prevStatusCode = "404";
		prevPath = _get_file_error(prevStatusCode);
	}

	// addLog(logFile,"Status Code: " + prevStatusCode);
	// addLog(logFile,"Path: " + prevPath);
	_response_handle_safe(prevStatusCode, prevPath, _autoindex(), "");
}



bool Http::_validate_request() {
	std::string prev_status_code = "400";
	bool has_error = false;
	std::vector<std::string> allowed_methods = _http_methods();

	if (_request.headers()["Content-Length"].length() && std::atoi(_request.headers()["Content-Length"].c_str()) > _body_size_limit())
		has_error = true;
	else if (allowed_methods.size() && !std::count(allowed_methods.begin(), allowed_methods.end(), _request.method()))
	{
		has_error = true;
		prev_status_code = "405";
	}

	if(has_error)
		_response_handle_safe(prev_status_code, _get_file_error(prev_status_code), false, "");

	return has_error;
}

std::string Http::_get_file_error(std::string status_code) {
		std::string file_error = "";
		std::string temp_file_path;

		if (_erros_pages().count(std::atoi(status_code.c_str()))) {
			temp_file_path = _root().append(_http_server.erros_pages()[std::atoi(status_code.c_str())]);
			if (Utils::file_exists(temp_file_path))
				file_error = temp_file_path;
		}

		return file_error;
}


std::string Http::_root(void) const {
	if (_has_location)
		return _http_location.root();
	else
		return _http_server.root();
}


std::string Http::_cgi_extension(void) const {
	if (_has_location)
		return _http_location.cgi_extension();
	else
		return _http_server.cgi_extension();
}


std::string Http::_cgi_path(void) const {
	if (_has_location)
		return _http_location.cgi_path();
	else
		return _http_server.cgi_path();
}


size_t Http::_cgi_timeout(void) const {
	if (_has_location)
		return _http_location.cgi_timeout();
	else
		return _http_server.cgi_timeout();
}

std::string Http::_index(void) const {
	if (_has_location)
		return _http_location.index()[0];
	else
		return _http_server.index()[0];
}

std::map<int, std::string> Http::_erros_pages(void) const {
	if (_has_location)
		return _http_location.erros_pages();
	else
		return _http_server.erros_pages();
}

int	Http::_body_size_limit(void) const {
	if (_has_location)
		return _http_location.body_size_limit();
	else
		return _http_server.body_size_limit();
}

bool Http::_autoindex(void) const {
	if (_has_location)
		return _http_location.autoindex();
	else
		return _http_server.autoindex();
}

std::vector<std::string> Http::_http_methods(void) const {
	if (_has_location)
		return _http_location.http_methods();
	else
		return _http_server.http_methods();
}

std::pair<std::string, std::string> Http::_http_redirect(void) const {
	if (_has_location)
		return _http_location.http_redirect();
	else
		return _http_server.http_redirect();
}

void Http::_response_handle_safe(std::string statuscode, std::string pathHTML, bool autoindex, std::string data) {
	try {
		_response.handle(statuscode, pathHTML, autoindex, data);
		_is_complete = 1;
	} catch (...) {
		addLog(logFile, "Response error Catched");
		close(_client_fd);
	}
}

int Http::is_complete(void) {
	return _is_complete;
}

void Http::send_safe() {
	_response.send_safe();
	close(_client_fd);
	_is_complete = 2;
}

bool Http::timeout(std::time_t current) {
	return std::difftime(current, _start_time) > 7;
}



std::ostream &operator<<(std::ostream &out, const Http &http) {
	(void)http;
	return out;
}
