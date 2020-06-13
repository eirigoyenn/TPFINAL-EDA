#include "NodeClient.h"


NodeClient::NodeClient(std::string IP_, int port_)
{
	IP = "localhost";
	own_port = port_;
	stillRunning = 1;
	
}

NodeClient::NodeClient(std::string IP_, int port_, std::vector<NodoSubconjunto>* PTR)
{
	IP = "localhost";
	own_port = port_;
	stillRunning = 1;
	Subconjunto = PTR;

	easyHandler = curl_easy_init();
	if (!easyHandler)
	{
		this->setErrorCode(CURLINIT_ERROR);
		this->setErrorMsg("Unable to start curl");
	}
	multiHandle = curl_multi_init();
	if (!multiHandle)
	{
		this->setErrorCode(CURLINIT_ERROR);
		this->setErrorMsg("Unable to start curl");
	}
}

NodeClient::NodeClient(std::string IP, int own_port, int neighbour_port)
{
	IP = "localhost";
	own_port = own_port;
	port = neighbour_port;
	stillRunning = 1;
	easyHandler = curl_easy_init();
	if (!easyHandler)
	{
		this->setErrorCode(CURLINIT_ERROR);
		this->setErrorMsg("Unable to start curl");
	}
	multiHandle = curl_multi_init();
	if (!multiHandle)
	{
		this->setErrorCode(CURLINIT_ERROR);
		this->setErrorMsg("Unable to start curl");
	}
}

NodeClient::NodeClient()
{
}


NodeClient::~NodeClient()
{
	curl_multi_remove_handle(multiHandle, easyHandler);
	curl_easy_cleanup(easyHandler);
	curl_multi_cleanup(multiHandle);
	
}

bool NodeClient::performRequest(void)
{
	if (IP.length() && port)
	{
		/*static bool isFinished = false;*/
		bool res = true;
		if (stillRunning)
		{
			multiError = curl_multi_perform(multiHandle, &stillRunning);
			if (multiError != CURLE_OK)
			{
				errorCode = CURLPERFORM_ERROR;
				errorMsg = "Could not perform curl.";
				std::cout << std::endl << " >>> REPLY <<<\n" << reply << std::endl;
				curl_easy_cleanup(easyHandler);
				curl_multi_cleanup(multiHandle);
				
			}

		}
		else
		{
			//Se limpia curl
			stillRunning = 1;						// MIRAR QUE PASA CON STILLRUNNING. DEVUELVE ALGO CURLMULTIPERFORME CUANDO TERMINA EL MSG?POR AHI EVITAMOS EL FLAG!!
			curl_easy_cleanup(easyHandler);
			curl_multi_cleanup(multiHandle);
			
			std::cout << std::endl  << "ESTA ES LA RESPUESTA" << reply << std::endl;
			if (reply.size() != 0){
				parsedReply = json::parse(reply);
			
				if (reply.find("NETWORK_NOTREADY") != std::string::npos)
				{
					//Agregamos a lista de nodos pertenecientes a la red
					NodoSubconjunto nodo2add;
					nodo2add.TEMP_ID = parsedReply["blockID"];
					nodo2add.TEMP_PUERTO = parsedReply["port"];
					nodo2add.numberofConnections = 0;
					nodo2add.checked = false;


					(*Subconjunto).push_back(nodo2add);
				}
				else if (reply.find("NETWORK_READY") != std::string::npos)
				{
					particularAlgorithm();
				}
			}
								// ACA hay que mandar reply a algun lado
			res = false;
		}

		return res;
	}
	else
	{
		errorCode = INVALID_DATA;
		errorMsg = "Invalid data.";
		std::cout << errorMsg;
		return false;
	}
}

void NodeClient::useGETmethod(std::string path_)
{
	method = GET;
	host = IP + ":" + std::to_string(port);
	url = "http://" + host + path_;
	//struct curl_slist* list = nullptr;
	reply.clear();
	easyHandler = curl_easy_init();
	if (!easyHandler)
	{
		this->setErrorCode(CURLINIT_ERROR);
		this->setErrorMsg("Unable to start curl");
	}
	multiHandle = curl_multi_init();
	if (!multiHandle)
	{
		this->setErrorCode(CURLINIT_ERROR);
		this->setErrorMsg("Unable to start curl");
	}
	/*Prosigo a configurar CURL para usar con el m�todo GET*/
	if (errorCode == ERROR_FREE2)
	{
		//Se configura la URL de la p�gina
		curl_easy_setopt(easyHandler, CURLOPT_URL, url.c_str());
		//Se configura para trabajar con HTTP
		curl_easy_setopt(easyHandler, CURLOPT_PROTOCOLS, CURLPROTO_HTTP);
		//Configuramos para que curl use myCallback al momento de escribir y a reply como userData.
		curl_easy_setopt(easyHandler, CURLOPT_WRITEFUNCTION, myCallback);
		curl_easy_setopt(easyHandler, CURLOPT_WRITEDATA, &reply);
		//Configuramos para recibir info de error, �til para debuguear
		curl_easy_setopt(easyHandler, CURLOPT_VERBOSE, 1L);
		//Configuro para que curl pueda seguir redireccionamiento de ser necesario
		curl_easy_setopt(easyHandler, CURLOPT_FOLLOWLOCATION, 1L);
		//Configuro el puerto de destino
		curl_easy_setopt(easyHandler, CURLOPT_PORT, port);
		//Configuro el propio puerto.
		curl_easy_setopt(easyHandler, CURLOPT_LOCALPORT, own_port);
		//Set handler y multiHandle
		curl_multi_add_handle(multiHandle, easyHandler);
		//Configuro el header
		/*list=curl_slist_append(list, data.dump().c_str());
		curl_easy_setopt(easyHandler, CURLOPT_HTTPHEADER, list);*/
	}
}

