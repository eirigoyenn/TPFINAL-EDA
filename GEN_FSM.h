#pragma once
#include "GenericFSM.h"
#include "GenEventGenerator.h"

#define TX(x) (static_cast<void (genericFSM::* )(genericEvent *)>(&GEN_FSM::x)) 

enum states:eventTypes {idle,waitinglayout,collecting,netcreated };

class GEN_FSM : public genericFSM
{
public:
	GEN_FSM() : genericFSM(&fsmTable[0][0], 4, 6, idle) {};
	~GEN_FSM();
	eventTypes getState(void) { return State; }
private:
	const fsmCell fsmTable[4][6] = {
		//     EVENTOS					PING								NETWORK NOT READY							NETWORK READY						NETWORK CREATED						NOEVENT								TIMER
	/*estados*/
		/*IDLE*/						{{idle,TX(RutaDefault)},			{idle,TX(RutaDefault)},						{idle,TX(RutaDefault)},				{idle,TX(RutaDefault)},				{idle,TX(Noevent_r_acc)},				{idle,TX(RutaDefault)} },
		/*WAITING LAYOUT*/				{{waitinglayout,TX(RutaDefault)},	{waitinglayout,TX(RutaDefault)},			{waitinglayout,TX(RutaDefault)},	{waitinglayout,TX(RutaDefault)},	{waitinglayout,TX(Noevent_r_acc)},	{waitinglayout,TX(RutaDefault)} },
		/*COLLECTING NET MEMBERS*/		{{collecting,TX(RutaDefault)},		{collecting,TX(RutaDefault)},				{collecting,TX(RutaDefault)},		{collecting,TX(RutaDefault)},		{collecting,TX(Noevent_r_acc)},		{collecting,TX(RutaDefault)} },
		/*NET CREATED*/					{{netcreated,TX(RutaDefault)},		{netcreated,TX(RutaDefault)},				{netcreated,TX(RutaDefault)},		{netcreated,TX(RutaDefault)},		{netcreated,TX(Noevent_r_acc)},		{netcreated,TX(RutaDefault)} },

	};
	void RutaDefault(genericEvent* ev);
	void Noevent_r_acc(genericEvent* ev);
\
	states State;
};

