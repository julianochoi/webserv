#include <Response.hpp>
#include <Request.hpp>
#include <Server.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
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


/*
void loadMapStatusCode(void){
	std::map<std::string, std::string> MapStatusCode;
    MapStatusCode.insert(std::make_pair("100", "root_html/default_responses/"));
	MapStatusCode.insert(std::make_pair("404", "root_html/default_responses/"));
	MapStatusCode.insert(std::make_pair("index2", "root_html/"));
	MapStatusCode.insert(std::make_pair("index3", "root_html/"));
}*/



void Response::handle(std::string statuscode) {
	std::map<std::string, std::string> MapStatusCode;


    MapStatusCode.insert(std::make_pair("100", "root_html/default_responses/"));
	MapStatusCode.insert(std::make_pair("404", "root_html/default_responses/"));
	MapStatusCode.insert(std::make_pair("index2", "root_html/"));
	MapStatusCode.insert(std::make_pair("index3", "root_html/"));

	/*loadMapStatusCode();*/
	addLog(logFile,"Path: " + MapStatusCode.find(statuscode)->second);
	/*Caso nao encontre no map, forca ser 404*/
	if (MapStatusCode.find(statuscode)->second == "")
		statuscode = "404";

	/*
	Request request = Request(_pollfd);
	std::string	 MMM = request.method();
	std::cout << MMM << std::endl;
	addLog(logFile,"TESTE " + MMM);
	addLog(logFile,"TESTE " + std::string(MMM));

	addLog(logFile,"TESTE " + request.method());
	addLog(logFile,"TESTE " + request.body());
	addLog(logFile,"TESTE " + request.path());
	*/
	ReadHTML(statuscode, MapStatusCode.find(statuscode)->second);
}



void Response::ReadHTML(std::string code_pag, std::string path) {

	const char* buffer;
	int buffer_len;	
	string line;
	string fullpath;
	fullpath = string(path) + code_pag + string(".html");
	
	std::cout << code_pag << std::endl;
	ifstream file(fullpath.c_str());
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

