#include <Http.hpp>
#include <Utils.hpp>

#include <iostream>
#include <fstream>
#include <string>

#include <sys/stat.h>
#include <string>
using namespace std;


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

	_client_fd = accept(_pollfd.fd, NULL, NULL);

	if (_client_fd == -1)
		throw ClientConnectionError();

	_request = Request(_pollfd, _client_fd);
	_response = Response(_pollfd, _client_fd);

	try {
		_request.handle();
	} catch (Request::BadRequestError& e) {
		addLog(logFile, "BadRequestError Catched");
		_response.handle("400", "");
		return ;
	} catch (Request::URITooLongError& e) {
		addLog(logFile, "URITooLongError Catched");
		_response.handle("414", "");
		return ;
	} catch (Request::InternalServerError& e) {
		addLog(logFile, "InternalServerError Catched");
		_response.handle("500", "");
		return ;
	}

	addLog(logFile,"HTTP handle> Client FD: " + std::string(temp));
	sprintf(temp, "%d", _client_fd);
	std::cout << _client_fd << std::endl;
	std::cout << _request << std::endl;
	_set_http_server();
	std::cout << _http_server << std::endl;
	_set_location();
	if (_has_location)
		std::cout << _http_location << std::endl;
	if (_validate_request())
		return;
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

void Http::_response_handler() {
	std::string response_file_path;
	//struct stat s;

	if (_index().length() && (!_remaining_path.length() || !_remaining_path.compare("/")))
		response_file_path.append(_root()).append("/").append(_index());
	else
		response_file_path.append(_root()).append(_remaining_path);
	addLog(logFile, "Response File Path: " + response_file_path);


	addLog(logFile,"CHECK: ");

	if (isFile(response_file_path)) {
        addLog(logFile,"FILE: " + response_file_path);
		/*eh preciso ver os outros metodos*/
		if (!_request.method().compare("GET"))
			_get_handler(response_file_path);
		else
			_response.handle("500", "");
    } else if (isDirectory(response_file_path)) {
        addLog(logFile,"DIR: " + response_file_path);
		/*inserir o if para o autoindex on*/
		_response.handle("0", response_file_path);
    } else {
        addLog(logFile,"DOES NOT EXIST: " + response_file_path);
		_response.handle("500", "");
    }


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



bool Http::_validate_request() {
	std::string prev_status_code = "400";
	bool has_error = false;
	std::vector<std::string> allowed_methods = _http_methods();

	if (_request.headers()["Content-Length"].length() && std::atoi(_request.headers()["Content-Length"].c_str()) > _body_size_limit())
		has_error = true;
	else if (allowed_methods.size() && !std::count(allowed_methods.begin(), allowed_methods.end(), _request.method()))
		has_error = true;

	if(has_error)
		_response.handle(prev_status_code, _get_file_error(prev_status_code));

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


std::ostream &operator<<(std::ostream &out, const Http &http) {
	(void)http;
	return out;
}
