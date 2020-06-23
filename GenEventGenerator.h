#pragma once
#include <iostream>
#include "EventHandling.h"
#include "GEN_FSM.h"
#include "json.hpp"

enum GENevents : eventTypes { ping , networknotready , networkready, networkcreated ,Noevent,timer, networklayout};

class GenEventGenerator : public eventGenerator
{
public:
	void parseEvents(std::string message);
	genericEvent* getEvent(unsigned int estado);
	GENevents getGENevent(void);
	bool anyEvent(void);

private:
	std::queue<GENevents> eventsQueue;
	int PUERTO;
	std::string MENSAJE;
	std::string LAYOUT;
};

class evPing : public genericEvent
{
public:
	evPing(int puerto): puertoAlQueLeEnviareRespuestaSiNoSoyElCollecting(puerto) {}

	int puertoAlQueLeEnviareRespuestaSiNoSoyElCollecting;
	eventTypes getType(void) { return ping; }
};


class evNetworklayout : public genericEvent
{
public:
	evNetworklayout(std::string mensaje):layouttt(mensaje){}
	
	std::string layouttt;
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