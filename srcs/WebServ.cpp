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

WebServ::~WebServ(void) {
	_client_list.clear();
}

std::vector<Server> WebServ::servers(void) const { return _servers; }


void WebServ::init(int argc, char **argv) {
	FileParser	fileParser;

	addLog(logFile,"Initialize file parser of server");
	_servers = fileParser.parse(argc, argv);
	_start_listening();
}

void WebServ::event_loop(void) {
	int connections;
	int client_fd;
	int servers_size = _pollfds.size();


	try {
		addLog(logFile,"Start Polling");

		while (true) {
			/*Looping fica ativo por 15 segundos, para criarmos um timeout*/
			//if (std::difftime(std::time(0), TS) > 15)
			//	break;
			connections = poll((pollfd *)&(*_pollfds.begin()), _pollfds.size(), -1);
			if (connections == -1)
				throw PoolError();

			for (int i = 0; i < servers_size; i++)
				if (_pollfds[i].revents & POLLIN) {
	        client_fd = accept(_pollfds[i].fd, NULL, NULL);

          _client_list[client_fd] = Http(_pollfds[i], _servers, client_fd);
					_client_list[client_fd].handle();
				}
		}
	}
	catch (...) {
		addLog(logFile,"Event loop Error");
		event_loop();
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

		if (fcntl(socket_fd, F_SETFL, O_NONBLOCK) == -1)
			throw SocketInitError();

		sockaddr bind_host_addrinfo = server->host_addrinfo();

		int yes =1;
		if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes))
			throw SocketOptError();

		struct timeval tv_recv;
		tv_recv.tv_sec = 15;
		tv_recv.tv_usec = 0;
		if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv_recv, sizeof tv_recv))
			throw SocketOptError();

		struct timeval tv_send;
		tv_send.tv_sec = 15;
		tv_send.tv_usec = 0;
		if (setsockopt(socket_fd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv_send, sizeof tv_send))
			throw SocketOptError();

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
