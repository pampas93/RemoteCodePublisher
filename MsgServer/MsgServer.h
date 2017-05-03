#pragma once


#include "../HttpMessage/HttpMessage.h"
#include "../Sockets/Sockets.h"
#include "../FileSystem/FileSystem.h"
#include "../Cpp11-BlockingQueue/Cpp11-BlockingQueue.h"
#include "../Logger/Logger.h"
#include "../Utilities/Utilities.h"
#include "../Analyzer/Executive.h"
#include <string>
#include <stdio.h>
#include <iostream>

using Show = Logging::StaticLogger<1>;
using namespace Utilities;

class ClientHandler
{
public:
	ClientHandler(Async::BlockingQueue<HttpMessage>& msgQ) : msgQ_(msgQ) {}
	void operator()(Socket socket);

	int findFromPort(HttpMessage msg);
	void analyseMsgContent(HttpMessage msg);

	bool deleteCategory(std::string category);
	bool publishCategory(std::string category);

private:
	bool connectionClosed_;
	HttpMessage readMessage(Socket& socket);
	bool readFile(const std::string& filename, size_t fileSize, Socket& socket, std::string category);
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
	std::string getAllhtmlFiles(std::string category);
	void sendFiles2Client(Socket& socket, std::string category, std::string cPort);
	void execute(const size_t TimeBetweenMessages, const size_t NumMessages, int toPort, std::string type, std::string category);
private:
	HttpMessage makeMessage(size_t n, const std::string& msgBody, const EndPoint& ep, std::string category, std::string type);
	void sendMessage(HttpMessage& msg, Socket& socket);
	bool sendFile(const std::string& fqname, Socket& socket, std::string category, std::string cPort);
};