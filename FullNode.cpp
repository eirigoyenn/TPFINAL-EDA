#include "FullNode.h"



FullNode::FullNode(boost::asio::io_context& io_context_,unsigned int ID_, std::string IP_, unsigned int port_,Blockchain& bchain): io_context(io_context_) 
{
	ID = ID_;
	IP = IP_;
	port = port_;
	NodeBlockchain = bchain;
	client = new NodeClient(IP, port +1);
	server = new NodeServer(io_context_ , IP , boost::bind(&FullNode::fullCallback,this,_1), port);
}

FullNode::FullNode(boost::asio::io_context& io_context_, unsigned int ID_, std::string IP_, unsigned int port_, Blockchain& bchain, unsigned int randomTime_) : io_context(io_context_)
{
	ID = ID_;
	IP = IP_;
	port = port_;
	NodeBlockchain = bchain;
	client = new NodeClient(IP, port + 1);
	GenesisState = GenesisStates::IDLE;
	server = new NodeServer(io_context_, IP, boost::bind(&FullNode::fullCallback, this, _1), port);
	RandomTime = randomTime_;
}


FullNode::~FullNode()
{
	if (client)
		delete client;
	if (server)
		delete server;
}

void FullNode::listen1sec(void) {

	using namespace std::chrono_literals;
	auto sec = 10ms;
	io_context.run_one_for(sec);

}

GenesisStates FullNode::getGenesisState(void)
{
	return GenesisState;
}

unsigned long int FullNode::getRandomTime(void)
{
	return RandomTime;
}

void FullNode::setGenesisState(GenesisStates new_state)
{
	GenesisState = new_state; 
}

/************************************************************************************************
*					                          MENSAJES											*
*																								*
*************************************************************************************************/

//POST Block
//Recibe ID del vecino, e ID del bloque a enviar
//Genera un JSON con los datos del ID del bloque (falta terminar esa función) para luego adjuntarlo como header del mensaje Post
//Sólo configura el mensaje, la idea sería llamar a perform request (del nodo no del cliente) una vez seteado (por ahí desde el método performRequest de cada nodo)
bool FullNode::POSTBlock(unsigned int neighbourID, std::string blockId)
{
	{
		if (neighbours.find(neighbourID) != neighbours.end())
		{
			if (state == FREE)
			{
				//client = new NodeClient(IP, port+1);
				state = CLIENT;
				json block = createJSONBlock(blockId);
				unsigned int port_ = neighbours[neighbourID].port;
				client->setPort(port_);
				client->setIP(IP);
				client->usePOSTmethod("/eda_coin/send_block", block);

				client->performRequest(); //Sólo ejecuta una vuelta de multiHandle. Para continuar usándolo se debe llamar a la función performRequest

				return true;
			}
			else {
				errorType = BUSY_NODE;
				errorMessage = "Node is not available to perform as client";
				return false;
			}
		}
		else {
			errorType = NOT_NEIGHBOUR;
			errorMessage = "Requested server is not a Neighbour of current Node";
			return false;
		}
	}
}


//POST Merkleblock
//Recibe el ID del vecino
//Para terminar de ejecutar usar performRequest del nodo (NO de client!!)
bool FullNode::POSTMerkleBlock(unsigned int neighbourID, std::string BlockID_, std::string TxID)
{
	if (neighbours.find(neighbourID) != neighbours.end())
	{
		if (state == FREE)
		{
			//client = new NodeClient(IP, port+1);
			state = CLIENT;
			json jsonMerkleBlock = createJSONMerkleBlock(BlockID_, TxID);
			unsigned int port_ = neighbours[neighbourID].port;
			client->setPort(port_);
			client->setIP(IP);
			client->usePOSTmethod("/eda_coin/send_merkle_block", jsonMerkleBlock);

			this->performRequest();
			delete client;

			return true;
		}
		else {
			errorType = BUSY_NODE;
			errorMessage = "Node is not available to perform as client";
			return false;
		}
	}
	else {
		errorType = NOT_NEIGHBOUR;
		errorMessage = "Requested server is not a Neighbour of current Node";
		return false;
	}
}

