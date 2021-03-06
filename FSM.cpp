
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
		/*******
			uint TYPE;
			int PUERTO;
			string IP;
			std::vector<RegistroNodo_t>* NodoArray;
			int nodeID;
			std::string * nameofFile;
		*******/

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
		if (this->state4Graphic == DASHBOARD_G)
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

		/** ESTAMOS IMPRIMIENDO GENESIS **/
		else if (this->state4Graphic == GENESIS_G)
		{
			/*
				unsigned long int timeoutVar;
			*/
			for (const auto& fullnode : fullArray)
			{
				fullnode->listen1sec();
				fullnode->performRequest();

	
				unsigned long int TIME = (static_cast<evMulti*>(ev)->timeoutVar) / 10;
				/* RECORRO ESTADOS DE NODOS FULL */
				int ID2Ping, ID2Connect;
				for (auto& node : fullArray)
				{
					switch (node->getGenesisState())
					{
					case GenesisStates::IDLE:
						if (node->getRandomTime() == TIME)		// x = i* 10  --> i = x / 10
							node->setGenesisState(GenesisStates::COLLECTING);
						cout << " ALGUIENN EN IDLE " << endl;

						break;

					case GenesisStates::WAITINGLAYOUT:
						/*
						* SI RECIBE NetworkLayout -> responde 200 OK + guarda info
						* SI RECIBE Ping -> responde NetworkReady agrega a nodo emisor como vecino
						*/
						cout << " ALGUIEN WAITING LAYOUT " << endl;

						break;

					case GenesisStates::COLLECTING:
						cout << " ALGUIEN COLLECTING " << endl;

						ID2Ping = node->selectRandomNode2Add(fullArray);
						/* FUNCION DONDE SE LE MANDA UN PING A ESE ID: */
						fullArray[node->getID()]->POSTPing(fullArray[ID2Ping]->getPort());
						break;

					case GenesisStates::SENDINGLAYOUT:
						cout << " ALGUIEN SENDING LAYOUT " << endl;
						int j;
						j = rand() % node->subconjuntoNodosRED.size();
						
						node->POSTNetworkLayout(node->subconjuntoNodosRED[j].TEMP_PUERTO);

						/* ESTO NO FUNCA
						int j;
						j = node->selectRandomNode2Add(fullArray);						
						node->POSTNetworkLayout(node->subconjuntoNodosRED[j].TEMP_PUERTO);		*/

						break;
						
					case GenesisStates::NETCREATED:
						cout << "LLEGAMOS A NETCREATED " << endl;

						break;

					default:
						break;
					}
				}
				cout << " >>>>>>> CYCLE <<<<<<<" << endl;
				if (isNetworkReady())
				{
					for (const auto& spvs : spvArray)
					{
						while ((sizeof(spvs->getNeighbours())) < 2)
						{
							ID2Connect = rand() % fullArray.size();

							for (; ID2Connect == spvs->getID(); )		//Si esteIndiceOK devuelve true hay que buscar otro
								ID2Connect = rand() % fullArray.size();

							spvs->addNeighbour2(spvArray[ID2Connect]->getID(), spvArray[ID2Connect]->getIP(), spvArray[ID2Connect]->getPort());
						}	
					}
					this->state4Graphic = DASHBOARD_G;			//Ahora imprimimos esto pero en realidad ya estabamos en el estado ShowingDashboard 
				}
			}
		}
	}
}


