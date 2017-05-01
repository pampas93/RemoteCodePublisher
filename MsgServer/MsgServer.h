#pragma once


#include "../HttpMessage/HttpMessage.h"
#include "../Sockets/Sockets.h"
#include "../FileSystem/FileSystem.h"
//#include "../Logger/Cpp11-BlockingQueue.h"
#include "../Logger/Logger.h"
#include "../Utilities/Utilities.h"
#include <string>
#include <iostream>

using Show = Logging::StaticLogger<1>;
using namespace Utilities;

class ClientHandler
{
public:
	ClientHandler(Async::BlockingQueue<HttpMessage>& msgQ) : msgQ_(msgQ) {}
	void operator()(Socket socket);
private:
	bool connectionClosed_;
	HttpMessage readMessage(Socket& socket);
	bool readFile(const std::string& filename, size_t fileSize, Socket& socket);
	Async::BlockingQueue<HttpMessage>& msgQ_;
};


class ServerCounter
{
public:
	ServerCounter() { ++servCount; }
	size_t count() { return servCount; }
private:
	static size_t servCount;
};
size_t ServerCounter::servCount = 0;


/////////////////////////////////////////////////////////////////////
// MsgClient class
// - was created as a class so more than one instance could be 
//   run on child thread
//
class MsgServer
{
public:
	using EndPoint = std::string;
	void execute(const size_t TimeBetweenMessages, const size_t NumMessages);
private:
	HttpMessage makeMessage(size_t n, const std::string& msgBody, const EndPoint& ep);
	void sendMessage(HttpMessage& msg, Socket& socket);
	bool sendFile(const std::string& fqname, Socket& socket);
};