bool FullNode::GETBlocks(unsigned int neighbourID, std::string blockID_, unsigned int count)
{
	if (neighbours.find(neighbourID) != neighbours.end())
	{
		if (state == FREE)
		{
			//client = new NodeClient(IP, port+1);
			state = CLIENT;
			unsigned int port_ = neighbours[neighbourID].port;
			client->setPort(port_);
			client->setIP(IP);
			client->useGETmethod("/eda_coin/get_blocks?block_id=" + blockID_ + "&count=" + to_string(count));
			client->performRequest();

			return true;

		}
		else return false;
	}
	else return false;
}

bool FullNode::makeTransaction(unsigned int neighbourID, std::string & wallet, unsigned int amount)
{
	if (neighbours.find(neighbourID) != neighbours.end())
	{
		if (state == FREE)
		{
			//client = new NodeClient(IP, port+1);
			json jsonTx;

			jsonTx["nTxin"] = 0;
			jsonTx["nTxout"] = 1;
			jsonTx["txid"] = "7E46A3BC";
			jsonTx["vin"] = json();
			json vout_;
			vout_["amount"] = amount;
			vout_["publicid"] = wallet;
			jsonTx["vout"] = vout_;

			state = CLIENT;
			unsigned int port_ = neighbours[neighbourID].port;
			client->setPort(port_);
			client->setIP(IP);
			//client->setIP(neighbours[neighbourID].IP);
			//client->setPort(neighbours[neighbourID].port);
			client->usePOSTmethod("/eda_coin/send_tx", jsonTx);

			client->performRequest();

			return true;
		}
		else {
			errorType = BUSY_NODE;
			errorMessage = "Node is not available to perform as client";
			return false;
		}
	}
	else {
		errorType = NOT_NEIGHBOUR;
		errorMessage = "Requested server is not a Neighbour of current Node";
		return false;
	}
}

/*********************************************************
*             	MENSAJES PARA EL GENSIS
*********************************************************/

bool FullNode::POSTPing(int neighbourPORT)
{
	if (state == FREE)
	{
		//IMPORTANTE le mandamos al cliente un puntero a nuestro vector de nodos en el subconjunto para que pueda añadir nuevos
		//client = new NodeClient(IP, port + 1,&(this->subconjuntoNodosRED));
		state = CLIENT;
		json noInfo = "";		//NO LE MANDO INFORMACION?
		unsigned int port_ = neighbourPORT;
		client->setPort(port_);
		client->setIP(IP);
		client->usePOSTmethod("/eda_coin/PING", noInfo);

		client->performRequest(); //Sólo ejecuta una vuelta de multiHandle. Para continuar usándolo se debe llamar a la función performRequest
		std::cout << this->subconjuntoNodosRED.size() << std::endl;

		return true;
	}
	else {
		errorType = BUSY_NODE;
		errorMessage = "Node is not available to perform as client";
		return false;
	}
}

bool FullNode::POSTNetworkLayout(int neighbourPORT)
{
	if (state == FREE)
	{
		//IMPORTANTE le mandamos al cliente un puntero a nuestro vector de nodos en el subconjunto para que pueda añadir nuevos
		//client = new NodeClient(IP, port + 1, &(this->subconjuntoNodosRED));
		state = CLIENT;
		json jsonLayout;
		
		jsonLayout["CAMPO"] = 0;

		unsigned int port_ = neighbours[neighbourPORT].port;
		client->setPort(port_);
		client->setIP(IP);
		client->usePOSTmethod("/eda_coin/NETWORK_LAYOUT", jsonLayout);

		client->performRequest(); //Sólo ejecuta una vuelta de multiHandle. Para continuar usándolo se debe llamar a la función performRequest
		std::cout << this->subconjuntoNodosRED.size() << std::endl;

		return true;
	}
	else {
		errorType = BUSY_NODE;
		errorMessage = "Node is not available to perform as client";
		return false;
	}

}


