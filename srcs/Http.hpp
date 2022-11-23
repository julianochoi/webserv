#ifndef HTTP_HPP
#define HTTP_HPP

#include <iostream>
#include <poll.h>
#include <unistd.h>
#include <Request.hpp>
#include <Response.hpp>

class Http {
	private:
		pollfd	_pollfd;

	public:
		Http(void);
		Http(Http const &http);
		Http(pollfd const &pollfd);
		Http& operator=(Http const &http);
		~Http(void);

		void handle(void);
};

std::ostream &operator<<(std::ostream &out, const Http &http);

#endif
