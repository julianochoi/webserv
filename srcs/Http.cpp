#include <Http.hpp>

Http::Http(void) {}

Http::Http(pollfd const &pollfd): _pollfd(pollfd) {}

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
	Response response = Response(_pollfd, client_fd);
	response.handle();
}

std::ostream &operator<<(std::ostream &out, const Http &http) {
	(void)http;
	return out;
}
