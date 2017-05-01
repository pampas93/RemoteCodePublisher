#pragma once

#include "../HttpMessage/HttpMessage.h"
#include "../Sockets/Sockets.h"
#include "../FileSystem/FileSystem.h"
#include "../Cpp11-BlockingQueue/Cpp11-BlockingQueue.h"
#include "../Logger/Logger.h"
#include "../Utilities/Utilities.h"
#include <string>
#include <iostream>

//class CommClass {
//public:
//
//	Sender sender;
//	Receiver receiver;
//
//private:
//	std::string dirPath;
//	std::string port;
//	SocketSystem ss;
//	SocketListener sl(8080, Socket::IP6);
//};


using Show = Logging::StaticLogger<1>;

class Sender {

};

class ClientHandler {

public:
	ClientHandler(Async::BlockingQueue<HttpMessage>& msgQ) : receiverQ(msgQ) {}
	void operator()(Socket socket);

private: 
	//std::string dir;
	bool connectionClosed_;
	HttpMessage getMessage();
	HttpMessage readMessage(Socket& socket);
	bool readFile(const std::string& filename, size_t fileSize, Socket& socket);
	Async::BlockingQueue<HttpMessage>& receiverQ;

};

class Server {

};