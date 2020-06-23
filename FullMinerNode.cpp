//#include "FullMinerNode.h"
//#include <chrono>
//
//double elapsedTime = 30.0;
//
//
//FullMinerNode::FullMinerNode()
//{
//}
//
//FullMinerNode::FullMinerNode(boost::asio::io_context & io_context_, unsigned int ID_, std::string IP_, unsigned int port_) //:io_context(io_context_) No detecta io_context y no logro ver por qué ya que es dato miembro de Fullnode
//{
//	ID = ID_;
//	IP = IP_;
//	port = port_;
//	publicKey= std::to_string(std::rand() % 99999999);
//	client = new NodeClient(IP, port, 0);
//	server = new NodeServer(io_context_, IP, boost::bind(&FullNode::fullCallback, this, _1), port);
//}
//
//
//FullMinerNode::~FullMinerNode()
//{
//}
//
//void FullMinerNode::minerPerformRequest(void)
//{
//	static auto startTime = chrono::steady_clock::now();
//	auto stopTime= chrono::steady_clock::now();
//
//	if (chrono::duration_cast<chrono::seconds>(stopTime - startTime).count() > elapsedTime)
//	{
//		//Si pasó el minuto mina el nuevo bloque.
//		cout << "Elapsed time in seconds: "
//			<< chrono::duration_cast<chrono::seconds>(stopTime - startTime).count()
//			<< "sec" << endl;
//
//		startTime = stopTime;
//		mineBlock();
//	}
//}
//
//void FullMinerNode::mineBlock(void)
//{
//	json block;
//	std::string blockID_;
//	block["height"] = NodeBlockchain.getBlocksSize();
//	block["nonce"] = rand() % 65536;
//
//	//Verifico si se trata del primer bloque de la blockchain.
//	if (NodeBlockchain.getBlocksSize())
//	{
//		int blockNumber = NodeBlockchain.getBlocksSize();
//		std::string prevBlockId_ = NodeBlockchain.getBlocksArr()[blockNumber - 1].getBlockID();
//		block["previousblockid"] = prevBlockId_;
//	}
//	else
//	{
//		block["previousblockid"] = "00000000";
//	}
//
//	//Agrego transacciones
//	for (const auto& Tx_ : transactions_)
//	{
//		block["tx"].push_back(Tx_);
//	}
//
//	//Agrego transacción del Minero
//	json minerTx_ = getMinerFee();
//	block["tx"].push_back(minerTx_);
//
//	//Agrego total de transacciones
//	block["nTx"] = transactions_.size()+1;
//
//	//Completo MerkleRoot
//	std::string merkleRoot_ = NodeBlockchain.createMerkleRoot(block);
//	block["merkleroot"] = merkleRoot_;
//
//	//Agrego BlockID
//	block["blockid"] = NodeBlockchain.generateBlockId(block);
//
//	//Agrego el bloque a la blockchain y procedo al flooding
//	NodeBlockchain.addBlock(block);
//	for (auto& neighbour : neighbours)
//	{
//		POSTBlock(neighbour.second.ID, blockID_);
//	}
//
//	//Limpio la lista de transacciones
//	transactions_ = json();
//
//}
//
//json FullMinerNode::getMinerFee(bool realTx_)
//{
//	json newTx_;
//	newTx_["nTxin"] = 0;
//	newTx_["nTxout"] = 1;
//	json vout_;
//	//La transacción del minero tiene vin vacío
//	newTx_["vin"];
//	//Completo el output de la transacción (que es el mismo minero).
//	vout_["publicid"] = publicKey;
//	vout_["amount"] = 10;
//	newTx_["vout"].push_back(vout_);
//	newTx_["txid"] = generateID(to_string(rand()));
//
//	//si la transacción es verdadera, la guardo en la lista de UTXOs, sino no hago nada.
//	if (realTx_)
//	{
//		UTXO newUTXO;
//		newUTXO.txid = newTx_["txid"].get<std::string>();
//		newUTXO.amount = vout_["amount"].get<unsigned int>();
//		newUTXO.outputIndex = 1;
//		newUTXO.publicid = publicKey;
//	}
//
//	return realTx_;
//}
