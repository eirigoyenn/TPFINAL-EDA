#include "NodeServer.h"


/*Server constructor. Initializes io_context, acceptor and socket.
Calls waitForConnection to accept connections.*/

NodeServer::NodeServer(boost::asio::io_context& io_context_, std::string IP, pcallback pcback_, unsigned int port_) :
	io_context(io_context_), acceptor(io_context_, tcp::endpoint(boost::asio::ip::tcp::v4(), port_)), socket(io_context_), nodeIP(IP), pcback(pcback_), port(port_) // q onda con ese puerto 80 eso q era ? // ahi creo q lucas dijo algo de remote endpoints 
{
	socketQ.push_back(&socket);
	ClientInputQ.emplace_back();
	if (socketQ.front()->is_open()) {
		socketQ.front()->shutdown(boost::asio::ip::tcp::socket::shutdown_both);
		socketQ.front()->close();
		socketQ.pop_front();
	}
	waitForConnection();
}

//Destructor. Closes open socket and acceptor.

NodeServer::~NodeServer() {
	std::cout << "\nClosing server.\n";
	
	for (auto& tempSocket : socketQ) {
		
		if (tempSocket->is_open()) {
			tempSocket->shutdown(boost::asio::ip::tcp::socket::shutdown_both);
			tempSocket->close();
		}
	}
	ClientInputQ.clear();
	socketQ.clear();
	if (acceptor.is_open())
		acceptor.close();

	std::cout << "Server is closed.\n";
}

/*Sets acceptor to accept (asynchronously).*/
void NodeServer::waitForConnection() {


	if (socketQ.front()->is_open()) {
		std::cout << "Error: Can't accept new connection from an open socket" << std::endl;
		auto newSocketPtr=new boost::asio::ip::tcp::socket(io_context);
		socketQ.emplace_back(newSocketPtr);
		ClientInputQ.emplace_back();
	}

	std::cout << "Waiting for connection.\n";
	
	acceptor.async_accept(*(socketQ.back()), boost::bind(&NodeServer::connectionCallback, this, boost::asio::placeholders::error));
	
	

}

//Closes socket and clears message holder.
void NodeServer::closeConnection() {
	socketQ.front()->shutdown(boost::asio::ip::tcp::socket::shutdown_both);
	socketQ.front()->close();
	socketQ.pop_front();
	ClientInputQ.pop_front();

}


/*Called when there's been a connection.*/
void NodeServer::connectionCallback(const boost::system::error_code& error) {
	if ((!error) || (error == boost::asio::error::eof)) {
		//Sets socket to read request.
		socket.async_read_some
		(
			boost::asio::buffer(ClientInputQ.front(), MAXSIZE),
			boost::bind
			(
				&NodeServer::parse,
				this, boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred
			)
		);
	}

	else
		std::cout << error.message() << std::endl;
}



/*Called when a message has been sent to client.*/
void NodeServer::messageCallback(const boost::system::error_code& error, size_t bytes_sent)
{
	if (!error)
		std::cout << "Response sent correctly.\n\n";

	else
		std::cout << "Failed to respond to connection\n\n";

	/*Once the answer was sent, it frees acceptor for a new connection.*/
	waitForConnection();
}


/*Validates input given in GET request.*/
void NodeServer::parse(const boost::system::error_code& error, size_t bytes_sent) {

	if (!error || error==boost::asio::error::eof) {

		bool isInputOk = false;

		//Creates string message from request.
		std::string message(ClientInputQ.front());

		//Validator has the da coin form.
		std::string validator = "/eda_coin/";

		//If there's been a match ...\
		
		auto it = message.find(validator);

		if (it != std::string::npos) 
		{
			/* LETS CHECK FOR FILTERS */
			//Validator has the filter.
			std::string validator = "/eda_coin/";

			//If there's been a match ...\
			
			auto it = message.find(validator);
			result = pcback(message);


			if (result["status"] == true) {

				std::cout << "client sent correct input" << std::endl;
				
			}
			else
			{
				//Error de contenido.
				result["status"] = false;
				result["result"] = 2;
			}
		}
		else
		{
			//Error de formato
			result["status"] = false;
			result["result"] = 1;
			std::cout << "Client sent wrong input.\n";

		}

		answer();
		waitForConnection();
	}

	//If there's been an error, prints the message.
	else
		std::cout << error.message() << std::endl;
}


/*Responds to input.*/
void NodeServer::answer() {

	generateTextResponse();

	boost::asio::async_write(
		*(socketQ.front()),
		boost::asio::buffer(ServerOutput), // aca poniamos mensaje
		boost::bind(
			&NodeServer::response_sent_cb,
			this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred
		)
	);


	socketQ.front()->shutdown(boost::asio::ip::tcp::socket::shutdown_both);
	socketQ.front()->close();
	if (socketQ.size() > 1){
		socketQ.pop_front();
		ClientInputQ.pop_front();
	}
}

/*Generates http response, according to validity of input.*/
void NodeServer::generateTextResponse(void) {

	ServerOutput = "HTTP/1.1 200 OK\r\nDate:" + makeDaytimeString(0) + "Content-Length:" + std::to_string(result.dump().size()) +
		"\r\nContent-Type: application/json; charset=iso-8859-1\r\n\r\n";

	msg = result.dump();
	ServerOutput += msg.substr(0, msg.size());

	ServerOutput += "\r\n\r\n";
//	std::cout << ServerOutput;

}


void NodeServer::response_sent_cb(const boost::system::error_code& error,
	size_t bytes_sent)
{
	std::cout << "response_sent_cb()" << std::endl;
	if (!error) {
		std::cout << "Response sent. " << bytes_sent << " bytes." << std::endl;
	}

}

std::string NodeServer::makeDaytimeString(int secs) { //funcion para devolver el tiempo 


	std::chrono::system_clock::time_point time = std::chrono::system_clock::now();

	time += std::chrono::seconds(secs);

	time_t time_ = std::chrono::system_clock::to_time_t(time);


	return ctime(&time_);
}