#include "FullNode.h"


FullNode::FullNode(boost::asio::io_context& io_context_,unsigned int ID_, std::string IP_, unsigned int port_,Blockchain& bchain): io_context(io_context_)
{
	ID = ID_;
	IP = IP_;
	publicKey= std::to_string(std::rand() % 99999999);
	port = port_;
	NodeBlockchain = bchain;
	client = new NodeClient(IP, port +1, &subconjuntoNodosRED, ID_);
	server = new NodeServer(io_context_ , IP , boost::bind(&FullNode::fullCallback,this,_1), port);
}

FullNode::FullNode(boost::asio::io_context& io_context_, unsigned int ID_, std::string IP_, unsigned int port_, Blockchain& bchain, unsigned int randomTime_) : io_context(io_context_)
{
	ID = ID_;
	IP = IP_;
	publicKey = std::to_string(std::rand() % 99999999);
	port = port_;
	NodeBlockchain = bchain;
	client = new NodeClient(IP, port + 1, &subconjuntoNodosRED, ID_);
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
	auto sec = 1ms;
	io_context.run_one_for(sec);

}

void FullNode::setTotalFullNodes(int total)
{
	this->client->cantNodes = total;
}

GenesisStates FullNode::getGenesisState(void)
{
	return this->client->GenesisState;
}

unsigned long int FullNode::getRandomTime(void)
{
	return RandomTime;
}

void FullNode::setGenesisState(GenesisStates new_state)
{
	this->client->GenesisState = new_state; 
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
			state = CLIENT;
			json jsonMerkleBlock = createJSONMerkleBlock(BlockID_, TxID);
			unsigned int port_ = neighbours[neighbourID].port;
			client->setPort(port_);
			client->setIP(IP);
			client->usePOSTmethod("/eda_coin/send_merkle_block", jsonMerkleBlock);

			this->performRequest();
//			delete client;

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
		state = CLIENT;
		json noInfo;
		noInfo["NO INFO"] = "NO INFO";		//NO LE MANDO INFORMACION?
		unsigned int port_ = neighbourPORT;
		client->setPort(port_);
		client->setIP(IP);
		client->usePOSTmethod("/eda_coin/PING", noInfo);

		client->performRequest(); //Sólo ejecuta una vuelta de multiHandle. Para continuar usándolo se debe llamar a la función performRequest

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
	
		state = CLIENT;
		json jsonLayout = client->getJSONlayout();
		cout << jsonLayout << endl;

		client->setPort(neighbourPORT);
		client->setIP(IP);
		client->usePOSTmethod("/eda_coin/NETWORK_LAYOUT",jsonLayout);

		client->performRequest(); //Sólo ejecuta una vuelta de multiHandle. Para continuar usándolo se debe llamar a la función performRequest

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
		if (this->client->GenesisState == GenesisStates::IDLE) {

			result["result"] = "NETWORK_NOTREADY";

			//Mandamos ID y puerto para que nodo en estado COLLECTING lo agregue a lista de Subconjuntos de nodos
			result["blockID"] = this->getID();
			result["port"] = this->getPort();

			//Cambio estado del nodo full
			this->client->GenesisState = GenesisStates::WAITINGLAYOUT;

		}
		else if (this->client->GenesisState == GenesisStates::WAITINGLAYOUT)
		{
			result["result"] = "NETWORK_READY";
			
			//
			//ACA GUARDAR AL NODO Q ENVIO EL MENSAJE Y AGREGARLO COMO VECINO
			/////////////////////////
			///////////////////////////
			//////////////////////////////
			////////////////////////////////
		}
		else {
			result["result"] = "false";
		}

	}

	/**********
	si me llega el network layout
	***********/
	else if ((message.find("NETWORK_LAYOUT") != std::string::npos))
	{
		if (this->client->GenesisState == GenesisStates::WAITINGLAYOUT) {
			result["result"] = "NET_CREATED";

			//Cambio estado del nodo full
			this->client->GenesisState = GenesisStates::NETCREATED;
			//ACA GUARDAR INFO DE LOS VECINOS QUE ESTA EN NETWORK LAYOUT
			MeGuardoAMisVecinos(message);
		}
		else {
			result["result"] = "false";
		}
	}
	else {
		result["status"] = false;
	}
	return result;
}


