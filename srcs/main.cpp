#include <WebServ.hpp>

int	main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	WebServ web_serv;
	web_serv.init();
	web_serv.event_loop();

	// std::cout << web_serv << std::endl;

	return (0);
}