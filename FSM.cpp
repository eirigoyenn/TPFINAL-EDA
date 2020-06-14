
#include "FSM.h"





FSM::~FSM()
{
	for (auto& node : spvArray) {
		if (node)
			delete node;
	}
	for (auto& node : fullArray) {
		if (node)
			delete node;
	}
}

void FSM::RutaDefault(genericEvent* ev)
{
	return;
}

void FSM::BuscarVecinos_r_acc(genericEvent* ev)
{
	if (static_cast<evBuscarVecinos*>(ev)->getType() == BuscarVecinos)
	{
		this->state4Graphic = LOOK4VECI_G;
	}
}

std::vector<SPVNode*>* FSM::getSPVArrayPTR(void)
{
	return &spvArray;
}

std::vector<FullNode*>* FSM::getFULLArrayPTR(void)
{
	return &fullArray;
}

void FSM::CrearNodo_r_acc(genericEvent* ev)
{
	if (static_cast<evCrearNodo*>(ev)->getType() == CrearNodo)
	{
		

		string input2file;

		if (static_cast<evCrearNodo*>(ev)->TYPE == SPV)
		{
			SPVNode * TempSPVNode = new SPVNode(io_context,static_cast<evCrearNodo*>(ev)->ID, static_cast<evCrearNodo*>(ev)->IP, static_cast<evCrearNodo*>(ev)->PUERTO);
			
			spvArray.push_back(TempSPVNode);
			input2file = "Se creo un nodo\nIP:" + static_cast<evCrearNodo*>(ev)->IP + " - ID: " + to_string(static_cast<evCrearNodo*>(ev)->ID) + " - TYPE: SPV \nPUERTO:" + to_string(static_cast<evCrearNodo*>(ev)->PUERTO) + "\n\n";
			Add2JSONfile(false, static_cast<evCrearNodo*>(ev)->PUERTO);

		}
		if (static_cast<evCrearNodo*>(ev)->TYPE == FULL)
		{
			FullNode * tempFullNode = new FullNode(io_context,static_cast<evCrearNodo*>(ev)->ID, static_cast<evCrearNodo*>(ev)->IP, static_cast<evCrearNodo*>(ev)->PUERTO,Bchain);
			
			fullArray.push_back(tempFullNode);

			input2file = "Se creo un nodo\nIP:" + static_cast<evCrearNodo*>(ev)->IP + " - ID: " + to_string(static_cast<evCrearNodo*>(ev)->ID) + " - TYPE: FULL\n" + "PUERTO:" + to_string(static_cast<evCrearNodo*>(ev)->PUERTO) + "\n\n";
			Add2JSONfile(true, static_cast<evCrearNodo*>(ev)->PUERTO);
		}

		RegistroNodo_t tempReg(static_cast<evCrearNodo*>(ev)->ID, static_cast<evCrearNodo*>(ev)->IP, static_cast<evCrearNodo*>(ev)->PUERTO, static_cast<evCrearNodo*>(ev)->TYPE);
			/***** ACA MANDAMOS UPDATE A BULLETIN   ******/

		*static_cast<evCrearNodo*>(ev)->nameofFile += input2file; 

		/***** ACTUALIZAMOS ARREGLO DE NODOS CREADOS EN ESTA INSTANCIA ******/
		RegistroNodo_t tempNewNodo;
		tempNewNodo.ID = static_cast<evCrearNodo*>(ev)->ID;
		tempNewNodo.TYPE = static_cast<evCrearNodo*>(ev)->TYPE;
		tempNewNodo.PUERTO = static_cast<evCrearNodo*>(ev)->PUERTO;

		static_cast<evCrearNodo*>(ev)->NodoArray->push_back(tempNewNodo);

	}
}

