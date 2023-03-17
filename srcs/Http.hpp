#ifndef HTTP_HPP
#define HTTP_HPP

#include <iostream>
#include <poll.h>
#include <algorithm>
#include <unistd.h>
#include <Request.hpp>
#include <Response.hpp>
#include <Server.hpp>
#include <ServerLocation.hpp>
#include <CgiHandler.hpp>

class Http {
	private:
		pollfd					_pollfd;
		std::vector<Server>		_servers;
		Server					_http_server;
		ServerLocation	_http_location;
		bool						_has_location;
		std::string			_remaining_path;
		Request					_request;
		Response				_response;
		int 						_client_fd;
		int 						_is_complete;


		void _set_http_server();
		void _set_location();
		bool _check_cgi(CgiHandler &cgi_handler);
		bool _has_cgi_extension(std::string filename, std::string cgi_extension);
		void _response_handler();
		void _get_handler(std::string response_file_path);
		bool _validate_request();
		std::string _get_file_error(std::string status_code);
 
		std::string								_root(void) const;
		std::string								_index(void) const;
		std::string								_cgi_extension(void) const;
		std::string								_cgi_path(void) const;
		size_t									_cgi_timeout(void) const;
		std::map<int, std::string>				_erros_pages(void) const;
		int										_body_size_limit(void) const;
		bool									_autoindex(void) const;
		std::vector<std::string>				_http_methods(void) const;
		std::pair<std::string, std::string>		_http_redirect(void) const;

		void _response_handle_safe(std::string statuscode, std::string pathHTML, bool autoindex, std::string data);

	public:
		Http(void);
		Http(Http const &http);
		Http(pollfd const &pollfd, std::vector<Server> servers, int client_fd);
		Http& operator=(Http const &http);
		~Http(void);

		void handle();
		void send_safe();
		int is_complete();

	class ClientConnectionError : public std::exception	{
		public:
			const char* what() const throw(){ return "Client Connection Error"; };
	};
};

std::ostream &operator<<(std::ostream &out, const Http &http);

#endif
