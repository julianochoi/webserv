#include <WebServ.hpp>
#include <Utils.hpp>
#include <iostream>

WebServ web_serv;

int	main(int argc, char **argv) {
	Utils::listen_signals();

	remove(logFile);
	addLog(logFile,"Start main");

	try {
		addLog(logFile,"Init HTML");
		addLog(logFile,"Initialize Server");
		web_serv.init(argc, argv);
		addLog(logFile,"Initialize Server Event Loop");
		web_serv.event_loop();
	} catch (const std::exception& e) {
		addLog(logFile, e.what());
		std::cout << e.what() << std::endl;
		return(1);
	}
	 
	return (0);
	}