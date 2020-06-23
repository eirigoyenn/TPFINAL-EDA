#include "GenEventGenerator.h"


void GenEventGenerator::parseEvents(std::string message) {


	/************
	SI LLEGA PING
	************/
	if ((message.find("PING") != std::string::npos)) {
		this->eventsQueue.push(GENevents::ping);
		std::size_t found = message.find("PUERTO:");
		std::size_t found2 = message.find("HTTP");

		if ((found != std::string::npos) && (found2 != std::string::npos))
		{
			std::string substring = message.substr(found + 7, found2 - 29);

			std::cout << "\n\nPUERTO" + substring << "FIN" << std::endl << std::endl;
			PUERTO = atoi(substring.c_str());
		}

	}


	/**********
	si me llega el network layout
	***********/
	else if ((message.find("NETWORK_LAYOUT") != std::string::npos))
	{
		eventsQueue.push(networklayout);
		LAYOUT = message; 
	}

	/**********
	si me llega el network ready
	***********/
	else if ((message.find("NETWORK_READY") != std::string::npos))
	{
		eventsQueue.push(networkready);
	}
	/**********
	si me llega el network not ready
	***********/
	else if ((message.find("NETWORK_NOTREADY") != std::string::npos))
	{
		eventsQueue.push(networknotready);
	}
	

}


genericEvent* GenEventGenerator::getEvent(unsigned int estado)
{
	genericEvent* ret = nullptr;
	int puerto; 
	switch (getGENevent())
	{
	case::ping:
		puerto = PUERTO;
		ret = new evPing(puerto);
		break;

	case::networklayout:
		ret = new evNetworklayout(LAYOUT);
		break;
	case::networkready:
		ret = new evNetworkready();
		break;
	case::networknotready:
		ret = new evNetworknotready();
		break;

	case::Noevent:
		ret = new evNoevent();
		break;
	}
	return ret;
}

GENevents GenEventGenerator::getGENevent()
{
	GENevents Event = GENevents::Noevent;

	if (!eventsQueue.empty())
	{
		Event = eventsQueue.front();
		eventsQueue.pop();
	}
	return Event;
}


bool GenEventGenerator::anyEvent()
{
	return !eventsQueue.empty();
}