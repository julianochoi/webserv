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


void WebServ::init(int argc, char **argv) {
	FileParser	fileParser;

	_servers = fileParser.parse(argc, argv);
	_start_listening();
}

void WebServ::event_loop(void) {
	int connections;

	while (true) {
		connections = poll((pollfd *)&(*_pollfds.begin()), _pollfds.size(), -1);
		std::cout << "conn " << connections << std::endl;
		if (connections == -1)
			throw std::exception();

		for (std::vector<pollfd>::const_iterator pollfd = _pollfds.begin(); pollfd != _pollfds.end(); pollfd++)
			if (pollfd->revents) {
				Http http = Http(*pollfd);
				http.handle();
			}
	}
}

void WebServ::_start_listening(void) {
	int									socket_fd;

	for (std::vector<Server>::const_iterator server = _servers.begin(); server != _servers.end(); server++) {
		socket_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (socket_fd == -1)
			throw std::exception();

		sockaddr bind_host_addrinfo = server->host_addrinfo();

		std::cout << 	server->host() << std::endl;
		std::cout << 	server->port() << std::endl;
		std::cout << 	&bind_host_addrinfo << std::endl;

		if (bind(socket_fd, &bind_host_addrinfo, server->host_addrinfo_len()))
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
