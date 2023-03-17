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
#define MAX_URI_LENGTH 1000000

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
		char								_buffer;
		int 								_client_fd;
		std::string							_total_buffer;
		bool										_headers_error;
		int 						_is_complete;
		int 						_parse_steps;
		int 						_line_found;
		int 						_chunk_parse;
		std::size_t 		_chunk_total_size;
		std::size_t			_chunk_size;



		std::string		_get_line();
		void			_get_full_body(std::size_t size);
		void			_get_chunked_body_line(std::size_t size);
		std::string		_get_chunked_size_line();
		void			_parse_first_line();
		void			_parse_headers();
		void			_parse_chunked_body();
		void			_parse_full_body();
		int			_validate_content_length();

		void			_set_headers(std::string line);
		void			_set_body(std::string line);
		void			_set_method(std::string line);
		void			_set_path(std::string line);
		void			_set_query(std::string line);
		void			_set_protocol_info(std::string line);

		ssize_t		_recv_safe(int __fd, void *__buf, size_t __n, int __flags);


	public:
		Request(void);
		Request(pollfd const &pollfd, int client_fd);
		Request(Request const &request);
		Request& operator=(Request const &request);
		~Request(void);

		void handle(void);
		int is_complete(void) const;

		std::map<std::string, std::string>	headers(void) const;
		std::string							body(void) const;
		std::string							method(void) const;
		std::string							path(void) const;
		std::string							query(void) const;
		std::string							protocol(void) const;
		std::string							protocol_version(void) const;

	class BadRequestError : public std::exception	{
		public:
			const char* what() const throw(){ return "Bad Request Error"; };
	};

	class URITooLongError : public std::exception	{
		public:
			const char* what() const throw(){ return "URI Too Long Error"; };
	};

	class InternalServerError : public std::exception	{
		public:
			const char* what() const throw(){ return "Internal Server Error"; };
	};

	class RecvError : public std::exception	{
		public:
			const char* what() const throw(){ return "Recv Error"; };
	};
};

std::ostream &operator<<(std::ostream &out, const Request &request);

#endif