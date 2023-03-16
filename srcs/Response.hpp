#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include <poll.h>
#include <unistd.h>
#include <sys/socket.h>
#include <Request.hpp>
#include <Server.hpp>
#include <Utils.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <sys/stat.h>

#include <dirent.h>
#include <stdio.h>
#include <cstdio>

class Response {
	private:
		pollfd	_pollfd;
		int			_client_fd;
		std::string _response_buffer;


	public:
		Response(void);
		Response(pollfd const &pollfd, int client_fd);
		Response(Response const &response);
		Response& operator=(Response const &response);
		~Response(void);

		void handle(std::string statuscode, std::string pathHTML, bool autoindex, std::string data);
		void send_safe();
		void OK(void);
		void RESP300(void);
		void ReadHTML(std::string code_pag, std::string msgStatusCode, std::string pathHTML, bool autoindex, std::string data);

	class SendError : public std::exception	{
		public:
			const char* what() const throw(){ return "Send Error"; };
	};
};

std::ostream &operator<<(std::ostream &out, const Response &response);

#endif