void FullNode::MeGuardoAMisVecinos(std::string reply)
{
	auto it = reply.find("\r\n\r\n");
	std::string crlf("\r\n\r\n");
	std::string response;
	response = reply.substr(it + crlf.size(), reply.size() - (it + crlf.size()));

	json LAYOUT = json::parse(response);
	std::cout << std::endl << std::endl << "LAYOUT:" << LAYOUT << std::endl << std::endl << std::endl;

	std::string soyyo = std::to_string(this->ID) + ":" + std::to_string(this->port - 1);

	for (auto& edge : LAYOUT["edges"]) {
		std::string target1 = edge["target1"];
		std::string target2 = edge["target2"];
		std::string target;

		cout << "SOY YO" << soyyo << endl << "TARGET 1"<< target1 << endl << "TARGET2" << target2 << endl;

		if (target1 == soyyo)
			target = target2;
		else if (target2 == soyyo)
			target = target1;

		if (target.length()) {
			int pos1 = target.find_first_of(':');
			std::string temp = target.substr(pos1 + 1, target.length() - pos1 - 1);
			std::string id = target.substr(0, pos1);
			int id_ = std::stoi(id);
			int pos2 = temp.find_first_of(':');
			int port_ = std::stoi(temp.substr(pos2 + 1, temp.length() - pos2 - 1));

			addNeighbour(id_, IP, port_);		
		}
	}
}

bool FullNode::validateTx(const json & Tx_)
{
	bool res = true;
	int i = 0;
	int inCoins = 0;
	int outCoins = 0;
	std::vector <usedID> usedIDs; //txid y outputindex de cada vin para comparar con las UTXO

	//Recupero los txid y los output index de cada transacción
	for (const auto& vin_ : Tx_["vin"])
	{
		usedIDs[i].txid = vin_["txid"].get<std::string>();
		usedIDs[i].outputIndex = vin_["outputIndex"].get<unsigned int>();
		i++;
	}

	//Verifico que todas las transacciones correspodan a UTXOs distintos.
	//Si se encuentra el UTXO a que corresponde se borra como disponible.
	for (int j = 0; j < usedIDs.size(); j++)
	{
		if (compareWithUTXO(usedIDs[j].txid, usedIDs[j].outputIndex))
		{
			//Si se encuentra el txid en la lista de Utxos disponibles, se recupera la cantidad de monedas disponibles
			//según indica el UTXO (inCoins) y se quita al UTXO como válido.
			inCoins += UTXOS[UtxoIndex].amount;
			UTXOS.erase(UTXOS.begin() + UtxoIndex);
		}
		else
		{
			res = false;
			cout << "Tx has no input listed in UTXO." << endl;
		}
	}

	//Verifico que coincidan la cantidad de monedas entrantes y salientes.

	for (const auto& vout_ : Tx_["vout"]) {
		outCoins += vout_["amount"].get<unsigned int>();
	}

	if (inCoins != outCoins) {
		res = false;
		cout << "Coins amount does not match up." << endl;
	}

	return res;

}

