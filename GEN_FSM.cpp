#include "GEN_FSM.h"

GEN_FSM::GEN_FSM(state_n* ptr): genericFSM(&fsmTable[0][0], 4, 6, idle), statePTR(ptr)
{

};

GEN_FSM::~GEN_FSM() {

}

void GEN_FSM::setRandomTime(unsigned long int Randomtime_) {
	RandomTime = Randomtime_;
}

void GEN_FSM::RutaDefault(genericEvent* ev)
{
	
}

void GEN_FSM::Noevent_r_acc(genericEvent* ev)
{
	RandomTime > 5 ? RandomTime - 5 : 0;
	if (RandomTime)
		RandomTime--;
	if (!RandomTime)
		collect_r_acc(NULL);
		/// Largar un evento de time out
	cout << "RANDOM TIME" << RandomTime << endl;
	return;
}
void GEN_FSM::idle_r_acc(genericEvent* ev)
{
	if (!RandomTime) {
		//cambiar estado  a mandar mensaje
//		this->State = 
	}
	return;
}

void GEN_FSM::firstping_r_acc(genericEvent* ev)
{
	if (!RandomTime) {
		//cambiar estado  a mandar mensaje
	}
	return;
}

void GEN_FSM::secping_r_acc(genericEvent* ev)
{
	if (!RandomTime) {
		//cambiar estado  a mandar mensaje
	}
	return;
}

void GEN_FSM::collect_r_acc(genericEvent* ev)
{
	if (*statePTR == FREE)
	{
		//IMPORTANTE le mandamos al cliente un puntero a nuestro vector de nodos en el subconjunto para que pueda añadir nuevos
		*statePTR = CLIENT;
		json noInfo;
		noInfo.clear();		
		unsigned int port_ = selectRandomNode();
		client->setPort(port_);
		client->setIP("localhost");
		client->usePOSTmethod("/eda_coin/PING", noInfo);
		client->performRequest(); //Sólo ejecuta una vuelta de multiHandle. Para continuar usándolo se debe llamar a la función performRequest
	}
	return;
}

void GEN_FSM::sendlayout_r_acc(genericEvent* ev)
{
	

}

void GEN_FSM::setCollecting(void) {

	this->RandomTime = 0;
}

unsigned int GEN_FSM::selectRandomNode(void) {
	unsigned int randPort;
	randPort = (*portsArray)[rand() % (portsArray->size())];
	cout << "PORT" << randPort << endl; 
	return randPort;

}

