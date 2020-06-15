#pragma once


#include "Node.h"
#include "EventHandling.h"
#include "GenEventGenerator.h"
#include "GEN_FSM.h"

#define NOTFOUND -1




class FullNode :
	public Node
{
public:

	FullNode(boost::asio::io_context& io_context_, unsigned int ID_, std::string IP_, unsigned int port_, Blockchain& bchain);


	~FullNode();

	json fullCallback(std::string msg);
	void listen1sec(void);
	//Funciones para enviar mensajes.
	bool POSTBlock(unsigned int neighbourID, std::string BlockID);
	bool POSTMerkleBlock(unsigned int neighbourID, std::string BlockID_, std::string TxID);
	bool GETBlocks(unsigned int neighbourID, std::string blockID_, unsigned int count);

	//Genesis
	unsigned long int getRandomTime(void);
	int selectRandomNode2Add(std::vector<FullNode*>& fullarrayy);
	bool esteIndiceNOT_OK(int randID);

	std::vector<NodoSubconjunto> subconjuntoNodosRED;		//Vector de los INDICES de los nodos full que logro unir a su red

	//MENSAJES de genesis
//	bool POSTPing(int neighbourPORT);
	//bool POSTNetworkLayout(int neighbourPORT);
	//bool POSTNetwork_NotReady(unsigned int neighbourID);
	//bool POSTNetwork_Ready(unsigned int neighbourID);

	void MeGuardoAMisVecinos(std::string reply);

//Funciones para generar los JSON de los mensajes
	json createJSONBlock(std::string BlockId);
	json createJSONMerkleBlock(std::string BlockID_, std::string TxID_);
	json createJSONheader(std::string BlockID_);

	// funciones para responder a los GET msg

	json find_array(std::string ID_, int count);
	json find_headers(std::string ID_, int count);


	// funciones para responder a los POSTS

	json findBlockJSON(std::string message);
	json findTxJSON(std::string message);
	json findFilterJSON(std::string message);

	void setGENFSM(GEN_FSM* pointer);
	GEN_FSM* getGENFSM(void) { return GenFSM; }
	void attach(void);
	void my_cycle(void);
	bool isnetworkcreated(void);
	void setFSMtimer(unsigned int time_) { GenFSM->setRandomTime(time_); }
	void setFSMCandS() {
		GenFSM->setClient(client);
		GenFSM->setServer(server);
	}
	void setFMSPortsArray(std::vector<unsigned int>* portsArray) {
		GenFSM->setportArray(portsArray);
	}

	void setFSMNodetoCollecting(void)
	{
		GenFSM->setCollecting();
	}
	
private:
	boost::asio::io_context& io_context;
	Blockchain NodeBlockchain;
	std::vector <std::string> filters;

	mainEventGenerator eventGen;	//agarra los gen de eventos (hago el general aca pq tiene las funciones de )
	GEN_FSM* GenFSM=nullptr;
	GenEventGenerator genEvents;

};

