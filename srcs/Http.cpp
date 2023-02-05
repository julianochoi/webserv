#include <Http.hpp>

Http::Http(void) {}

Http::Http(pollfd const &pollfd, std::vector<Server> servers): _pollfd(pollfd), _servers(servers) {}

Http::Http(Http const &http) {
	(void)http;
}

Http &Http::operator=(Http const &http) {
	(void)http;
	return *this;
}

Http::~Http(void) {}

void Http::handle(void) {
	std::cout << "FD " << _pollfd.fd << std::endl;
	std::cout << _pollfd.revents << std::endl;
	Request request = Request(_pollfd);
	int client_fd = request.handle();
	std::cout << client_fd << std::endl;
	std::cout << request << std::endl;
	_set_http_server(request);
	std::cout << _http_server << std::endl;
	Response response = Response(_pollfd, client_fd);
	response.handle();
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

std::ostream &operator<<(std::ostream &out, const Http &http) {
	(void)http;
	return out;
}
