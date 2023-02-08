#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <vector>
#include <map>
#include <poll.h>
#include <sys/socket.h>
#include <Utils.hpp>
#include <sstream>

#define BUFFER_SIZE 1

class Request {
	private:
		pollfd								_pollfd;
		std::map<std::string, std::string>	_headers;
		std::string							_body;
		std::string							_method;
		std::string							_path;
		std::string							_query;
		std::string							_protocol;
		std::string							_protocol_version;
		char								*_buffer;
		int 								_client_fd;
		std::string							_total_buffer;


		std::string		_get_line();
		void			_get_full_body(std::size_t size);
		void			_get_chunked_body_line(std::size_t size);
		std::string		_get_chunked_size_line();
		void			_parse_first_line();
		void			_parse_headers();
		void			_parse_chunked_body();
		void			_parse_full_body();

		void			_set_headers(std::string line);
		void			_set_body(std::string line);
		void			_set_method(std::string line);
		void			_set_path(std::string line);
		void			_set_query(std::string line);
		void			_set_protocol_info(std::string line);

	public:
		Request(void);
		Request(pollfd const &pollfd);
		Request(Request const &request);
		Request& operator=(Request const &request);
		~Request(void);

		int handle(void);

		std::map<std::string, std::string>	headers(void) const;
		std::string							body(void) const;
		std::string							method(void) const;
		std::string							path(void) const;
		std::string							query(void) const;
		std::string							protocol(void) const;
		std::string							protocol_version(void) const;

	class ClientConnectionError : public std::exception	{
		public:
			const char* what() const throw(){ return "Client Connection Error"; };
	};
};

std::ostream &operator<<(std::ostream &out, const Request &request);

#endif