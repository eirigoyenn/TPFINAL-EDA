#pragma once
#include <iostream>
#include "EventHandling.h"
#include "GEN_FSM.h"


enum GENevents : eventTypes { ping , networklayout ,networkready , networknotready ,Noevent,timer};

class GenEventGenerator : public eventGenerator
{
public:
	void parseEvents(std::string message);
	genericEvent* getEvent(unsigned int estado);
	GENevents getGENevent(void);
	bool anyEvent(void);
private:
	std::queue<GENevents> eventsQueue;

};

class evPing : public genericEvent
{
public:
	eventTypes getType(void) { return ping; }
};


class evNetworklayout : public genericEvent
{
public:
	eventTypes getType(void) { return networklayout; }
};


class evNetworkready : public genericEvent
{
public:
	eventTypes getType(void) { return networkready; }
};


class evNetworknotready : public genericEvent
{
public:
	eventTypes getType(void) { return networknotready; }
};


class evNoevent : public genericEvent
{
public:
	eventTypes getType(void) { return Noevent; }
};