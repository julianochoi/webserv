#include <WebServ.hpp>
#include <Utils.hpp>


WebServ web_serv;

int	main(int argc, char **argv)
{

	try {
		web_serv.init(argc, argv);
		//std::cout << web_serv << std::endl;
		web_serv.event_loop();
	} catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
		return(1);
	}
	 

	return (0);
}