#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <poll.h>
#include <sys/socket.h>

class Request {
	private:
		pollfd	_pollfd;

	public:
		Request(void);
		Request(pollfd const &pollfd);
		Request(Request const &request);
		Request& operator=(Request const &request);
		~Request(void);

		int handle(void);

	class ClientConnectionError : public std::exception
		{
			public:
				const char* what() const throw(){ return "Client Connection Error"; };
		};
};

std::ostream &operator<<(std::ostream &out, const Request &request);

#endif