#include <WebServ.hpp>
#include <Utils.hpp>

WebServ web_serv;

int	main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	Utils::listen_signals();

	web_serv.init();
	web_serv.event_loop();

	// std::cout << web_serv << std::endl;

	return (0);
}