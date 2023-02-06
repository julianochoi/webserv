#include <WebServ.hpp>

#include <cstdio>

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

	addLog(logFile,"Initialize file parser of server");
	_servers = fileParser.parse(argc, argv);
	_start_listening();
}

void WebServ::event_loop(void) {
	int connections;

	
	while (true) {
		/*Looping fica ativo por 15 segundos, para criarmos um timeout*/
		//if (std::difftime(std::time(0), TS) > 15)
		//	break;
		connections = poll((pollfd *)&(*_pollfds.begin()), _pollfds.size(), -1);
		if (connections == -1)
			throw PoolError();

		for (std::vector<pollfd>::const_iterator pollfd = _pollfds.begin(); pollfd != _pollfds.end(); pollfd++)
			if (pollfd->revents) {
				Http http = Http(*pollfd, _servers);
				http.handle();
			}
	}
}

void WebServ::_start_listening(void) {
	int									socket_fd;

	for (std::vector<Server>::const_iterator server = _servers.begin(); server != _servers.end(); server++) {
		std::cout << YELLOW << "Initializing server - " << server->host() << ":" << server->port() << COLOR_OFF << std::endl;
		char portserv[100];
		sprintf(portserv, "%d", server->port());
		addLog(logFile,"Initializing server - " + server->host() + ":" + portserv);

		socket_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (socket_fd == -1)
			throw SocketInitError();

		sockaddr bind_host_addrinfo = server->host_addrinfo();

		if (bind(socket_fd, &bind_host_addrinfo, server->host_addrinfo_len()))
			throw BindInitError();

		if (listen(socket_fd, 500))
			throw ListenInitError();

		struct pollfd				pollfd;

		pollfd.fd = socket_fd;
		pollfd.events = POLLIN;

		_pollfds.push_back(pollfd);

		addLog(logFile,"Successfully initialized");
		std::cout << GREEN << "Successfully initialized" << COLOR_OFF << std::endl;
	}
}

std::ostream &operator<<(std::ostream &out, const WebServ &web_serv) {
	out << "WebServ:: " << std::endl << std::endl;
	std::vector<Server> servers = web_serv.servers();
	for (std::vector<Server>::const_iterator  it = servers.begin(); it != servers.end(); it++)
		out << *it << std::endl;

	return out;
}
