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
//		char buffer[9];
//		sprintf_s(buffer, "%08X", stoi(prevBlockId_) + 1);
//		blockID_=std::string(buffer);
//		block["blockid"] = blockID_;
//	}
//	else
//	{
//		block["previousblockid"] = "00000000";
//		block["blockid"] = "00000001";
//	}
//
//	//Agrego transacciones
//	for (const auto& Tx_ : transactions_)
//	{
//		block["tx"].push_back(Tx_);
//	}
//	block["nTx"] = transactions_.size();
//
//	//Completo MerkleRoot
//	std::string merkleRoot_ = NodeBlockchain.createMerkleRoot(block);
//	block["merkleroot"] = merkleRoot_;
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
