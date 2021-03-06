#include "Blockchain.h"



void Blockchain::parsingBlockchain(json chain_JData)
{
	/* GETTING BLOCKS */
	for (const auto& data : chain_JData)
	{
		Block tempBlock;

		string BBID = data["blockid"];
		tempBlock.setBlockID(BBID);

		string MKLR = data["merkleroot"];
		tempBlock.setMerkleRoot(MKLR);

		uint NTX = data["nTx"];
		tempBlock.setNtx(NTX);

		int NCE = data["nonce"];
		tempBlock.setNonce(NCE);

		uint HGT = data["height"];
		tempBlock.setHeight(HGT);

		string PBID = data["previousblockid"];
		tempBlock.setPrevBlockID(PBID);

		json transObj = data["tx"];

		/* TRANSACCIONES */
		for (const auto& TXX : transObj)			//Voy dividiendo en pequenos jsons
		{
			Transaction tempTx;

			auto TXD = TXX["nTxin"];
			tempTx.nTxin = TXD;
		

			auto TXO = TXX["nTxout"];
			tempTx.nTxout = TXO;
			

			auto TXID = TXX["txid"];
			tempTx.txID = TXID.get<string>();

			

			/* VINS */
			json VinObj = TXX["vin"];

			for (const auto& VINdata : VinObj)
			{
				VinS tempVin;

				auto LBID = VINdata["blockid"];
				tempVin.LilblockID = LBID.get<string>();

				

				auto OUIX = VINdata["outputIndex"];
				tempVin.outputIndex = OUIX;

				

				auto SGT = VINdata["signature"];
				tempVin.signature = SGT.get<string>();

				

				auto TXID = VINdata["txid"];
				tempVin.txID = TXID.get<string>();;

				

				/* Vin temporario listo para agregar al vector de vins de transaccion temporal*/
				tempTx.vIn.push_back(tempVin);
			}

		
			tempBlock.setPush_Back(tempTx);
		}
		/* Block temporal listo para agregar a nuestro vector de bloques */
		BlocksArr.push_back(tempBlock);
	}
}
int Blockchain::saveBlockInfo(std::string path)
{
	std::ifstream chain(path.c_str(), std::ifstream::binary);
	int result=0;

	if (chain)
	{
		Jdata = json::parse(chain);
		parsingBlockchain(Jdata);

	}

	//CREO Q EN FASE DE VERIFICACION PODEMOS USAR EL CALLBACK DE JSON TALVEZ
	//https://nlohmann.github.io/json/classnlohmann_1_1basic__json_a265a473e939184aa42655c9ccdf34e58.html#a265a473e939184aa42655c9ccdf34e58
	else
	{
		result = BCHAINERROR;
	}
	
	return result;
}

const string Blockchain::createMerkleRoot(const json & block)
{
	Block blck(block);
	blck.createMerkleTree();
	return blck.getMerkleRoot();
}

void Blockchain::addBlock(const json & block_json)
{
	Block newblock(block_json);
	BlocksArr.push_back(newblock);
}

std::string Blockchain::generateBlockId(const json & block)
{
	//Para generar el BlockID hago un hash del header (menos el blockid)
	Block blck(block);
	auto blockHeader = blck.getBlockHeader();
	blockHeader.erase("blockid");
	std::string unhashedId;
	for (auto& header_ : blockHeader)
	{
		unhashedId.append(header_.get<std::string>());
	}
	unsigned int BlockId_ = blck.generateID((unsigned char*)unhashedId.c_str());
	char tohex[9];
	int n = sprintf_s(tohex, sizeof(tohex), "%08X", BlockId_);
	std::string BlockID_(tohex);
	return BlockID_;

}
