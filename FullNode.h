#pragma once
#include "Node.h"

#define NOTFOUND -1

struct UTXO
{
	std::string txid;
	unsigned int outputIndex;
	unsigned int amount;
	std::string publicid;
};

struct usedID
{
	std::string txid;
	unsigned int outputIndex;
};


class FullNode :
	public Node
{
public:
	FullNode();
	FullNode(boost::asio::io_context& io_context_,unsigned int ID_, std::string IP_ ,unsigned int port_,Blockchain& bchain);
	
	FullNode(boost::asio::io_context& io_context_,unsigned int ID_, std::string IP_, unsigned int port_, Blockchain& bchain, unsigned int randomTime);

	~FullNode();

	json fullCallback(std::string msg);
	void listen1sec(void);
	//Funciones para enviar mensajes.
	bool POSTBlock(unsigned int neighbourID, std::string BlockID);
	bool POSTMerkleBlock(unsigned int neighbourID, std::string BlockID_, std::string TxID);
	bool GETBlocks(unsigned int neighbourID, std::string blockID_, unsigned int count);
	bool makeTransaction(unsigned int neighbourID, std::string& wallet, unsigned int amount);
	
	//Genesis
	GenesisStates getGenesisState(void);
	GenesisStates* getGenesisStateAddress(void);

	void setGenesisState(GenesisStates new_state);
	unsigned long int getRandomTime(void);
	int selectRandomNode2Add(std::vector<FullNode*>& fullarrayy);
	bool esteIndiceNOT_OK(int randID);

	std::vector<NodoSubconjunto> subconjuntoNodosRED;		//Vector de los INDICES de los nodos full que logro unir a su red

	//MENSAJES de genesis
	bool POSTPing(int neighbourPORT);
	bool POSTNetworkLayout(int neighbourPORT);
		//bool POSTNetwork_NotReady(unsigned int neighbourID);
		//bool POSTNetwork_Ready(unsigned int neighbourID);


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

	void setTotalFullNodes(int total);
	void AddVecinosFromAlgoritmo(void);
	void MeGuardoAMisVecinos(std::string reply);

	//Funciones de verificación
	bool validateTx(const json& Tx_);
	bool validateBlock(const json& Block_);
	void newUTXOs(const json& newTx);
	bool compareWithUTXO(std::string txid_, unsigned int outputIndex_);


protected:
	std::vector<Neighbour> VecinosdeAlgoritmo;

	boost::asio::io_context& io_context;
	Blockchain NodeBlockchain;
	std::vector <std::string> filters;
	unsigned long int RandomTime; 
	std::queue<GenesisEvents> EventQueueGenesis;
	int nodes2Add;
	int UtxoIndex;
	json transactions_;
	std::vector <UTXO> UTXOS;

};

