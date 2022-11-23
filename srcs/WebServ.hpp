#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <FileParser.hpp>
#include <Server.hpp>
#include <Http.hpp>

class WebServ {
	private:
		std::vector<Server>		_servers;
		std::vector<pollfd> _pollfds;

		void _start_listening(void);

	public:
		WebServ(void);
		WebServ(WebServ const &web_serv);
		WebServ& operator=(WebServ const &web_serv);
		~WebServ(void);

		std::vector<Server> servers(void) const;

		void init(void);
		void event_loop(void);
};

std::ostream &operator<<(std::ostream &out, const WebServ &web_serv);

#endif
