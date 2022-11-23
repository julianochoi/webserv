#include <Response.hpp>

Response::Response(void) {}

Response::Response(pollfd const &pollfd, int client_fd): _pollfd(pollfd), _client_fd(client_fd) {}


Response::Response(Response const &response) {
	(void)response;
}

Response &Response::operator=(Response const &response) {
	(void)response;
	return *this;
}

Response::~Response(void) {}

void Response::handle(void) {
	send(_client_fd, "HTTP/1.1 200 OK\n", 16, 0);
	send(_client_fd, "Content-Type: text/html\n", 24, 0);
	send(_client_fd, "Content-Length: 48\n\n", 20, 0);
	send(_client_fd, "<html><body><h1>Hello, World!</h1></body></html>", 48, 0);
	close(_client_fd);
}

std::ostream &operator<<(std::ostream &out, const Response &response) {
	(void)response;
	return out;
}