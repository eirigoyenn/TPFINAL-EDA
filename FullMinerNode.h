#pragma once
#include "FullNode.h"



class FullMinerNode :
	public FullNode
{
public:
	//FullMinerNode();
	FullMinerNode(boost::asio::io_context& io_context_, unsigned int ID_, std::string IP_, unsigned int port_, unsigned int randomTime_);

	~FullMinerNode();

	void minerPerformRequest(void);

	void mineBlock(void);
	json getMinerFee(bool realTX_);

private:
};

