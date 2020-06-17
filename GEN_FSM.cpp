#include "GEN_FSM.h"

GEN_FSM::GEN_FSM(state_n* ptr, int myport_): genericFSM(&fsmTable[0][0], 4, 6, idle), statePTR(ptr)
{
	NodoDelSubconjuntoQueLeVoyAEnviarElLayout = 0;
	myport = myport_;
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
	if (!RandomTime && (this->getState() == idle))
		collect_r_acc(NULL);
		/// Largar un evento de time out
	return;
}
void GEN_FSM::idle_r_acc(genericEvent* ev)
{
	if (!RandomTime) {

	}
	return;
}

void GEN_FSM::firstping_r_acc(genericEvent* ev)
{
	if (!RandomTime) {

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
	if (client->Subconjunto->size() != NodoDelSubconjuntoQueLeVoyAEnviarElLayout)
	{
		*statePTR = CLIENT;
		json Layout;
		Layout = client->LAYOUT2SEND;
		int port_ = (*client->Subconjunto)[NodoDelSubconjuntoQueLeVoyAEnviarElLayout].TEMP_PUERTO;

		cout << port_ << endl;

		client->setPort(port_);
		client->setIP("localhost");
		client->usePOSTmethod("/eda_coin/NETWORK_LAYOUT", Layout);
		client->performRequest(); //Sólo ejecuta una vuelta de multiHandle. Para continuar usándolo se debe llamar a la función performRequest
		NodoDelSubconjuntoQueLeVoyAEnviarElLayout++;
	}
	else
		this->state = netcreated;

}

void GEN_FSM::setCollecting(void) {

	this->RandomTime = 0;
}

unsigned int GEN_FSM::selectRandomNode(void) {
	unsigned int randPort;
	randPort = (*portsArray)[rand() % (portsArray->size())];

	//Asi no nos auto mandamos un ping 
	for (; randPort == this->myport ; )
		randPort = (*portsArray)[rand() % (portsArray->size())];

//	cout << "PORT" << randPort << endl; 
	return randPort;
}