/************************************************************************************************
*					               GENERADORES DE JSON											*
*																								*
*************************************************************************************************/



//Genera un JSON del bloque de la blockchain que coincida con BlockId

json FullNode::createJSONBlock(std::string BlockId)
{
	json jsonblock;
	Block block;
	for (int i = 0; i < NodeBlockchain.getBlocksSize(); i++)
	{
		if (NodeBlockchain.getBlocksArr()[i].getBlockID() == BlockId) {
			block = NodeBlockchain.getBlocksArr()[i];
			break;
		}
	}
	jsonblock["blockid"] = block.getBlockID();
	jsonblock["height"] = block.getHeight();
	jsonblock["merkleroot"] = block.getMerkleRoot();
	jsonblock["nTx"] = block.getNtx();
	jsonblock["nonce"] = block.getNonce();
	jsonblock["previousblockid"] = block.getPrevBlovkID();

	auto tx = json::array();
	for (auto tx_ = 0; tx_ < block.getNtx(); tx_++)
	{
		tx.push_back(createJSONTx(block.getTxVector()[tx_]));
	}
	jsonblock["tx"] = tx;
	return jsonblock;
}


//En esta fase uso lo que dice la guía, creo que no hay que generar el merkle path
//Cargo con datos del primer bloque del arreglo.
//Para fases futuros hay que agregar en Block.h una función que recupere el MerklePath
//Genera el JSON de un Merkle Block.
json FullNode::createJSONMerkleBlock(std::string BlockID_, std::string TxID)
{
	json MerkleBlock;
	Block block;
	Transaction tx;
	for (int i = 0; i < NodeBlockchain.getBlocksSize(); i++)
	{
		if (NodeBlockchain.getBlocksArr()[i].getBlockID() == BlockID_) {
			block = NodeBlockchain.getBlocksArr()[i];
			break;
		}
	}
	MerkleBlock["blockid"] = block.getBlockID();
	for (int i = 0; i < block.getTxVector().size(); i++)
	{
		if (block.getTxVector()[i].txID == TxID)
		{
			tx = block.getTxVector()[i];
			break;
		}
	}
	MerkleBlock["tx"] = createJSONTx(tx);
	MerkleBlock["txPos"] = 1;
	MerkleBlock["merklePath"] = block.getMerklePath(tx);
	
	return MerkleBlock;
}

json FullNode::createJSONheader(std::string BlockID_)
{
	json jsonHeader;
	Block block;
	for (int i = 0; i < NodeBlockchain.getBlocksSize(); i++)
	{
		if (NodeBlockchain.getBlocksArr()[i].getBlockID() == BlockID_) {
			block = NodeBlockchain.getBlocksArr()[i];
			break;
		}
	}
	jsonHeader["blockid"] = block.getBlockID();
	jsonHeader["height"] = block.getHeight();
	jsonHeader["merkleroot"] = block.getMerkleRoot();
	jsonHeader["nTx"] = block.getNtx();
	jsonHeader["nonce"] = block.getNonce();
	jsonHeader["previousblockid"] = block.getPrevBlovkID();

	return jsonHeader;
}



/************************************************************************************************
*					               CALLBACK														*
*																								*
*************************************************************************************************/

