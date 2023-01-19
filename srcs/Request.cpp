#include <Request.hpp>

Request::Request(void) {}

Request::Request(pollfd const &pollfd): _pollfd(pollfd) {}

Request::Request(Request const &request) {
	(void)request;
}

Request &Request::operator=(Request const &request) {
	(void)request;
	return *this;
}

Request::~Request(void) {}

int	Request::handle(void) {
	int client_fd = accept(_pollfd.fd, NULL, NULL);
	if (client_fd == -1)
		throw ClientConnectionError();
	return client_fd;
}

std::ostream &operator<<(std::ostream &out, const Request &request) {
	(void)request;
	return out;
}
