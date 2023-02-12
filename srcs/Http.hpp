#ifndef HTTP_HPP
#define HTTP_HPP

#include <iostream>
#include <poll.h>
#include <unistd.h>
#include <Request.hpp>
#include <Response.hpp>
#include <Server.hpp>
#include <ServerLocation.hpp>

class Http {
	private:
		pollfd					_pollfd;
		std::vector<Server>		_servers;
		Server					_http_server;
		ServerLocation	_http_location;
		bool						_has_location;
		std::string			_remaining_path;
		Response				_response;


		void _set_http_server(Request request);
		void _set_location(Request request);
		void _response_handler(Request request);
		void _get_handler(std::string response_file_path);
		std::string _get_file_error(std::string status_code);

		std::string								_root(void) const;
		std::string				_index(void) const;
		std::map<int, std::string>				_erros_pages(void) const;


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