json FullNode::fullCallback(string message) {

	json result;
	std::string ID_;
	unsigned int count_;

	result["status"] = true;


	if ((message.find("get_blocks") != std::string::npos) || (message.find("get_block_header") != std::string::npos))
	{
		unsigned int idPosition = message.find("block_id=");
		unsigned int countPosition = message.find("count=");
		string block_id("block_id=");
		string count("count=");

		if (idPosition != std::string::npos && countPosition != std::string::npos)
		{

			ID_ = message.substr(idPosition + block_id.size(), message.find_last_of("&") - idPosition - block_id.size());
			std::string tempcount = message.substr(countPosition + count.size(), message.size() - countPosition - count.size());
			count_ = std::stoi(tempcount);
		}
		else {
			result["status"] = false;
		}

		
		if (message.find("get_blocks") != std::string::npos)
		{
			result["result"] = find_array(ID_ , count_);
			if (result["result"] == "NULL") {
				result["status"] = false;
			}
		}

		else if ( (message.find("get_block_header")) != std::string::npos)
		{

			result["result"] = find_headers(ID_,count_);
			if (result["result"] == "NULL") {
				result["status"] = false;
			}
		}
		else {
			result["status"] = false;
		}

	}
	else if ((message.find("send_block") != std::string::npos) || (message.find("send_tx") != std::string::npos) || (message.find("send_filter") != std::string::npos)) {


		//Si se trata de un POSTblock guarda el block enviado
		if (message.find("send_block") != std::string::npos)
		{
			result["result"] = findBlockJSON(message);
			if (result["result"] == "NULL") {
				result["status"] = false;
			}
		}
		//Si se trata de un POSTtransaction
		else if (message.find("send_tx") != std::string::npos)
		{
			result["result"] = findTxJSON(message);

		}
		//Si se trata de un POSTfilter
		else if (message.find("send_filter") != std::string::npos)
		{
			result["result"] = findFilterJSON(message);// guardar los datos

		}
		else {
			result["status"] = false;
		}
	}
	/***********************
	mensajes GENESIS
	************************/
	/**********
		si me llega un ping
	***********/
	else if ((message.find("PING") != std::string::npos))
	{
		if (GenesisState == GenesisStates::IDLE) {

			result["result"] = "NETWORK_NOTREADY";

			//Mandamos ID y puerto para que nodo en estado COLLECTING lo agregue a lista de Subconjuntos de nodos
			result["blockID"] = this->getID();
			result["port"] = this->getPort();

			//Cambio estado del nodo full
			GenesisState = GenesisStates::WAITINGLAYOUT;

		}
		else if (GenesisState == GenesisStates::COLLECTING) {

			result["result"] = "NETWORK_READY";

			//Cambio estado del nodo full
			GenesisState = GenesisStates::NETCREATED;
		}
		else if (GenesisState == GenesisStates::WAITINGLAYOUT)
		{
			result["result"] = "NETWORK_READY";
			
			//
			//ACA GUARDAR AL NODO Q ENVIO EL MENSAJE Y AGREGARLO COMO VECINO
			/////////////////////////
			///////////////////////////
			//////////////////////////////
			////////////////////////////////
		}
	}

	/**********
	si me llega el network layout
	***********/
	else if ((message.find("NETWORK_LAYOUT") != std::string::npos))
	{
		if (GenesisState == GenesisStates::WAITINGLAYOUT) {
			result["result"] = "NETWORK_READY";

			//Cambio estado del nodo full
			GenesisState = GenesisStates::NETCREATED;

			//ACA GUARDAR INFO DE LOS VECINOS QUE ESTA EN NETWORK LAYOUT
		}
	}

	/**********
	si me llega el network ready
	***********/
	else if ((message.find("NETWORK_READY") != std::string::npos))
	{
		if (GenesisState == GenesisStates::COLLECTING) {
			result["result"] = "NETWORKREADY";

			//Cambio estado del nodo full
			GenesisState = GenesisStates::NETCREATED;

			//ACA GUARDAR INFO DE LOS VECINOS QUE ESTA EN NETWORK LAYOUT

			//AlgoritmoParticular();
		}
	}
	else if ((message.find("NETWORK_NOTREADY") != std::string::npos))
	{
		//??? ACA NO HAGO NADA
	}

	else {
		result["status"] = false;
	}

	return result;


}



