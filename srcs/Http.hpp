#ifndef HTTP_HPP
#define HTTP_HPP

#include <iostream>
#include <poll.h>
#include <unistd.h>
#include <Request.hpp>
#include <Response.hpp>
#include <Server.hpp>

class Http {
	private:
		pollfd					_pollfd;
		std::vector<Server>		_servers;
		Server					_http_server;


		void _set_http_server(Request request);

	public:
		Http(void);
		Http(Http const &http);
		Http(pollfd const &pollfd, std::vector<Server> servers);
		Http& operator=(Http const &http);
		~Http(void);

		void handle();
};

std::ostream &operator<<(std::ostream &out, const Http &http);

#endif
