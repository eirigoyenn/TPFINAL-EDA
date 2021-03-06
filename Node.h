#pragma once
#include "Blockchain.h"
#include "NodeClient.h"
#include "NodeServer.h"
#include <chrono>
#include <map>
#include "Structs.h"
#include <chrono>


typedef enum {FREE,CLIENT,SERVER} state_n;
typedef enum {ERROR_FREE, CLIENT_ERROR, SERVER_ERROR, BUSY_NODE, NOT_NEIGHBOUR} errorType_n;
typedef enum {POSTBLOCK, POSTTRANSACTION, POSTMERKLE, POSTFILTER, GETBLOCKS, GETHEADER} connection__;
typedef enum {SPV, FULL} nodeTypes;

//struct MerkleBlock {
//	std::string BlockId;
//	vector<Transaction> TxVector_;
//	unsigned int TxPosition;
//	vector<std::string> merklePath;
//};

class Node
{
public:
	Node();
	~Node();
	//Funci�n para agregar vecinos

	bool addNeighbour(int ID_,std::string& IP_, int port_);
	
	bool addNeighbour2(int ID_, std::string IP_, int port_);

	//Funciones para enviar mensajes.
	bool POSTTransaction(unsigned int neighbourID, Transaction Tx_);

	virtual std::string makeDaytimeString(int secs);
	std::string parseResponse(std::string message);
	void setIP(std::string IP_);
	void setPort(unsigned int port_);
	void setID(unsigned int ID_);
	void setState(state_n state_);
	void setErrorType(errorType_n errorType_);
	void setErrorMessage(std::string errorMessage_);
	std::string getIP(void);
	unsigned int getPort(void);
	unsigned int getID(void);
	state_n getState(void);
	errorType_n getErrorType(void);
	std::string getErrorMessage(void);
	std::map <unsigned int, Neighbour> getNeighbours(void);
	std::vector <std::string> getFilters(void);
	
	std::string errReply(void);
	//Funciones para generar los JSON de los mensajes
	json createJSONTx(Transaction Tx_);
	
	bool GperformRequest(GenesisStates* bbb);	
	bool performRequest(void);

	string createAddress(string ip,int port);

protected:

	//void TxCallback(string respuesta);
	std::string IP;
	std::string publicKey;
	unsigned int port;
	unsigned int ID;
	int sentMessage;
	int receivedMessage;

	state_n state;

	NodeClient* client;
	NodeServer* server;
	errorType_n errorType;
	std::string errorMessage;
	/*std::vector <Neighbour> neighbours;*/
	std::map <unsigned int, Neighbour> neighbours;
	std::vector <std::string> FilterArray;
	//std::vector <Transaction> TxVector;
	//std::vector <MerkleBlock> MerkleBlockVector;

};