json FullNode::find_array(std::string blockID, int count) {

	auto jsonarray = json::array();
	Block block;
	int pointer =  NOTFOUND;
	int numBlocks = count;
	for (int i = 0; i < NodeBlockchain.getBlocksSize(); i++)
	{
		if (NodeBlockchain.getBlocksArr()[i].getBlockID() == blockID) {
			block = NodeBlockchain.getBlocksArr()[i];
			pointer = i;
			break;
		}
	}

	if ((NodeBlockchain.getBlocksSize() - block.getHeight()) < count) {
		numBlocks = NodeBlockchain.getBlocksSize() - block.getHeight();
	}

	if (pointer != NOTFOUND) {
		for (int i = 0;i < numBlocks;i++, pointer++) {
			jsonarray.push_back(createJSONBlock(NodeBlockchain.getBlocksArr()[pointer].getBlockID()));
		}
	}
	else {
		jsonarray = "NULL";
	}
	return jsonarray;
}


json FullNode::find_headers(std::string blockID, int count) {

	auto jsonarray = json::array();
	json jsonblock;
	Block block;
	int pointer = NOTFOUND;
	int numBlocks = count;
	cout << blockID << endl;

	for (int i = 0; i < NodeBlockchain.getBlocksSize(); i++)
	{
		if (NodeBlockchain.getBlocksArr()[i].getBlockID() == blockID) {
			block = NodeBlockchain.getBlocksArr()[i];
			pointer = i;
			break;
		}

	}

	if ((NodeBlockchain.getBlocksSize() - block.getHeight()) < count) {
		numBlocks = NodeBlockchain.getBlocksSize() - block.getHeight();
	}

	if (pointer != NOTFOUND) {
		for (int i = 0;i < numBlocks;i++, pointer++) {

			jsonblock["blockid"] = NodeBlockchain.getBlocksArr()[pointer].getBlockID();
			jsonblock["height"] = NodeBlockchain.getBlocksArr()[pointer].getHeight();
			jsonblock["merkleroot"] = NodeBlockchain.getBlocksArr()[pointer].getMerkleRoot();
			jsonblock["nTx"] = NodeBlockchain.getBlocksArr()[pointer].getNtx();
			jsonblock["nonce"] = NodeBlockchain.getBlocksArr()[pointer].getNonce();
			jsonblock["previousblockid"] = NodeBlockchain.getBlocksArr()[pointer].getPrevBlovkID();

			jsonarray.push_back(jsonblock);

		}
	}
	else {
		return "NULL";
	}
	return jsonarray;

}



json FullNode::findBlockJSON(std::string message) {

	cout << "MENSAJEE " << message  << endl;
	json blockJSON = json::parse(parseResponse(message)); 
	
	Block block(blockJSON);
	if (block.createMerkleTree()) {
		return blockJSON;
	}
	else {
		return "NULL";
	}

}

json FullNode::findTxJSON(std::string message) {


	json TxJSON = json::parse(parseResponse(message));

	return "NULL";
}


json FullNode::findFilterJSON(std::string message) {


	json filterJSON = json::parse(parseResponse(message));

	return "NULL";
}

