#pragma once

#include "../HttpMessage/HttpMessage.h"
#include "../Sockets/Sockets.h"
#include "../FileSystem/FileSystem.h"
#include "../Cpp11-BlockingQueue/Cpp11-BlockingQueue.h"
#include "../Logger/Logger.h"
#include "../Utilities/Utilities.h"
#include "../Analyzer/Executive.h"
#include <string>
#include <iostream>
#include <thread>

using Show = Logging::StaticLogger<1>;
using namespace Utilities;
using Utils = StringHelper;
using namespace CodeAnalysis;

class ClientCounter
{
public:
	ClientCounter() { ++clientCount; }
	size_t count() { return clientCount; }
private:
	static size_t clientCount;
};
/////////////////////////////////////////////////////////////////////
// MsgClient class
// - was created as a class so more than one instance could be 
//   run on child thread
//
class MsgClient
{
public:
	using EndPoint = std::string;
	void execute(const size_t TimeBetweenMessages, const size_t NumMessages, std::string type, std::string category, std::string path = "none");
	//std::vector<std::string> getAllFiles(std::string path);
	std::string uploadFunction(std::string path, std::vector<std::string> files, int cat);
	std::string deleteFunction(int cat);
	std::string publishFunction(int cat);
	std::string displayFunction(int cat);
	std::string downloadFunction(int cat, std::vector<std::string> filesToOpen = {""});

	HttpMessage listenerFunction();
	bool analyzeMessageFromServer(HttpMessage m);
	std::string getCategory(int c);

	void sendFiles2Server(Socket& socket, std::string category);

	//void uploadMessage(std::string path, std::string category);
	//void deleteMessage(std::string category);
	//void publishMessage(std::string category);
	//void displayMessage(std::string category);
	//void downloadMessage(std::string category);
	std::vector<std::string> filestoSend;
private:
	
	std::vector<std::string> filestoOpen;

	HttpMessage makeMessage(size_t n, const std::string& msgBody, const EndPoint& ep, std::string category, std::string type);
	void sendMessage(HttpMessage& msg, Socket& socket);
	bool sendFile(const std::string& filenameAbs, Socket& socket, std::string category);
};
class ServerHandler
{
public:
	ServerHandler(Async::BlockingQueue<HttpMessage>& msgQ) : msgQ_(msgQ) {}
	void operator()(Socket socket);
	//void listenerFunction();

private:
	bool connectionClosed_;
	HttpMessage readMessage(Socket& socket);
	bool readFile(const std::string& filename, size_t fileSize, Socket& socket, std::string category);
	Async::BlockingQueue<HttpMessage>& msgQ_;
};