void FSM::Add2JSONfile(bool isFullNode, int puerto_)
{
	//En InputRed tengo el json ya parsead
	if(isFullNode)
		InputRED["full-nodes"] += puerto_;
	else
		InputRED["spv-nodes"] += puerto_;

	std::ofstream input_file("redOrigen.json");
	input_file << InputRED;
}


void FSM::MultiiPerform(genericEvent* ev)
{
	if (static_cast<evMulti*>(ev)->getType() == NoEvent)
	{

		for (const auto& spvnode : spvArray) {
			spvnode->listen1sec();
			spvnode->performRequest();
		}

		for (const auto& fullnode : fullArray) {
			fullnode->listen1sec();
			fullnode->performRequest();
		}

		
	}

}

unsigned int FSM::getIndex(unsigned int senderID, nodeTypes nodeType)
{
	int index = -1;

	if (nodeType == FULL)
	{
		for (int i = 0; i < fullArray.size() && index == -1; i++)
		{
			if (fullArray[i]->getID() == senderID)
				index = i;
		}
		return index;
	}
	else
	{
		for (int i = 0; i < spvArray.size() && index == -1; i++)
		{
			if (spvArray[i]->getID() == senderID)
				index = i;
		}
		return index;
	}

}

unsigned int FSM::getneighbourIDfromPort(unsigned int neighbourPORT, nodeTypes nodetype)
{
	int neighbourID = -1;
	if (nodetype == FULL)
	{
		for (int i = 0; i < fullArray.size() && neighbourID == -1; i++)
		{
			if (fullArray[i]->getPort() == neighbourPORT)
				neighbourID = fullArray[i]->getID();
		}
		return neighbourID;
	}
	else
	{
		for (int i = 0; i < spvArray.size() && neighbourID == -1; i++)
		{
			if (spvArray[i]->getPort() == neighbourPORT)
				neighbourID = spvArray[i]->getID();
		}
		return neighbourID;
	}
}

void FSM::VolverADashboard_r_acc(genericEvent* ev)
{
	if (static_cast<evBack2Dashboard*>(ev)->getType() == Back2Dashboard)
	{
		cout << " VOLVER A DASHBOARD " << endl;
		this->state4Graphic = DASHBOARD_G;
	}
}

