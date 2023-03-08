#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <iostream>
#include <FileParser.hpp>
#include <Server.hpp>
#include <Http.hpp>
#include <Utils.hpp>

#include <cstdio>

class WebServ {
	private:
		std::vector<Server>	_servers;
		std::vector<pollfd> _pollfds;
		std::map<int, Http> _client_list;


		void _start_listening(void);

	public:
		WebServ(void);
		WebServ(WebServ const &web_serv);
		WebServ& operator=(WebServ const &web_serv);
		~WebServ(void);

		std::vector<Server> servers(void) const;

		void init(int argc, char **argv);
		void event_loop(void);

		class SocketInitError : public std::exception
		{
			public:
				const char* what() const throw(){ return "Socket init error"; };
		};

		class SocketOptError : public std::exception
		{
			public:
				const char* what() const throw(){ return "Socket opt error"; };
		};

		class BindInitError : public std::exception
		{
			public:
				const char* what() const throw(){ return "Bind init error"; };
		};

		class ListenInitError : public std::exception
		{
			public:
				const char* what() const throw(){ return "Listen init error"; };
		};

		class PoolError : public std::exception
		{
			public:
				const char* what() const throw(){ return "Poll error"; };
		};
};

std::ostream &operator<<(std::ostream &out, const WebServ &web_serv);

#endif
