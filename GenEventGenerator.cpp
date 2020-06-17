#include "GenEventGenerator.h"


void GenEventGenerator::parseEvents(std::string message) {

	/************
	SI LLEGA PING
	************/
	if ((message.find("PING") != std::string::npos)) {
		this->eventsQueue.push(ping);
	}


	/**********
	si me llega el network layout
	***********/
	else if ((message.find("NETWORK_LAYOUT") != std::string::npos))
	{
		eventsQueue.push(networklayout);
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
	switch (getGENevent())
	{
	case::ping:
		ret = new evPing();
		break;
	case::networklayout:
		ret = new evNetworklayout();
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
	if(!eventsQueue.empty())
		cout << "EVENT" << to_string(eventsQueue.front()) << endl;
	return !eventsQueue.empty();
}