#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include <poll.h>
#include <unistd.h>
#include <sys/socket.h>

class Response {
	private:
		pollfd	_pollfd;
		int			_client_fd;

	public:
		Response(void);
		Response(pollfd const &pollfd, int client_fd);
		Response(Response const &response);
		Response& operator=(Response const &response);
		~Response(void);

		void handle(std::string statuscode, std::string pathHTML);
		void OK(void);
		void RESP300(void);
		void ReadHTML(std::string code_pag, std::string msgStatusCode, std::string pathHTML);
};

std::ostream &operator<<(std::ostream &out, const Response &response);

#endif