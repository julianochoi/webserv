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
	_response_handler(client_fd, request);
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

void Http::_response_handler(int client_fd, Request request) {
	Response response = Response(_pollfd, client_fd);

	//! comenta isso
	std::ifstream file("statuscode.txt");
	std::string statuscode;
	if (file.is_open()) {
		std::getline(file, statuscode);
		file.close();
	}

	response.handle(statuscode);

	//! descomenta isso
	// if (!request.method().compare("GET")) {
	// 	std::string response_file_path;
	// 	std::string root;
	// 	std::string index;
	// 	if (_has_location) {
	// 		root = _http_location.root();
	// 		index = _http_location.index()[0];
	// 	} else {
	// 		root = _http_server.root();
	// 		index = _http_server.index()[0];
	// 	}
	// 	if (index.length())
	// 		response_file_path.append(root).append("/").append(index);
	// 	else
	// 		response_file_path.append(root).append(_remaining_path);

	// 	std::cout << response_file_path << std::endl;
	// 	std::ifstream file(response_file_path.c_str());
	// 	if (file.is_open()) {
	// 		file.close();
	// 		response.handle("200", response_file_path);
	// 	}
	// 	else {
	// 		std::string file_error = "";
	// 		if (_has_location) {
	// 			if (_http_location.erros_pages().count(404))
	// 				file_error = root.append(_http_location.erros_pages()[404]);
	// 		} else {
	// 			if (_http_server.erros_pages().count(404))
	// 				file_error = root.append(_http_server.erros_pages()[404]);
	// 		}
	// 		response.handle("404", file_error);
	// 	}
	// }
}

std::ostream &operator<<(std::ostream &out, const Http &http) {
	(void)http;
	return out;
}
