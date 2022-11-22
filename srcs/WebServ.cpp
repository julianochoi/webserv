#include <WebServ.hpp>

WebServ::WebServ(void) {}

WebServ::WebServ(WebServ const &web_serv) {
	(void)web_serv;
}

WebServ &WebServ::operator=(WebServ const &web_serv) {
	(void)web_serv;
	return *this;
}

WebServ::~WebServ(void) {}

std::vector<Server> WebServ::servers(void) const { return _servers; }


void WebServ::init(void) {
	FileParser					fileParser;

	_servers = fileParser.parse();
	_start_listening();
}

void WebServ::event_loop(void) {
	int connections;
	int client_fd;
	int16_t revents;

	while (true) {
		connections = poll((pollfd *)&(*_pollfds.begin()), _pollfds.size(), -1); // TODO - should free this leeks
		std::cout << "conn " << connections << std::endl;
		for (std::vector<pollfd>::const_iterator pollfd = _pollfds.begin(); pollfd != _pollfds.end(); pollfd++)
		{
			revents = pollfd->revents;
			if (revents) {
				std::cout << "FD " << pollfd->fd << std::endl;
				std::cout << revents << std::endl;
				client_fd = accept(pollfd->fd, NULL, NULL);
				std::cout << client_fd << std::endl;
				send(client_fd, "HTTP/1.1 200 OK\n", 16, 0);
				send(client_fd, "Content-Type: text/html\n", 24, 0);
				send(client_fd, "Content-Length: 48\n\n", 20, 0);
				send(client_fd, "<html><body><h1>Hello, World!</h1></body></html>", 48, 0);
				close(client_fd);
			}
		}
	}
}

void WebServ::_start_listening(void) {
	int									socket_fd;
	struct sockaddr_in	sockaddr_in;

	for (std::vector<Server>::const_iterator server = _servers.begin(); server != _servers.end(); server++) {
		socket_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (socket_fd == -1)
			throw std::exception();

		sockaddr_in.sin_family = AF_INET;
		sockaddr_in.sin_port = htons(server->port());
		sockaddr_in.sin_addr.s_addr = htonl(INADDR_ANY); // htonl(INADDR_LOOPBACK);

		std::cout << 	sockaddr_in.sin_port << std::endl;
		std::cout << 	server->port() << std::endl;
		std::cout << sockaddr_in.sin_addr.s_addr << std::endl;

		if (bind(socket_fd, (struct sockaddr *)&sockaddr_in, sizeof(sockaddr_in)))
			throw std::exception();

		if (listen(socket_fd, 500))
			throw std::exception();

		struct pollfd				pollfd;

		pollfd.fd = socket_fd;
		pollfd.events = POLLIN;

		_pollfds.push_back(pollfd);
	}
}

std::ostream &operator<<(std::ostream &out, const WebServ &web_serv) {
	out << "WebServ:: " << std::endl << std::endl;
	std::vector<Server> servers = web_serv.servers();
	for (std::vector<Server>::const_iterator  it = servers.begin(); it != servers.end(); it++)
		out << *it << std::endl;

	return out;
}