void FSM::EnviarMensaje_r_acc(genericEvent* ev)
{
	if (static_cast<evEnviarMsj*>(ev)->getType() == EnviarMsj)
	{
		this->state4Graphic = DASHBOARD_G;

		Neighbour NodoReceptor = static_cast<evEnviarMsj*>(ev)->Comunication.VECINO;
		/*******************
		*  enviar filter  *
		********************/
		if ((static_cast<evEnviarMsj*>(ev)->Comunication.MENSAJE) == FILTER_Genv)
		{
			//Recupero el ID del vecino y el del sender
			int neighbourID = static_cast<evEnviarMsj*>(ev)->Comunication.VECINO.ID;
			//Si se tiene el puerto del vecino pero no su ID puede hacerse int neighbourID=getneighbourIDfromPort(unsigned int neighbourPORT, nodeTypes neighbourType);
			unsigned int senderID = static_cast<evEnviarMsj*>(ev)->Comunication.NodoEmisor.ID;
			//Busco el índice en el arreglo de nodos SPV (sólo SPV pueden enviar mensajes tipo Filter).
			unsigned int senderIndex = getIndex(senderID, SPV);
			//Recupero la publickey del nodo y configuro para enviar el mensaje.
			spvArray[senderIndex]->POSTFilter(neighbourID, spvArray[senderIndex]->getKey());

		}

		/************************
		*  enviar get blocks   *
		************************/
		else if ((static_cast<evEnviarMsj*>(ev)->Comunication.MENSAJE) == GETBLOCKS_Genv)
		{
			//Recupero el ID del vecino y el del sender
			int neighbourID = static_cast<evEnviarMsj*>(ev)->Comunication.VECINO.ID;
			unsigned int senderID = static_cast<evEnviarMsj*>(ev)->Comunication.NodoEmisor.ID;
			//Busco el índice del nodo en el arreglo (sólo nodos full usan envían este mensaje)
			unsigned int senderIndex = getIndex(senderID, FULL);
			//Recupero valores de count y blockID (en esta fase no importan)
			//unsigned int count = 1;
			//std::string blockID = "75FF25E0";
			//Configuro el mensaje
			fullArray[senderIndex]->GETBlocks(neighbourID, static_cast<evEnviarMsj*>(ev)->Comunication.BLOCK_ID, static_cast<evEnviarMsj*>(ev)->Comunication.CANT_BLOCKS);

		}

		/*****************************
		* get blocks headers enviar  *
		******************************/
		else if ((static_cast<evEnviarMsj*>(ev)->Comunication.MENSAJE) == GETBLOCKHEADERS_Genv)
		{
			//Recupero el ID del vecino y el del sender
			int neighbourID = static_cast<evEnviarMsj*>(ev)->Comunication.VECINO.ID;
			unsigned int senderID = static_cast<evEnviarMsj*>(ev)->Comunication.NodoEmisor.ID;
			//Busco el índice del nodo en el arreglo (sólo nodos spv envían este mensaje)
			unsigned int senderIndex = getIndex(senderID, SPV);
			//Recupero valores de count y blockID (en esta fase no importan)
			//unsigned int count = 1;
			//std::string blockID = "75FF25E0";
			//Configuro el mensaje
			spvArray[senderIndex]->GETBlockHeader(neighbourID, static_cast<evEnviarMsj*>(ev)->Comunication.BLOCK_ID, static_cast<evEnviarMsj*>(ev)->Comunication.CANT_BLOCKS);
		}

		/*********************
		*    POST Block enviar    *
		**********************/
		else if ((static_cast<evEnviarMsj*>(ev)->Comunication.MENSAJE) == BLOCK_Genv)
		{
			//Recupero el ID del vecino y el del sender
			int neighbourID = static_cast<evEnviarMsj*>(ev)->Comunication.VECINO.ID;
			unsigned int senderID = static_cast<evEnviarMsj*>(ev)->Comunication.NodoEmisor.ID;
			//Busco el índice del nodo en el arreglo (sólo nodos full envían este mensaje)
			unsigned int senderIndex = getIndex(senderID, FULL);
			//Recupero valor de BlockID (en esta fase no importa)
			//std::string BlockID="75FF25E0";
			fullArray[senderIndex]->POSTBlock(neighbourID, static_cast<evEnviarMsj*>(ev)->Comunication.BLOCK_ID );
		}

		/******************************
		*  POST merekle block enviar   *
		*******************************/
		else if ((static_cast<evEnviarMsj*>(ev)->Comunication.MENSAJE) == MERKLEBLOCK_Genv)
		{
			//Recupero el ID del vecino y el del sender
			int neighbourID = static_cast<evEnviarMsj*>(ev)->Comunication.VECINO.ID;
			unsigned int senderID = static_cast<evEnviarMsj*>(ev)->Comunication.NodoEmisor.ID;
			//Busco el índice del nodo en el arreglo (sólo nodos full envían este mensaje)
			unsigned int senderIndex = getIndex(senderID, FULL);
			//Recupero valores de BlockID y TxId (en esta fase no importan)
			//std::string TxID_="7B857A14"
			//std::string blockID = "75FF25E0";
			//Configuro el mensaje
			fullArray[senderIndex]->POSTMerkleBlock(neighbourID, static_cast<evEnviarMsj*>(ev)->Comunication.BLOCK_ID, static_cast<evEnviarMsj*>(ev)->Comunication.nTX_);

		}



		/******************************
		*    transaction enviar       *
		*******************************/
		else if ((static_cast<evEnviarMsj*>(ev)->Comunication.MENSAJE) == TRANSACTION_Genv)
		{
			//Recupero el tipo de nodo emsior
			nodeTypes type = (nodeTypes)static_cast<evEnviarMsj*>(ev)->Comunication.NodoEmisor.TYPE;
			//Recupero el ID del vecino y el del sender
			int neighbourID = static_cast<evEnviarMsj*>(ev)->Comunication.VECINO.ID;
			unsigned int senderID = static_cast<evEnviarMsj*>(ev)->Comunication.NodoEmisor.ID;
			//Busco el índice del nodo en el arreglo
			if (type == FULL)
				unsigned int senderIndex = getIndex(senderID, FULL);
			else
				unsigned int senderIndex = getIndex(senderID, SPV);
			//Recupero el monto a enviar y la wallet a donde enviar y configuro el mensaje

			Transaction jsonTx;

			jsonTx.nTxin = 0;
			jsonTx.nTxout= 1;
			jsonTx.txID= "7E46A3BC"; //generica dummy

			VinS vin;
			vin.LilblockID = "0";
			vin.outputIndex = 0;
			vin.signature = "0";
			vin.txID = "0";

			jsonTx.vIn.push_back(vin);

			VoutS vout;
			vout.amount = static_cast<evEnviarMsj*>(ev)->Comunication.COINS_G;
			vout.publicID = static_cast<evEnviarMsj*>(ev)->Comunication.PublicKey_G;

			jsonTx.vOut.push_back(vout);

			if (type == FULL)
			{
				fullArray[getIndex(senderID, FULL)]->POSTTransaction(neighbourID,jsonTx);
			}
			else
			{
				spvArray[getIndex(senderID, SPV)]->POSTTransaction(neighbourID, jsonTx);
			}
		}


		/***** ACA MANDAMOS UPDATE A BULLETIN   ******/
		string input2file;
		input2file = "Se envio un mensaje\n Emisor\n    PUERTO:: " + to_string(static_cast<evEnviarMsj*>(ev)->Comunication.NodoEmisor.PUERTO) + "\n Receptor\n    PUERTO: " + to_string(static_cast<evEnviarMsj*>(ev)->Comunication.VECINO.port) + "\n\n";

		*static_cast<evEnviarMsj*>(ev)->nameofFile += input2file;
	}
}

