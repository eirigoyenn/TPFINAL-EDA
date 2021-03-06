#pragma once
#include "Block.h"
#include "json.hpp"
#include <list>
#include <fstream>
#define BCHAINERROR 4
using json = nlohmann::json;

class Blockchain
{
public:
	
	void parsingBlockchain(json chain_JData);
	vector<Block>& getBlocksArr(void) { return BlocksArr; }
	size_t getBlocksSize() { return BlocksArr.size(); }
	int saveBlockInfo(std::string);
	const string createMerkleRoot(const json& block);
	void addBlock(const json& block_json);
	std::string generateBlockId(const json& block);

private:

	//list<Block> Bchain;

	/* JSON */
	json Jdata;
	vector<Block> BlocksArr;

};

