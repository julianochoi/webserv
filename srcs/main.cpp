#include <WebServ.hpp>
#include <Utils.hpp>

#include <iostream>
#include <ctime>

time_t TS;

WebServ web_serv;

int	main(int argc, char **argv) {
	Utils::listen_signals();

	remove(logFile);
	TS = time(0);
	
	addLog(logFile,"Start main");

	std::cout << "Chronometer started." << std::endl;
	std::time_t start = std::time(0);
	while ( std::difftime(std::time(0), start) < 1) {
		//std::cout << "Elapsed time: " << std::difftime(std::time(0), start) << " seconds" << std::endl;
	}
	std::cout << "Chronometer finished." << std::endl;

	std::cout << "Elapsed time: " << std::difftime(std::time(0), TS) << " seconds" << std::endl;

	try {
		addLog(logFile,"Initialize Server");
		web_serv.init(argc, argv);
		//std::cout << web_serv << std::endl;
		addLog(logFile,"Initialize Server Event Loop");
		web_serv.event_loop();
	} catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
		return(1);
	}
	 

	return (0);

	}