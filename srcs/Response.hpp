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

		void handle(void);
};

std::ostream &operator<<(std::ostream &out, const Response &response);

#endif