#pragma once
#include "GenericFSM.h"
#include "GenEventGenerator.h"

#define TX(x) (static_cast<void (genericFSM::* )(genericEvent *)>(&FSM::x)) 



class GEN_FSM: public genericFSM
{
public:
	GEN_FSM() : genericFSM(&fsmTable[0][0], 7, 11, ) {};
	~GEN_FSM();

private:
	const fsmCell fsmTable[4][4] = {
		//     EVENTOS				PING					NETWORK NOT READY					NETWORK READY			NETWORK CREATED
	/*estados*/					
		/*IDLE*/					{{,TX(RutaDefault)},		{,TX(RutaDefault)},				{,TX(RutaDefault)},		{,TX(RutaDefault)}, },
	/*WAITING LAYOUT*/				{{,TX(RutaDefault)},		{,TX(RutaDefault)},				{,TX(RutaDefault)},		{,TX(RutaDefault)}, },
	/*COLLECTING NET MEMBERS*/		{{,TX(RutaDefault)},		{,TX(RutaDefault)},				{,TX(RutaDefault)},		{,TX(RutaDefault)}, },
	/*NET CREATED*/					{{,TX(RutaDefault)},		{,TX(RutaDefault)},				{,TX(RutaDefault)},		{,TX(RutaDefault)}, },
};

