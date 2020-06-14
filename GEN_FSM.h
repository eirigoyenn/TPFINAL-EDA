#pragma once


#include "GenericFSM.h"
#include "GenEventGenerator.h"
#include "FullNode.h"

#define TR(x) (static_cast<void (genericFSM::* )(genericEvent *)>(&GEN_FSM::x)) 

enum states:eventTypes {idle,waitinglayout,collecting,netcreated };

class GEN_FSM : public genericFSM
{
public:
	GEN_FSM() : genericFSM(&fsmTable[0][0], 4, 6, idle) {};
	~GEN_FSM();
	eventTypes getState(void) { return State; }
	void setRandomTime(unsigned long int Randomtime);
	void setFullArr(std::vector<FullNode*>* fullArray_) { fullArray = fullArray_; };
	void setClient(NodeClient* cliente_) { client = cliente_; };
	void setServer(NodeServer* server_) { server = server_; };
private:
	const fsmCell fsmTable[4][6] = {
		//     EVENTOS					PING								NETWORK NOT READY							NETWORK READY						NETWORK CREATED						NOEVENT										TIMER
	/*estados*/
		/*IDLE*/						{{waitinglayout,TR(firstping_r_acc)},			{idle,TR(RutaDefault)},						{idle,TR(RutaDefault)},				{idle,TR(RutaDefault)},				{idle,TR(Noevent_r_acc)},				{collecting,TR(collect_r_acc)} },
		/*WAITING LAYOUT*/				{{waitinglayout,TR(secping_r_acc)},	{waitinglayout,TR(RutaDefault)},			{waitinglayout,TR(RutaDefault)},	{waitinglayout,TR(RutaDefault)},	{waitinglayout,TR(Noevent_r_acc)},		{waitinglayout,TR(RutaDefault)} },
		/*COLLECTING NET MEMBERS*/		{{netcreated,TR(sendlayout_r_acc)},		{collecting,TR(collect_r_acc)},				{netcreated,TR(sendlayout_r_acc)},		{collecting,TR(RutaDefault)},		{collecting,TR(Noevent_r_acc)},			{collecting,TR(RutaDefault)} },
		/*NET CREATED*/					{{netcreated,TR(RutaDefault)},		{netcreated,TR(RutaDefault)},				{netcreated,TR(RutaDefault)},		{netcreated,TR(RutaDefault)},		{netcreated,TR(Noevent_r_acc)},			{netcreated,TR(RutaDefault)} },

	};
	void RutaDefault(genericEvent* ev);
	void Noevent_r_acc(genericEvent* ev);
	void firstping_r_acc(genericEvent* ev);
	void secping_r_acc(genericEvent* ev);
	void collect_r_acc(genericEvent* ev);
	void sendlayout_r_acc(genericEvent* ev);
	void startcollecting_r_acc(genericEvent* ev);
	void idle_r_acc(genericEvent* ev);

	states State;
	std::vector<FullNode*>* fullArray;
	unsigned long int RandomTime;
	NodeClient* client=nullptr;
	NodeServer* server = nullptr;
};