void NodeClient::usePOSTmethod(std::string path_, const json data)
{
	method = POST;
	host = IP + ":" + std::to_string(port);
	url = "http://" + host + path_;
	struct curl_slist* list = nullptr;
	reply.clear();
	myjson = data.dump();
	const char* mydata = myjson.c_str();
	std::string line("Content-Type: application/json");
	std::string line2("Content-Length:" + std::to_string(strlen(mydata)));

	easyHandler = curl_easy_init();
	if (!easyHandler)
	{
		this->setErrorCode(CURLINIT_ERROR);
		this->setErrorMsg("Unable to start curl");
	}
	multiHandle = curl_multi_init();
	if (!multiHandle)
	{
		this->setErrorCode(CURLINIT_ERROR);
		this->setErrorMsg("Unable to start curl");
	}


	/*Prosigo a configurar CURL para usar con el m�todo POST*/
	if (errorCode == ERROR_FREE2)
	{

		list = curl_slist_append(list, line.c_str());
		list = curl_slist_append(list, "Expect:");
		list = curl_slist_append(list, "Accept: application/json:");
		//list = curl_slist_append(list,line2.c_str());
		curl_easy_setopt(easyHandler, CURLOPT_HTTPHEADER, list);
		curl_easy_setopt(easyHandler, CURLOPT_POSTFIELDSIZE, -1L);
		curl_easy_setopt(easyHandler, CURLOPT_POSTFIELDS, mydata);
		curl_easy_setopt(easyHandler, CURLOPT_POST, 1L);

		curl_easy_setopt(easyHandler, CURLOPT_TIMEOUT, 15);
		curl_easy_setopt(easyHandler, CURLOPT_MAXREDIRS, 1);



		//Se configura la URL de la p�gina
		curl_easy_setopt(easyHandler, CURLOPT_URL, url.c_str());
		//Configuro el propio puerto
		curl_easy_setopt(easyHandler, CURLOPT_LOCALPORT, own_port);
		//Configuro el puerto dedestino
		curl_easy_setopt(easyHandler, CURLOPT_PORT, port);
		//Se configura para trabajar con HTTP
		curl_easy_setopt(easyHandler, CURLOPT_PROTOCOLS, CURLPROTO_HTTP);
		//Configuramos para que curl use myCallback al momento de escribir y a reply como userData.
		curl_easy_setopt(easyHandler, CURLOPT_WRITEFUNCTION, myCallback);
		curl_easy_setopt(easyHandler, CURLOPT_WRITEDATA, &reply);
		//Configuramos para recibir info de error, �til para debuguear
		curl_easy_setopt(easyHandler, CURLOPT_VERBOSE, 1L);
		//Configuro para que curl pueda seguir redireccionamiento de ser necesario
		curl_easy_setopt(easyHandler, CURLOPT_FOLLOWLOCATION, 1L);
		//Set handler y multiHandle
		curl_multi_add_handle(multiHandle, easyHandler);
		//Configuro el header


	}
}

void NodeClient::setIP(std::string IP_)
{
	IP = IP_;
}

void NodeClient::setPort(unsigned int port_)
{
	port = port_;
}

void NodeClient::setURL(std::string URL_)
{
	url = URL_;
}

void NodeClient::setHost(std::string host_)
{
	host = host_;
}

void NodeClient::setMethod(method_n method_)
{
	method = method_;
}

void NodeClient::setRunningStatus(int RunningStatus)
{
	stillRunning = RunningStatus;
}

void NodeClient::setErrorCode(errorCode_n errorCode_)
{
	errorCode = errorCode_;
}

void NodeClient::setErrorMsg(std::string errorMsg_)
{
	errorMsg = errorMsg_;
}

std::string NodeClient::getIP(void)
{
	return IP;
}

unsigned int NodeClient::getPort(void)
{
	return port;
}

std::string NodeClient::getURL(void)
{
	return url;
}

std::string NodeClient::getHost(void)
{
	return host;
}