void FSM::CrearConexion_r_acc(genericEvent* ev)
{

	if (static_cast<evCrearConexion*>(ev)->getType() == CrearConexion)
	{


		for (int i = 0; i < fullArray.size() ; i++)
		{
			if (fullArray[i]->getPort() == (static_cast<evCrearConexion*>(ev)->Nodo1.PUERTO))
			{
				fullArray[i]->addNeighbour((static_cast<evCrearConexion*>(ev)->Nodo2.ID), (static_cast<evCrearConexion*>(ev)->Nodo2.IP), (static_cast<evCrearConexion*>(ev)->Nodo2.PUERTO));
			}
		}
		for (int i = 0; i < spvArray.size(); i++)
		{
			if (spvArray[i]->getPort() == (static_cast<evCrearConexion*>(ev)->Nodo1.PUERTO))
			{
				spvArray[i]->addNeighbour((static_cast<evCrearConexion*>(ev)->Nodo2.ID), (static_cast<evCrearConexion*>(ev)->Nodo2.IP), (static_cast<evCrearConexion*>(ev)->Nodo2.PUERTO));

			}
		}
		

		/***** ACA MANDAMOS UPDATE A BULLETIN   ******/
		string input2file;
		input2file = "Conexion creada entre\n Nodo con Puerto: " + to_string(static_cast<evCrearConexion*>(ev)->Nodo1.PUERTO) + "\n Nodo con Puerto:" + to_string(static_cast<evCrearConexion*>(ev)->Nodo2.PUERTO) + "\n\n";
		*static_cast<evCrearConexion*>(ev)->nameofFile += input2file;

	}
}

