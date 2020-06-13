#include "GenEventGenerator.h"


void GenEventGenerator::parseEvents(std::string message) {

	/************
	SI LLEGA PING
	************/
	if ((message.find("PING") != std::string::npos)) {

	}


	/**********
	si me llega el network layout
	***********/
	else if ((message.find("NETWORK_LAYOUT") != std::string::npos))
	{
		GenEventsGenerator(message);
	}

	/**********
	si me llega el network ready
	***********/
	else if ((message.find("NETWORK_READY") != std::string::npos))
	{
		GenEventsGenerator(message);

	}
	else if ((message.find("NETWORK_NOTREADY") != std::string::npos))
	{

	}
}