bool FullNode::validateBlock(const json & Block_)
{
	bool res = true;
	bool checkedMinerTransaction = false;
	unsigned int height = NodeBlockchain.getBlocksSize();

	//Reviso que se trate del último bloque en la blockchain (que fue subido por el minero). La única excepción es
	//cuando se trata del primer bloque (ie height==0).
	if (height)
	{
		if (Block_["blockid"] != NodeBlockchain.getBlocksArr()[height - 1].getBlockID())
		{
			res = false;
			cout << "Block height not valid." << endl;
		}
	}

	//Verifico que el merkleroot sea correcto
	if (Block_["merkleroot"] != NodeBlockchain.createMerkleRoot(Block_))
	{
		res = false;
		cout << "merkleroot not valid." << endl;
	}

	//Verifico que el blockid corresponda con el que se obtiene por hasheo del bloque.
	if (Block_["blockid"] != NodeBlockchain.generateBlockId(Block_))
	{
		res = false;
		cout << "blockid does not match up with the hashed one." << endl;
	}

	//Verifico cada una de las transacciones (considerando que hay una única transacción sin vin que es la del minero).
	for (auto& tx_ : Block_["tx"])
	{
		if (tx_["vin"].is_null() && checkedMinerTransaction==false)
		{
			checkedMinerTransaction = true;
		}
		else
		{
			if (!validateTx(tx_))
			{
				res = false;
				cout << "transaction not valid." << endl;
			}
		}
	}

	return res;
}

void FullNode::newUTXOs(const json & newTx)
{
	std::vector <UTXO> newUTXOS;

	newUTXOS.clear();
	Transaction Tx;
	auto nTxin_ = newTx["nTxin"].get<unsigned int>();
	Tx.nTxin = nTxin_;
	auto nTxout_ = newTx["nTxout"].get<unsigned int>();
	Tx.nTxout = nTxout_;
	auto txID_ = newTx["txid"].get<std::string>();
	Tx.txID = txID_;
	auto Vin_ = newTx["vin"];
	for (auto& vin_ : Vin_)
	{
		VinS tempVin;

		auto LilBlockId_ = vin_["blockid"].get<std::string>();
		tempVin.LilblockID = LilBlockId_;
		auto outputIndex_ = vin_["outputIndex"].get<int>();
		tempVin.outputIndex = outputIndex_;
		auto signature_ = vin_["signature"].get<std::string>();
		tempVin.signature = signature_;
		auto txid_ = vin_["txid"].get<std::string>();
		tempVin.txID = txid_;

		Tx.vIn.push_back(tempVin);
	}
	auto Vout_ = newTx["vout"];
	for (auto& vout_ : Vout_)
	{
		VoutS tempVout;

		auto amount_ = vout_["amount"].get<unsigned int>();
		tempVout.amount = amount_;
		auto publicID_ = vout_["publicid"].get<std::string>();
		tempVout.publicID = publicID_;

		Tx.vOut.push_back(tempVout);
	}

	for (int i = 0; i < Tx.nTxin; i++)
	{
		UTXO newUTXO;
		newUTXO.txid = Tx.vIn[i].txID;
		newUTXO.outputIndex = Tx.vIn[i].outputIndex;
		newUTXOS.push_back(newUTXO);
	}
	
	for (int j = 0; j < newUTXOS.size(); j++)
	{
		unsigned int outputIndex_ = newUTXOS[j].outputIndex;
		newUTXOS[j].amount = Tx.vOut[outputIndex_+1].amount;
		newUTXOS[j].publicid = Tx.vOut[outputIndex_ + 1].publicID;
		UTXOS.push_back(newUTXOS[j]);
	}

}

bool FullNode::compareWithUTXO(std::string txid_, unsigned int outputIndex_)
{
	bool result=false;

	for (int i = 0; i < UTXOS.size(); i++)
	{
		if (UTXOS[i].txid == txid_ && UTXOS[i].outputIndex == outputIndex_)
		{
			UtxoIndex = i;
			result = true;
		}
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




int FullNode::selectRandomNode2Add(std::vector<FullNode*>& fullarrayy)
{
	int randomNum = rand() % fullarrayy.size();

	for (; esteIndiceNOT_OK(randomNum) ; )		//Si esteIndiceOK devuelve true hay que buscar otro
		randomNum = rand() % fullarrayy.size();

	//Salimos del for cuando ya tenemos el random indice
	if (randomNum == fullarrayy.size())
		randomNum -= 1;

	return randomNum;
}

bool FullNode::esteIndiceNOT_OK(int randID)
{
	bool result = false;
	if (randID == this->getID())		//ID es igual al indice
		result = true;
	return result;

}