method_n NodeClient::getMethod(void)
{
	return method;
}

int NodeClient::getRunningStatus(void)
{
	return stillRunning;
}

std::string NodeClient::getReply(void)
{
	return reply;
}

errorCode_n NodeClient::getErrorCode(void)
{
	return errorCode;
}

std::string NodeClient::getErrorMsg(void)
{
	return errorMsg;
}

size_t myCallback(char* contents, size_t size, size_t nmemb, void* userp)
{
	std::string* userDataPtr = (std::string*) userp;
	userDataPtr->append(contents, size * nmemb);

	return size * nmemb;


	//size_t realsize = size * nmemb;
	//char* data = (char*)contents;
	////fprintf(stdout, "%s",data);
	//std::string* s = (std::string*)userp;
	//s->append(data, realsize);
	//return realsize;						//recordar siempre devolver realsize
}


void NodeClient::particularAlgorithm(void)
{


	int nextNode, nextNode_index;
	int index = NOTFOUND;
	int i, j;
	json layout; //Layout de la red
	json nodes; //Nodos de la red
	json edges;	//Aristas de la red

	if ((*Subconjunto).size())	//Cargo los nodos con ID:puerto (no se me ocurre otra forma).
	{
		for (int i = 0; i < (*Subconjunto).size(); i++) {
			json nodeInfo = "localhost:" + std::to_string((*Subconjunto)[i].TEMP_PUERTO);
			layout["nodes"] += nodeInfo;
		}
	}

	if ((*Subconjunto).size() > 2) //Necesito m�s de dos elementos para formar la red
	{

		for (i = 0;  i < (*Subconjunto).size() ; i++)
		{
			if ((*Subconjunto)[i].numberofConnections >= 2) //Si ya tiene m�s de dos conexiones no hace falta seguir agregando.
			{
				std::cout << (*Subconjunto)[i].numberofConnections << std::endl;
			}

			//Caso contrario sigue el agloritmo
			else
			{
				while ((*Subconjunto)[i].numberofConnections < 2)
				{

					nextNode_index = (rand() % ((*Subconjunto).size())); //Busco un aleatorio para conextarme
					nextNode=randomPORT(nextNode_index);

					for (j = 0; j < (*Subconjunto).size(); j++) //
					{

						if ((*Subconjunto)[i].numberofConnections == 0 ||(*Subconjunto)[i].connections[0] != nextNode ){
							index = nextNode;
						}
					}

					if (index != NOTFOUND ) {		//Si se logr� conectar con otro o toco aleatorio el mismo nodo, busca otro.

						//Agregar JSONS
						edges.clear();
						std::string Node1Info = (*Subconjunto)[i].TEMP_ID + ":" + std::to_string((*Subconjunto)[i].TEMP_PUERTO);
						std::string Node2Info = (*Subconjunto)[nextNode_index].TEMP_ID + ":" + std::to_string((*Subconjunto)[nextNode_index].TEMP_PUERTO);
						edges.push_back({ { "target1", Node1Info }, { "target2", Node2Info } });
						layout["edges"] = edges;

						(*Subconjunto)[i].connections.push_back(nextNode); //Los agrego como conectados
						(*Subconjunto)[nextNode_index].connections.push_back(i);
						(*Subconjunto)[i].numberofConnections++;
						(*Subconjunto)[nextNode_index
						 ].numberofConnections++;
					}
				}
			}
		}
	}
	else if((*Subconjunto).size() == 2)  //Caso contrario, armo el layout con los dos nodos presentes, no hace falta BFS ni DFS puesto que ya es conexo.
	{
		std::string Node1Info = (*Subconjunto)[0].TEMP_ID + ":" + std::to_string((*Subconjunto)[0].TEMP_PUERTO);
		std::string Node2Info = (*Subconjunto)[1].TEMP_ID + ":" + std::to_string((*Subconjunto)[1].TEMP_PUERTO);
		edges.push_back({ { "target1", Node1Info }, { "target2", Node2Info } });
		layout["edges"] = edges;
	}
}

bool NodeClient::isConvex(void)
{
	BFS(0);
	for (int i = 0; i < (*Subconjunto).size(); i++)	//Busco en todos los nodos a ver si hay alguno no visitado
	{
		if (!((*Subconjunto)[i].checked))
			return false;
	}

	return true;
}

void NodeClient::BFS(int nodeToVisit)
{
	if (!((*Subconjunto)[nodeToVisit].checked)) //Si todav�a no se lleg� a este nodo, lo marca como visitado y visita a todos sus vecinos
	{
		(*Subconjunto)[nodeToVisit].checked = true;
		for (int i = 0; i < (*Subconjunto)[nodeToVisit].connections.size(); i++)
			BFS(i);
	}
}

int NodeClient::randomPORT(int i){
	return (*Subconjunto)[i].TEMP_PUERTO;
}
