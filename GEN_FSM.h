#pragma once


#include "GenericFSM.h"
#include "GenEventGenerator.h"
#include "Node.h"

#define TR(x) (static_cast<void (genericFSM::* )(genericEvent *)>(&GEN_FSM::x)) 

enum states:stateTypes {idle,waitinglayout,collecting,netcreated, sendinglayout };

class GEN_FSM : public genericFSM
{
public:
	GEN_FSM(state_n* stateptr, int port); 
	~GEN_FSM();
	stateTypes getState(void) { return state; }
	void setRandomTime(unsigned long int Randomtime);
	unsigned int selectRandomNode(void) ;
	void setCollecting(void);
	void setClient(NodeClient* cliente_) { client = cliente_; };
	void setServer(NodeServer* server_) { server = server_; };
	void setportArray(std::vector<unsigned int>* portsArray_) { portsArray = portsArray_; };
	void setState(states estadonuevo);

private:
	const fsmCell fsmTable[5][7] = {
		//     EVENTOS					PING								    NETWORK NOT READY					NETWORK READY						NETWORK CREATED						NOEVENT										TIMER									LAYOUT
		/*estados*/
		/*IDLE*/						{{waitinglayout,TR(firstping_r_acc)},	{idle,TR(RutaDefault)},				{idle,TR(RutaDefault)},					{idle,TR(RutaDefault)},				{idle,TR(Noevent_r_acc)},				{collecting,TR(collect_r_acc)} ,		{idle,TR(RutaDefault)} },

		/*WAITING LAYOUT*/				{{waitinglayout,TR(secping_r_acc)},		{waitinglayout,TR(RutaDefault)},	{waitinglayout,TR(RutaDefault)},		{waitinglayout,TR(RutaDefault)},	{waitinglayout,TR(Noevent_r_acc)},		{waitinglayout,TR(RutaDefault)},		{netcreated,TR(tengo_layout_r_acc)} },

		/*COLLECTING NET MEMBERS*/		{{collecting,TR(collect_r_acc)},		{collecting,TR(collect_r_acc)},		{sendinglayout,TR(sendlayout_r_acc)},	{collecting,TR(RutaDefault)},		{collecting,TR(Noevent_r_acc)},			{collecting,TR(RutaDefault)},			{collecting,TR(collect_r_acc)} },

		/*NET CREATED*/					{{netcreated,TR(RutaDefault)},			{netcreated,TR(RutaDefault)},		{netcreated,TR(RutaDefault)},			{netcreated,TR(RutaDefault)},		{netcreated,TR(RutaDefault)},			{netcreated,TR(RutaDefault)},			{collecting,TR(collect_r_acc)} },

		/*SENDING LAYOUT*/				{{sendinglayout,TR(RutaDefault)},		{sendinglayout,TR(RutaDefault)},	{sendinglayout,TR(RutaDefault)},		{sendinglayout,TR(RutaDefault)},	{sendinglayout,TR(Noevent_r_acc)},	{sendinglayout,TR(sendlayout_r_acc)} ,	{collecting,TR(collect_r_acc)}},
	};

	void RutaDefault(genericEvent* ev);
	void Noevent_r_acc(genericEvent* ev);
	void firstping_r_acc(genericEvent* ev);
	void secping_r_acc(genericEvent* ev);
	void collect_r_acc(genericEvent* ev);
	void sendlayout_r_acc(genericEvent* ev);
	void idle_r_acc(genericEvent* ev);
	void tengo_layout_r_acc(genericEvent* ev);

//	states State;
	unsigned long int RandomTime;
	NodeClient* client=nullptr;
	NodeServer* server = nullptr;
	std::vector<unsigned int>* portsArray;
	state_n* statePTR;
	int NodoDelSubconjuntoQueLeVoyAEnviarElLayout;
	int myid;
	
};
