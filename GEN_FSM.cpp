#include "GEN_FSM.h"

GEN_FSM::GEN_FSM(state_n* ptr, int myid_): genericFSM(&fsmTable[0][0], 5, 7, idle), statePTR(ptr)
{
	NodoDelSubconjuntoQueLeVoyAEnviarElLayout = 0;
	myid = myid_;
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
	if (RandomTime)
		RandomTime--;

	if (!RandomTime)
	{
		this->state = states::collecting;
		collect_r_acc(NULL);
	}
	return;
}
void GEN_FSM::idle_r_acc(genericEvent* ev)
{
	if (!RandomTime) 
	{

	}
	return;
}

void GEN_FSM::thirdping_r_acc(genericEvent* ev)
{
	//IMPORTANTE le mandamos al cliente un puntero a nuestro vector de nodos en el subconjunto para que pueda añadir nuevos
	*statePTR = CLIENT;
	json Info;
	Info.clear();
	Info["PORT"] = this->client->getPort();
	client->setPort(static_cast<evPing*>(ev)->puertoAlQueLeEnviareRespuestaSiNoSoyElCollecting);
	client->setIP("localhost");
	client->usePOSTmethod("/eda_coin/NETWORK_READY_2", Info);
	client->performRequest(); //Sólo ejecuta una vuelta de multiHandle. Para continuar usándolo se debe llamar a la función performRequest

}

void GEN_FSM::firstping_r_acc(genericEvent* ev)
{
	if (static_cast<evPing*>(ev)->getType() == ping)			
	{
		if (!RandomTime && (*statePTR == FREE)) {
			std::cout << std::endl << std::endl << "firstoing_r_acc \nLAYOUT:" << std::endl << std::endl << std::endl;
			/* Guardo al nodo que me envio el ping como futuro vecino al terminar el genesis */
			//Neighbour tempNei;
			//tempNei.port = static_cast<evPing*>(ev)->puertoAlQueLeEnviareRespuestaSiNoSoyElCollecting;
			//tempNei.IP = "localhost";
			//neighbourtsPARAelNodoFullDespsDelGenesisPTR->push_back(tempNei); 
	
			//IMPORTANTE le mandamos al cliente un puntero a nuestro vector de nodos en el subconjunto para que pueda añadir nuevos
			*statePTR = CLIENT;
			json Info;
			Info.clear();
			Info["PORT"] = this->client->getPort();
			client->setPort(static_cast<evPing*>(ev)->puertoAlQueLeEnviareRespuestaSiNoSoyElCollecting);
			client->setIP("localhost");
			client->usePOSTmethod("/eda_coin/NETWORK_NOTREADY", Info);
			client->performRequest(); //Sólo ejecuta una vuelta de multiHandle. Para continuar usándolo se debe llamar a la función performRequest
		}
	}
	return;
}

void GEN_FSM::secping_r_acc(genericEvent* ev)
{
	if (static_cast<evPing*>(ev)->getType() == ping)
	{
		if (*statePTR == FREE)
		{
			std::cout << std::endl << std::endl << "secping_r_acc \nLAYOUT:"<< std::endl << std::endl << std::endl;

			//IMPORTANTE le mandamos al cliente un puntero a nuestro vector de nodos en el subconjunto para que pueda añadir nuevos
			*statePTR = CLIENT;
			json response;
			response.clear();

			client->setPort(static_cast<evPing*>(ev)->puertoAlQueLeEnviareRespuestaSiNoSoyElCollecting);
			client->setIP("localhost");
			client->usePOSTmethod("/eda_coin/NETWORK_READY", response);
			client->performRequest(); //Sólo ejecuta una vuelta de multiHandle. Para continuar usándolo se debe llamar a la función performRequest
		}
	}


	return;
}

