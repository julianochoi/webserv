#include <Http.hpp>

#include <iostream>
#include <fstream>
#include <string>


Http::Http(void) {}

Http::Http(pollfd const &pollfd, std::vector<Server> servers): _pollfd(pollfd), _servers(servers), _has_location(false) {}

Http::Http(Http const &http) {
	(void)http;
}

Http &Http::operator=(Http const &http) {
	(void)http;
	return *this;
}

Http::~Http(void) {}

void Http::handle() {
	char temp[100];

	std::cout << "FD " << _pollfd.fd << std::endl;
	std::cout << _pollfd.revents << std::endl;

	sprintf(temp, "%d", _pollfd.fd);
	addLog(logFile,"HTTP handle> FD: " + std::string(temp));

	sprintf(temp, "%d", _pollfd.revents);
	addLog(logFile,"HTTP handle> REvents: " + std::string(temp));

	Request request = Request(_pollfd);
	int client_fd = request.handle();
	addLog(logFile,"HTTP handle> Client FD: " + std::string(temp));
	sprintf(temp, "%d", client_fd);
	std::cout << client_fd << std::endl;
	std::cout << request << std::endl;
	_set_http_server(request);
	std::cout << _http_server << std::endl;
	_set_location(request);
	if (_has_location)
		std::cout << _http_location << std::endl;
	_response = Response(_pollfd, client_fd);
	_response_handler(request);
}

void Http::_set_http_server(Request request) {
	std::vector<std::string> server_names;
	std::string host = request.headers()["Host"];

	_http_server = _servers[0];
	for (std::vector<Server>::const_iterator  sev = _servers.begin(); sev != _servers.end(); sev++) {
		server_names = sev->server_names();
		for (std::vector<std::string>::const_iterator  name = server_names.begin(); name != server_names.end(); name++) {
			if (!name->compare(host))
				_http_server = *sev;
		}
	}
}

void Http::_set_location(Request request) {
	std::vector<std::string> path_tokens = Utils::string_split(request.path(), "/");
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

void Http::_response_handler(Request request) {
	std::string response_file_path;

	if (_index().length() && (!_remaining_path.length() || !_remaining_path.compare("/")))
		response_file_path.append(_root()).append("/").append(_index());
	else
		response_file_path.append(_root()).append(_remaining_path);
	addLog(logFile, "Response File Path: " + response_file_path);

	if (!request.method().compare("GET"))
		_get_handler(response_file_path);
}

void Http::_get_handler(std::string response_file_path) {
	std::string prevStatusCode = "500";
	std::string prevPath = "";


	if (Utils::file_exists(response_file_path)) {
		prevStatusCode = "200";
		prevPath = response_file_path;
	}
	else {
		prevStatusCode = "404";
		prevPath = _get_file_error(prevStatusCode);
	}

	addLog(logFile,"Status Code: " + prevStatusCode);
	addLog(logFile,"Path: " + prevPath);
	_response.handle(prevStatusCode, prevPath);
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

std::ostream &operator<<(std::ostream &out, const Http &http) {
	(void)http;
	return out;
}
