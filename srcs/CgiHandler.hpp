#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include <iostream>
#include <map>
#include <cstdio>
#include <algorithm>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include "Server.hpp"
#include "ServerLocation.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Utils.hpp"

typedef std::pair<std::string, std::string> cgi_pair;
typedef std::map<std::string, std::string> env_map;

class CgiHandler {
	private:
		env_map			_env_map;
		std::string	_filepath;
		std::string _full_path;
		char**		_env;
		char**		_argv;
		cgi_pair	_cgi;

		cgi_pair	_get_cgi(Server &server, ServerLocation &location);
		std::string	_get_default_cgi(std::string extension);
		std::string	_get_cgi_output(std::FILE *tmp_out_file);
		void		_send_cgi_response(int client_fd, std::string &buffer);
		void		_add_header_to_env(Request &request);
		char**		_build_env(Server &server, Request &request);
		char**		_build_argv();
		void		_chdir_wrapper(const char *path);
		int			_exec_cgi(int tmp_in_fd, int tmp_out_fd);

	public:
		CgiHandler(void);
		CgiHandler(CgiHandler const&cgi_handler);
		CgiHandler& operator=(CgiHandler const&cgi_handler);
		~CgiHandler(void);

		void build(Server server, ServerLocation location, Request request, std::string filepath, std::string full_path);
		void handle(int client_fd, Request &request);

		class ChdirError : public std::exception
		{
			public:
				const char* what() const throw(){ return "Chdir error"; };
		};

		class CGIError : public std::exception
		{
			public:
				const char* what() const throw(){ return "CGI error"; };
		};

		class InternalServerError : public std::exception
		{
			public:
				const char* what() const throw(){ return "Internal Server error"; };
		};

		class UnsupportedCGI : public std::exception
		{
			private:
				std::string _extension;
			public:
				UnsupportedCGI(std::string extension) : _extension(extension) {};
				~UnsupportedCGI(void) throw () {};

				const char* what() const throw() {
					return ("Unsupported CGI extension: " + _extension).c_str(); 
				};
		};
};

#endif
