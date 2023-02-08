#include <Response.hpp>
#include <Request.hpp>
#include <Server.hpp>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

Response::Response(void) {}

Response::Response(pollfd const &pollfd, int client_fd): _pollfd(pollfd), _client_fd(client_fd) {}

Response::Response(Response const &response) {
	(void)response;
}

Response &Response::operator=(Response const &response) {
	(void)response;
	return *this;
}

Response::~Response(void) {}

void Response::handle(std::string statuscode) {

	//OK();
	//RESP300();
	/*std::cout << "Server Names: " << std::endl;
	std::vector<std::string> server_names = server.server_names();
	for(std::vector<std::string>::const_iterator  it = server_names.begin(); it != server_names.end(); it++)
		std::cout << " " << *it << std::endl;*/

	//std::cout << "Param - " << Server->listen_atributes[1] << std::endl;
	//ReadHTML("100");
	Request request = Request(_pollfd);
	std::string	 MMM = request.method();
	std::cout << MMM << std::endl;
	ReadHTML(statuscode);
}



void Response::ReadHTML(std::string code_pag) {

	const char* buffer;
	int buffer_len;	
	string line;
	string path;
	//path = string("root_html/default_responses/") + code_pag + string(".html");
	path = string("root_html/default_responses/") + code_pag + string(".html");
	
	std::cout << code_pag << std::endl;
	ifstream file(path.c_str());
	if (file.is_open())
	{
		send(_client_fd, "HTTP/1.1 200 OK\n", 16, 0);
		send(_client_fd, "Content-Type: text/html\n", 24, 0);
		send(_client_fd, "\n", 1, 0);
		while (getline(file, line))
		{
			//std::cout << "line" << std::endl;
			//std::cout << line << " - " << line.length() <<std::endl;
			buffer=line.c_str();
			buffer_len=line.length();
			send(_client_fd, buffer, buffer_len, 0);
			send(_client_fd, "\n", 1, 0);
		}
	}
	close(_client_fd);
}




std::ostream &operator<<(std::ostream &out, const Response &response) {
	(void)response;
	return out;
}

