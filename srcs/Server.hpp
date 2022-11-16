#ifndef SERVER_HPP
#define SERVER_HPP

#include <stdlib.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <ServerLocation.hpp>
#include <Utils.hpp>

class ServerLocation;

class Server {
	private:
		std::vector<std::string>							_server_names;
		std::string														_host;
		int																		_port;
		std::map<int, std::string>						_erros_pages;
		std::vector<std::string>							_http_methods;
		std::pair<int, std::string>						_http_redirect;
		std::string														_root;
		int																		_body_size_limit;
		bool																	_autoindex;
		std::vector<std::string>							_index;
		std::map<std::string, ServerLocation>	_locations;
		std::string														_cgi_extension;

		void	_parse_location_attributes(std::ifstream &fs, std::string line, std::string path);
		void	_set_server_attributes(std::vector<std::string> line_tokens);
		void	_set_listen_attribute(std::vector<std::string> line_tokens);
		void	_set_server_name_attribute(std::vector<std::string> line_tokens);
		void	_set_error_page_attribute(std::vector<std::string> line_tokens);
		void	_set_client_body_size_attribute(std::vector<std::string> line_tokens);
		void	_set_cgi_attribute(std::vector<std::string> line_tokens);
		void	_set_http_methods_attribute(std::vector<std::string> line_tokens);
		void	_set_http_redirect_attribute(std::vector<std::string> line_tokens);
		void	_set_root_attribute(std::vector<std::string> line_tokens);
		void	_set_autoindex_attribute(std::vector<std::string> line_tokens);
		void	_set_index_attribute(std::vector<std::string> line_tokens);

	public:
		Server(void);
		Server(Server const &server);
		Server& operator=(Server const &server);
		~Server(void);

		void	parse_server_attributes(std::ifstream &fs, std::string line);

		std::vector<std::string>							server_names(void) const;
		std::string														host(void) const;
		int																		port(void) const;
		std::map<int, std::string>						erros_pages(void) const;
		std::vector<std::string>							http_methods(void) const;
		std::pair<int, std::string>						http_redirect(void) const;
		std::string														root(void) const;
		int																		body_size_limit(void) const;
		bool																	autoindex(void) const;
		std::vector<std::string>							index(void) const;
		std::map<std::string, ServerLocation>	locations(void) const;
		std::string														cgi_extension(void) const;
		ServerLocation												location(std::string path) const;
};

std::ostream &operator<<(std::ostream &out, const Server &server);

#endif