void FSM::isThereAnIDLEturnit2COLLECTING()
{
	int i;
	for (i=0; i< fullArray.size() ; i++)
	{
		if (fullArray[i]->getGenesisState() == GenesisStates::IDLE)
		{
			fullArray[i]->setGenesisState(GenesisStates::COLLECTING);
			i = fullArray.size();
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
		/****************
		typedef struct
		{
			RegistroNodo_t NodoEmisor;
			std::map<unsigned int, Neighbour> NodosVecinos;
			std::string mensaje;
			std::string vecinos;	//Esto se usa para la funcion combo de ImGui
			int selectedVecino;

		} ParticipantesMsj_t;

		ParticipantesMsj_t Comunication;
		
		std::vector<RegistroNodo_t>* NodoArray;
		*****************/
		cout << " Veamos si la info que le llega a r_acc esta bien:\n "<< endl;
		cout << "\nSU PUERTO:  " + to_string(static_cast<evEnviarMsj*>(ev)->Comunication.VECINO.port) << endl;

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
			//Busco el �ndice en el arreglo de nodos SPV (s�lo SPV pueden enviar mensajes tipo Filter).
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
			//Busco el �ndice del nodo en el arreglo (s�lo nodos full usan env�an este mensaje)
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
			//Busco el �ndice del nodo en el arreglo (s�lo nodos spv env�an este mensaje)
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
			//Busco el �ndice del nodo en el arreglo (s�lo nodos full env�an este mensaje)
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
			//Busco el �ndice del nodo en el arreglo (s�lo nodos full env�an este mensaje)
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
			//Busco el �ndice del nodo en el arreglo
			if (type == FULL)
				unsigned int senderIndex = getIndex(senderID, FULL);
			else
				unsigned int senderIndex = getIndex(senderID, SPV);
			//Recupero el monto a enviar y la wallet a donde enviar y configuro el mensaje
			if (type == FULL)
			{
				fullArray[getIndex(senderID, FULL)]->makeTransaction(neighbourID, static_cast<evEnviarMsj*>(ev)->Comunication.PublicKey_G, static_cast<evEnviarMsj*>(ev)->Comunication.COINS_G);
			}
			else
			{
				spvArray[getIndex(senderID, SPV)]->makeTransaction(neighbourID, static_cast<evEnviarMsj*>(ev)->Comunication.PublicKey_G, static_cast<evEnviarMsj*>(ev)->Comunication.COINS_G);
			}
		}


		/******************************
		*    transaction recibir       *
		*******************************/
		//else if ((static_cast<evEnviarMsj*>(ev)->Comunication.MENSAJE) == TRANSACTION_Grec)
		//{
		//	//Recupero el tipo de nodo emisor
		//	nodeTypes type = (nodeTypes)static_cast<evEnviarMsj*>(ev)->Comunication.selectedVecino.TYPE;
		//	//Recupero el ID del vecino y el del sender
		//	int neighbourID = static_cast<evEnviarMsj*>(ev)->Comunication.selectedVecino;
		//	unsigned int senderID = static_cast<evEnviarMsj*>(ev)->Comunication.NodoEmisor.ID;
		//	//Busco el �ndice del nodo en el arreglo
		//	if (type == FULL)
		//		unsigned int senderIndex = getIndex(senderID, FULL);
		//	else
		//		unsigned int senderIndex = getIndex(senderID, SPV);
		//	//Recupero el monto a enviar y la wallet a donde enviar y configuro el mensaje
		//	if (type == FULL)
		//		fullArray[getIndex(senderID, FULL)]->makeTransaction(neighbourID, static_cast<evEnviarMsj*>(ev)->Comunication.PublicKey_G, static_cast<evEnviarMsj*>(ev)->Comunication.COINS_G);
		//	else
		//		spvArray[getIndex(senderID, SPV)]->makeTransaction(neighbourID, static_cast<evEnviarMsj*>(ev)->Comunication.PublicKey_G, static_cast<evEnviarMsj*>(ev)->Comunication.COINS_G);
		//}


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
		/*			
		RegistroNodo_t Nodo1;
		RegistroNodo_t Nodo2;
		std::vector<RegistroNodo_t>* NodoArrayC;
		string* nameofFile;		
		*/

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
	/*
		string JSONPath;
		int * node_id;   ---> PUNTERO AL ID DE LA CLASE GUIEVENTGENERATOR
	*/

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
				selectRandomFullNode(fullArray.size() - 1);
		}
	}
	//this->state4Graphic = DASHBOARD_G;
				//Usamos evento mostrar nodos para no tener q crear evento nuevo 
}

void FSM::selectRandomFullNode(int i)
{
	int selectedNode = rand() % (i);		//Le resto uno pq ya esta incrementado en 1 el i para que usen los SPV
	cout << "selectedNode" << selectedNode << endl;
	fullArray[selectedNode]->setGenesisState(GenesisStates::COLLECTING);
	fullArray[selectedNode]->setTotalFullNodes((int) fullArray.size());

}

unsigned int FSM::makeRandomTime(void)
{
	return ((rand() % 10000) + 10);
}

void FSM::RutaDefaultInitState(genericEvent* ev)
{
	/*
		unsigned long int timeoutVar;
	*/

	if (static_cast<evMulti*>(ev)->getType() == NoEvent)			//Usamos evento mostrar nodos para no tener q crear evento nuevo 
	{
	
	}
}

bool FSM::isNetworkReady(void)
{
	bool itsReady;
	itsReady = true;
	
	for (auto& node : fullArray) 
	{
		if ((node->getGenesisState() != GenesisStates::NETCREATED) && (node->getGenesisState() != GenesisStates::SENDINGLAYOUT))
			itsReady = false;
		if ((node->getGenesisState() != GenesisStates::SENDINGLAYOUT) && (node->getGenesisState() != GenesisStates::NETCREATED))
		itsReady = false;

	}

	return itsReady;
}

void FSM::Start_app_r_acc(genericEvent* ev)
{
	if (static_cast<evMostrarNodos*>(ev)->getType() == MostrarNodos)			//Usamos evento mostrar nodos para no tener q crear evento nuevo 
	{
		this->state4Graphic = DASHBOARD_G;
	}
}

Blockchain& FSM::getBchain() {
	return Bchain;
}