void FullNode::particularAlgorithm(void)
{
	

	bool isConnectedToNode = false;; //para loopear en la red
	int nextNode;

	json layout; //Layout de la red
	json nodes; //Nodos de la red
	json edges;	//Aristas de la red

	if ((*PTR2Subconjunto).size())	//Cargo los nodos con ID:puerto (no se me ocurre otra forma).
	{
		for (int i = 0; i < (*PTR2Subconjunto).size(); i++) {
			std::string nodeInfo = (*PTR2Subconjunto)[i].TEMP_ID + ":" + std::to_string((*PTR2Subconjunto)[i].TEMP_PUERTO);
			nodes.push_back(nodeInfo);
		}
	}
	layout["nodes"] = nodes;

	if ((*PTR2Subconjunto).size() >= 2) //Necesito más de dos elementos para formar la red
	{	
		int j;
		for (int i=0; i < (*PTR2Subconjunto).size(); i++)
		{
			if ((*PTR2Subconjunto)[i].numberofConnections >= 2) //Si ya tiene más de dos conexiones no hace falta seguir agregando.
			{
				cout << (*PTR2Subconjunto)[i].numberofConnections << endl;
			}

			//Caso contrario sigue el agloritmo
			else
			{
				while ((*PTR2Subconjunto)[i].numberofConnections < 2)
				{
					int stillGoing = 1;
					while (stillGoing)
					{
						nextNode = (rand() % ((*PTR2Subconjunto).size())); //Busco un aleatorio para conextarme
						for (j = 0; j < (*PTR2Subconjunto)[i].connections.size() && stillGoing; j++) //
						{
							if ((*PTR2Subconjunto)[i].connections[j] == nextNode) {
								stillGoing = 0;
								isConnectedToNode = true;
							}
						}
						 
							
					}
					if (j != i && isConnectedToNode != true) {		//Si se logró conectar con otro o toco aleatorio el mismo nodo, busca otro.

						//Agregar JSONS
						edges.clear();
						std::string Node1Info = (*PTR2Subconjunto)[i].TEMP_ID + ":" + std::to_string((*PTR2Subconjunto)[i].TEMP_PUERTO);
						std::string Node2Info = (*PTR2Subconjunto)[nextNode].TEMP_ID + ":" + std::to_string((*PTR2Subconjunto)[j].TEMP_PUERTO);
						edges.push_back({ { "target1", Node1Info }, { "target2", Node2Info } });
						layout["edges"] = edges;

						(*PTR2Subconjunto)[i].connections.push_back(j); //Los agrego como conectados
						(*PTR2Subconjunto)[j].connections.push_back(i);
						(*PTR2Subconjunto)[i].numberofConnections++;
						(*PTR2Subconjunto)[j].numberofConnections++;
					}
				}
			}
		}
	}

	else //Caso contrario, armo el layout con los dos nodos presentes, no hace falta BFS ni DFS puesto que ya es conexo.
	{	
		std::string Node1Info= PTR2Subconjunto[0]->TEMP_ID + ":" + std::to_string(PTR2Subconjunto[0]->TEMP_PUERTO);
		std::string Node2Info = PTR2Subconjunto[1]->TEMP_ID + ":" + std::to_string(PTR2Subconjunto[1]->TEMP_PUERTO);
		edges.push_back({ { "target1", Node1Info }, { "target2", Node2Info } });
		layout["edges"] = edges;
	}
}

bool FullNode::isConvex(void)
{
	BFS(0);
	for (int i = 0; i < (*PTR2Subconjunto).size(); i++)	//Busco en todos los nodos a ver si hay alguno no visitado
	{
		if (!((*PTR2Subconjunto)[i].checked))
			return false;
	}
	
	return true;
}

void FullNode::BFS(int nodeToVisit)
{
	if (!((*PTR2Subconjunto)[nodeToVisit].checked)) //Si todavía no se llegó a este nodo, lo marca como visitado y visita a todos sus vecinos
	{
		(*PTR2Subconjunto)[nodeToVisit].checked = true;
		for (int i = 0; i < (*PTR2Subconjunto)[nodeToVisit].connections.size(); i++)
			BFS(i);
	}
}



int FullNode::selectRandomNode2Add(std::vector<FullNode*>& fullarrayy)
{
	int randomNum = rand() % fullarrayy.size();

	for (; esteIndiceNOT_OK(randomNum) ; )		//Si esteIndiceOK devuelve true hay que buscar otro
		randomNum = rand() % fullarrayy.size();

	//Salimos del for cuando ya tenemos el random indice
//	this->subconjuntoNodosRED.push_back(randomNum);		//Lo guardamos en arreglo de indices
	return randomNum;
}

bool FullNode::esteIndiceNOT_OK(int randID)
{
	bool result = false;
	if (randID == this->getID())		//ID es igual al indice
		result = true;

	return result;

}