void GEN_FSM::tengo_layout_r_acc(genericEvent* ev)
{
	if (static_cast<evNetworklayout*>(ev)->getType() == networklayout)
	{

		string reply = static_cast<evNetworklayout*>(ev)->layouttt;

		auto it = reply.find("\r\n\r\n");
		std::string crlf("\r\n\r\n");
		std::string response;
		response = reply.substr(it + crlf.size(), reply.size() - (it + crlf.size()));

		json LAYOUT = json::parse(response);

		std::cout << std::endl << std::endl << "Tengo_layout_r_acc \nLAYOUT:" << LAYOUT << std::endl << std::endl << std::endl;
		int MIPUERTO = (this->client->getOwnPort()) - 1;
		std::string soyyo = std::to_string(MIPUERTO);

		for (auto& edge : LAYOUT["edges"]) {
			std::string target1 = edge["target1"];
			std::string target2 = edge["target2"];
			std::string target;

			if (target1 == soyyo)
				target = target2;
			else if (target2 == soyyo)
				target = target1;

			if (target.length()) {
				int pos1 = target.find_first_of(':');
				std::string temp = target.substr(pos1 + 1, target.length() - pos1 - 1);
				int pos2 = temp.find_first_of(':');
				int port_ = std::stoi(temp.substr(pos2 + 1, temp.length() - pos2 - 1));

				std::cout << std::endl << std::endl << std::endl << "PORT" << port_ <<std::endl << std::endl;

				//Agrego al nodo como vecino
				if (port_ > 0)
				{
					Neighbour tempNei;
					tempNei.port = port_;
					tempNei.IP = "localhost";
					neighbourtsPARAelNodoFullDespsDelGenesisPTR->push_back(tempNei);

				}
			}
		}

	}
}

void GEN_FSM::collect_r_acc(genericEvent* ev)
{
	if (*statePTR == FREE)
	{
		std::cout << std::endl << std::endl << "collect_r_acc \nLAYOUT:" << std::endl << std::endl << std::endl;

		//IMPORTANTE le mandamos al cliente un puntero a nuestro vector de nodos en el subconjunto para que pueda añadir nuevos
		*statePTR = CLIENT;
		json response;
		response.clear();	
		int PUERTOO = (int) this->client->getOwnPort() - 1;  
		//Mandamos puerto para que nodo en estado IDLE O WAITING LAYOUT le responda
		std::string mensajeDeCollecting = "/eda_coin/PING/PUERTO:" + to_string(PUERTOO) + "/" ;

		unsigned int port_ = selectRandomNode();
		client->setPort(port_);
		client->setIP("localhost");
		client->usePOSTmethod(mensajeDeCollecting.c_str(), response);
		client->performRequest(); //Sólo ejecuta una vuelta de multiHandle. Para continuar usándolo se debe llamar a la función performRequest
	}
	return;
}



void GEN_FSM::sendlayout_r_acc(genericEvent* ev)
{
	if ((client->Subconjunto->size() >= (NodoDelSubconjuntoQueLeVoyAEnviarElLayout+1)) && (client->Subconjunto->size() != 0))
	{
		if (*statePTR == FREE)
		{
			std::cout << std::endl << std::endl << "\nsendlayout_r_acc \nLAYOUT:" << std::endl << std::endl << std::endl;

			*statePTR = CLIENT;
			json Layout;
			Layout = client->LAYOUT2SEND;
			int port_ = (*client->Subconjunto)[NodoDelSubconjuntoQueLeVoyAEnviarElLayout].TEMP_PUERTO;

			client->setPort(port_);
			client->setIP("localhost");
			client->usePOSTmethod("/eda_coin/NETWORK_LAYOUT", Layout);
			client->performRequest(); //Sólo ejecuta una vuelta de multiHandle. Para continuar usándolo se debe llamar a la función performRequest
			NodoDelSubconjuntoQueLeVoyAEnviarElLayout++;

			/* Ademas me lo guardo como vecino*/
			Neighbour tempNei;
			tempNei.port = port_;
			tempNei.IP = "localhost";
			neighbourtsPARAelNodoFullDespsDelGenesisPTR->push_back(tempNei);
		}
	}
	else
	{
		this->state = states::netcreated;
	}
}

void GEN_FSM::setCollecting(void) {

	this->state = states::collecting;
}

unsigned int GEN_FSM::selectRandomNode(void) {
	unsigned int randPort;
	randPort = (*portsArray)[rand() % (portsArray->size())];
	//Asi no nos auto mandamos un ping 
	for (; randPort == this->client->getPort() ; )
		randPort = (*portsArray)[rand() % (portsArray->size())];
	return randPort;
}

void GEN_FSM::setState(states estadonuevo)
{
	this->state = estadonuevo;
}


