#include <Response.hpp>
#include <Request.hpp>
#include <Server.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <sys/stat.h>

using namespace std;

Response::Response(void) {}

Response::Response(pollfd const &pollfd, int client_fd): _pollfd(pollfd), _client_fd(client_fd) {}

Response::Response(Response const &response) {
	_pollfd = response._pollfd;
	_client_fd = response._client_fd;
}

Response &Response::operator=(Response const &response) {
	_pollfd = response._pollfd;
	_client_fd = response._client_fd;
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


void Response::handle(std::string statuscode, std::string pathHTML) {
	std::map<std::string, std::string> MapStatusCode;

	MapStatusCode.insert(std::make_pair("100", "Continue"));
	MapStatusCode.insert(std::make_pair("101", "Switching Protocols"));
	MapStatusCode.insert(std::make_pair("102", "Processing"));
	MapStatusCode.insert(std::make_pair("200", "OK"));
	MapStatusCode.insert(std::make_pair("201", "Created"));
	MapStatusCode.insert(std::make_pair("202", "Accepted"));
	MapStatusCode.insert(std::make_pair("203", "Non-Authoritative Information"));
	MapStatusCode.insert(std::make_pair("204", "No Content"));
	//MapStatusCode.insert(std::make_pair("205", "Reset Content"));
	MapStatusCode.insert(std::make_pair("206", "Partial Content"));
	MapStatusCode.insert(std::make_pair("207", "Multi-Status"));
	//MapStatusCode.insert(std::make_pair("208", "Already Reported"));
	//MapStatusCode.insert(std::make_pair("226", "IM Used"));
	MapStatusCode.insert(std::make_pair("300", "Multiple Choices"));
	MapStatusCode.insert(std::make_pair("301", "Moved Permanently"));
	MapStatusCode.insert(std::make_pair("302", "Found"));
	MapStatusCode.insert(std::make_pair("303", "See Other"));
	MapStatusCode.insert(std::make_pair("304", "Not Modified"));
	MapStatusCode.insert(std::make_pair("305", "Use Proxy"));
	//MapStatusCode.insert(std::make_pair("306", "Switch Proxy"));
	MapStatusCode.insert(std::make_pair("307", "Temporary Redirect"));
	MapStatusCode.insert(std::make_pair("308", "Permanent Redirect"));
	MapStatusCode.insert(std::make_pair("400", "Bad Request"));
	MapStatusCode.insert(std::make_pair("401", "Unauthorized"));
	MapStatusCode.insert(std::make_pair("402", "Payment Required"));
	MapStatusCode.insert(std::make_pair("403", "Forbidden"));
	MapStatusCode.insert(std::make_pair("404", "Not Found"));
	MapStatusCode.insert(std::make_pair("405", "Method Not Allowed"));
	MapStatusCode.insert(std::make_pair("406", "Not Acceptable"));
	MapStatusCode.insert(std::make_pair("407", "Proxy Authentication Required"));
	MapStatusCode.insert(std::make_pair("408", "Request Timeout"));
	MapStatusCode.insert(std::make_pair("409", "Conflict"));
	MapStatusCode.insert(std::make_pair("410", "Gone"));
	MapStatusCode.insert(std::make_pair("411", "Length Required"));
	MapStatusCode.insert(std::make_pair("412", "Precondition Failed"));
	MapStatusCode.insert(std::make_pair("413", "Payload Too Large"));
	MapStatusCode.insert(std::make_pair("414", "URI Too Long"));
	MapStatusCode.insert(std::make_pair("415", "Unsupported Media Type"));
	MapStatusCode.insert(std::make_pair("416", "Range Not Satisfiable"));
	MapStatusCode.insert(std::make_pair("417", "Expectation Failed"));
	MapStatusCode.insert(std::make_pair("418", "'Im a teapot'"));
	MapStatusCode.insert(std::make_pair("420", "Enhance Your Calm"));
	MapStatusCode.insert(std::make_pair("421", "Misdirected Request"));
	MapStatusCode.insert(std::make_pair("422", "Unprocessable Entity"));
	MapStatusCode.insert(std::make_pair("423", "Locked"));
	MapStatusCode.insert(std::make_pair("424", "Failed Dependency"));
	MapStatusCode.insert(std::make_pair("425", "Too Early"));
	MapStatusCode.insert(std::make_pair("426", "Upgrade Required"));
	//MapStatusCode.insert(std::make_pair("428", "Precondition Required"));
	MapStatusCode.insert(std::make_pair("429", "Too Many Requests"));
	MapStatusCode.insert(std::make_pair("431", "Request Header Fields Too Large"));
	MapStatusCode.insert(std::make_pair("444", "No Response"));
	MapStatusCode.insert(std::make_pair("450", "Blocked by Windows Parental Controls"));
	MapStatusCode.insert(std::make_pair("451", "Unavailable For Legal Reasons"));
	MapStatusCode.insert(std::make_pair("497", "HTTP Request Sent to HTTPS Port"));
	MapStatusCode.insert(std::make_pair("498", "Token expired/invalid"));
	MapStatusCode.insert(std::make_pair("499", "Client Closed Request"));
	MapStatusCode.insert(std::make_pair("500", "Internal Server Error"));
	MapStatusCode.insert(std::make_pair("501", "Not Implemented"));
	MapStatusCode.insert(std::make_pair("502", "Bad Gateway"));
	MapStatusCode.insert(std::make_pair("503", "Service Unavailable"));
	MapStatusCode.insert(std::make_pair("504", "Gateway Timeout"));
	MapStatusCode.insert(std::make_pair("505", "HTTP Version Not Supported"));
	MapStatusCode.insert(std::make_pair("506", "Variant Also Negotiates"));
	MapStatusCode.insert(std::make_pair("507", "Insufficient Storage"));
	MapStatusCode.insert(std::make_pair("508", "Loop Detected"));
	MapStatusCode.insert(std::make_pair("509", "Bandwidth Limit Exceeded"));
	MapStatusCode.insert(std::make_pair("510", "Not Extended"));
	MapStatusCode.insert(std::make_pair("511", "Network Authentication Required"));
	MapStatusCode.insert(std::make_pair("521", "Web Server Is Down"));
	MapStatusCode.insert(std::make_pair("522", "Connection Timed Out"));
	MapStatusCode.insert(std::make_pair("523", "Origin Is Unreachable"));
	MapStatusCode.insert(std::make_pair("525", "SSL Handshake Failed"));
	MapStatusCode.insert(std::make_pair("599", "Network connect timeout error"));

	MapStatusCode.insert(std::make_pair("index2", "Continue"));
	MapStatusCode.insert(std::make_pair("index3", "Continue"));

	if (MapStatusCode.find(statuscode) == MapStatusCode.end())
		statuscode = "404";
	//addLog(logFile,"StatusCode " + MapStatusCode.find(statuscode)->second);

	ReadHTML(statuscode, MapStatusCode.find(statuscode)->second, pathHTML);
}



void Response::ReadHTML(std::string code_pag, std::string msgStatusCode, std::string pathHTML) {

	const char* buffer;
	int buffer_len;
	string line;
	struct stat file_status;
	char temp[100];
	string bodylength;
	//string fullpath;

	/*if (isdigit(code_pag[0]))
		fullpath = "root_html/default_responses/" + code_pag + string(".html");
	if (code_pag[0] == 'i')
		fullpath = "root_html/" + code_pag + string(".html");*/

	//fullpath = string(path) + code_pag + string(".html");
	addLog(logFile,"MsgCode " + msgStatusCode);
	addLog(logFile,"Path " + pathHTML);

	if (pathHTML == "")
		pathHTML = "root_html/default_responses/" + code_pag + string(".html");

	std::cout << code_pag << std::endl;

	if (stat(pathHTML.c_str(), &file_status) == 0){
		sprintf(temp, "%ld", file_status.st_size);
		addLog(logFile,"Body bytes: " + std::string(temp));
		std::cout << "The size of the file is: " << file_status.st_size << " bytes." << std::endl;
		std::cout << "The size of the file is: " << 17 + file_status.st_size << " bytes." << std::endl;
	}


	//ifstream file(fullpath.c_str());
	ifstream file(pathHTML.c_str());
	if (file.is_open())
	{
		send(_client_fd, "HTTP/1.1 ", 9, 0);
		send(_client_fd, (code_pag + " ").c_str(), 4, 0);
		send(_client_fd, msgStatusCode.c_str(), msgStatusCode.length(), 0);
		send(_client_fd, "\n", 1, 0);
		
		bodylength = "Content-Length: " + std::string(temp) + "\n";
		send(_client_fd, bodylength.c_str(), bodylength.length(), 0);
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
		file.close();
	}
	close(_client_fd);
}


std::ostream &operator<<(std::ostream &out, const Response &response) {
	(void)response;
	return out;
}

