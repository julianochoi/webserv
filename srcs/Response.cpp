#include <Response.hpp>
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

void Response::handle(void) {

	//OK();
	//RESP300();
	ReadHTML("101");
}


void Response::ReadHTML(std::string code_pag) {

	const char* buffer;
	int buffer_len;	
	string line;
	string path;
	path = string("root_html/default_responses/") + code_pag + string(".html");
	
	//std::cout << code_pag << std::endl;

	ifstream file(path.c_str());
	if (file.is_open())
	{
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

void Response::OK(void) {
	send(_client_fd, "HTTP/1.1 200 OK\n", 16, 0);
	send(_client_fd, "Content-Type: text/html\n", 24, 0);
	send(_client_fd, "Content-Length: 48\n\n", 20, 0);
	send(_client_fd, "<html><body><h1>Hello, World!</h1></body></html>", 48, 0);
	close(_client_fd);
}

void Response::RESP300(void) {
	send(_client_fd, "<html style='height: 100%;'>\n", 29, 0);
    send(_client_fd, "<head>\n", 7, 0);
    send(_client_fd, "<meta name='viewport' content='width=device-width, minimum-scale=0.1'>\n", 71, 0);
    send(_client_fd, "<title>300 (750x600)</title>\n", 29, 0);
	send(_client_fd, "<style type='text/css'>\n", 24, 0);
	send(_client_fd, "@font-face { font-family: Roboto; src: url('chrome-extension://mcgbeeipkmelnpldkobichboakdfaeon/css/Roboto-Regular.ttf'); }\n", 124, 0);
	send(_client_fd, "</style>\n", 9, 0);
	send(_client_fd, "</head>\n", 8, 0);
	send(_client_fd, "<body style='margin: 0px; background: #0e0e0e; height: 100%>\n", 61, 0);
	send(_client_fd, "<img style='display: block;-webkit-user-select: none;margin: auto;cursor: zoom-in;background-color: hsl(0, 0%, 90%);transition: background-color 300ms;' src='https://http.cat/300' width='580' height='464'>\n", 206, 0);
	send(_client_fd, "</body>\n", 8, 0);
	send(_client_fd, "</html>\n", 8, 0);
	close(_client_fd);
}

std::ostream &operator<<(std::ostream &out, const Response &response) {
	(void)response;
	return out;
}

