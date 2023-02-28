#ifndef SERVERLOCATION_HPP
#define SERVERLOCATION_HPP

#include <stdlib.h>
#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <Utils.hpp>
#include <Server.hpp>

class Server;

class ServerLocation {
	private:
		std::vector<std::string>		_http_methods;
		std::pair<std::string, std::string>	_http_redirect;
		std::string									_root;
		bool												_autoindex;
		std::vector<std::string>		_index;
		std::string									_cgi_extension;
		std::map<int, std::string>	_erros_pages;
		int													_body_size_limit;

		void	_set_location_attributes(std::vector<std::string>);
		void	_set_error_page_attribute(std::vector<std::string> line_tokens);
		void	_set_client_body_size_attribute(std::vector<std::string> line_tokens);
		void	_set_cgi_attribute(std::vector<std::string> line_tokens);
		void	_set_http_methods_attribute(std::vector<std::string> line_tokens);
		void	_set_http_redirect_attribute(std::vector<std::string> line_tokens);
		void	_set_root_attribute(std::vector<std::string> line_tokens);
		void	_set_autoindex_attribute(std::vector<std::string> line_tokens);
		void	_set_index_attribute(std::vector<std::string> line_tokens);


	public:
		ServerLocation(void);
		ServerLocation(Server const &server);
		ServerLocation(ServerLocation const &server_location);
		ServerLocation& operator=(ServerLocation const &server_location);
		~ServerLocation(void);

		void parse_location_attributes(std::ifstream &fs, std::string line);

		std::map<int, std::string>						erros_pages(void) const;
		std::vector<std::string>							http_methods(void) const;
		std::pair<std::string, std::string>						http_redirect(void) const;
		std::string														root(void) const;
		int																		body_size_limit(void) const;
		bool																	autoindex(void) const;
		std::vector<std::string>							index(void) const;
		std::string														cgi_extension(void) const;

	class InvalidLocationParam : public std::exception
	{
		public:
			const char* what() const throw(){ return "Invalid Location Param"; };
	};

	class MissingLocationArgs : public std::exception
	{
		public:
			const char* what() const throw(){ return "Missing Location Args"; };
	};

	class InvalidRoot: public std::exception
	{
		public:
			const char* what() const throw(){ return "Invalid Root"; };
	};

	class InvalidBodySize: public std::exception
	{
		public:
			const char* what() const throw(){ return "Invalid Body Size"; };
	};

	class InvalidCGIExtension: public std::exception
	{
		public:
			const char* what() const throw(){ return "Invalid CGI Extension"; };
	};

	class InvalidAutoIndexParam: public std::exception
	{
		public:
			const char* what() const throw(){ return "Invalid AutoIndex - Should be on or off"; };
	};

	class InvalidRedirect: public std::exception
	{
		public:
			const char* what() const throw(){ return "Invalid Redirect Config"; };
	};
};

std::ostream &operator<<(std::ostream &out, ServerLocation const &server_location);


#endif