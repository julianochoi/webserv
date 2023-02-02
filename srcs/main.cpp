#include <WebServ.hpp>
#include <Utils.hpp>


WebServ web_serv;

int	main(int argc, char **argv)
{
	Utils::listen_signals();

	try {
		web_serv.init(argc, argv);
		web_serv.event_loop();
	} catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
		return(1);
	}

	// std::cout << web_serv << std::endl;

	return (0);
}