void FSM::ShwNodos_r_acc(genericEvent* ev)
{
	if (static_cast<evBuscarVecinos*>(ev)->getType() == MostrarNodos)
	{
		this->state4Graphic = SHWNODOS_G;
	}
}

void FSM::BlockSelected_r_acc(genericEvent* ev)
{
	if (static_cast<evBlockSelected*>(ev)->getType() == BlockSelected)
	{
		this->state4Graphic = SHWSELB_G;
	}
}

void FSM::ErrorEncontrado_r_acc(genericEvent* ev)
{
	if (static_cast<evBuscarVecinos*>(ev)->getType() == Error)
	{
		cout << " ERROR ENCONTRADO " << endl;
		this->state4Graphic = SHWERROR_G;
	}
}

void FSM::Start_genesis_r_acc(genericEvent* ev)
{

	//Seed for random timeout
	srand((unsigned)time(NULL));


	if (static_cast<evBuscarVecinos*>(ev)->getType() == BuscarVecinos)			//Usamos evento mostrar vecinos para no tener q crear evento nuevo 
	{
		this->state4Graphic = GENESIS_G;

		

		string GenesisPath = static_cast<evBuscarVecinos*>(ev)->JSONPath;
		fs::path bPath(GenesisPath.c_str());
		if (exists(bPath) && is_directory(bPath))
		{
			for (fs::directory_iterator iterator(bPath); iterator != fs::directory_iterator(); iterator++)
			{
				if (iterator->path().filename().string() == "redOrigen.json")
				{
					std::ifstream RED(iterator->path().filename().string().c_str(), std::ifstream::binary);
					if (RED)
					{
						json RED_JDATA = json::parse(RED);
						this->InputRED = RED_JDATA; 

						/* CREATING FULL NODES */
						auto FULLNODEPORT = RED_JDATA["full-nodes"];
						int i = 0;
						for (const auto& FULL : FULLNODEPORT)
						{
							FullNode* tempFullNode = new FullNode(io_context, i++, "localhost", FULL, Bchain, makeRandomTime() );
							auto pointer = new GEN_FSM();
							tempFullNode->setGENFSM(pointer);          //INICIAS LA GEN_FSM de cada uno
							tempFullNode->attach();
							fullArray.push_back(tempFullNode);
						}

						/* CREATING SPV NODES */
						auto SPVNODEPORT = RED_JDATA["spv-nodes"];
						for (const auto& SPV : SPVNODEPORT)
						{
							SPVNode* tempSpvNode = new SPVNode(io_context, i++, "localhost", SPV);
							spvArray.push_back(tempSpvNode);
						}

						//Asi cuando se creen nodos en modo apendice se empiezan a crear a partir de los ya existentes
						*(static_cast<evBuscarVecinos*>(ev)->nodeIDPTR) = i;
					}
				}
			}
			
		}
	}			
}


unsigned int FSM::makeRandomTime(void)
{
	return ((rand() % 10000) + 10);
}

void FSM::RutaDefaultInitState(genericEvent* ev)
{
	if (static_cast<evMulti*>(ev)->getType() == NoEvent)			//Usamos evento mostrar nodos para no tener q crear evento nuevo 
	{
	
	}
}


void FSM::Start_app_r_acc(genericEvent* ev)
{
	if (static_cast<evMostrarNodos*>(ev)->getType() == MostrarNodos)			
	{
		this->state4Graphic = DASHBOARD_G;
	}
}










void FSM::finish_r_acc(genericEvent* ev)
{
		int count = 0;
		for (auto it : fullArray) {
			if (it->isnetworkcreated()) {
				count++;
			}
		}
		if (count == fullArray.size()) {
			//cambiar estado a dashboard again 
		}
}

void FSM::cycle_each_r_acc(genericEvent* ev)
{
	int count;
	for (auto it : fullArray) {
		(it)->my_cycle();
	}
}




Blockchain& FSM::getBchain() {
	return Bchain;
}