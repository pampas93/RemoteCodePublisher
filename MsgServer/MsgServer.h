#pragma once
/////////////////////////////////////////////////////////////////////
//  MsgServer.h		 - Server Functionalities	   				   //
//																   //
//  Language:      Visual C++ 2015                                 //
//  Platform:      Dell Inspiron, Windows 8.1			           //
//  Application:   Dependency Analysis - CIS 687 Project 4         //
//  Author:        Abhijit Srikanth SUID:864888072			       //
/////////////////////////////////////////////////////////////////////
/*
Module Operations:
==================

This package implements a client that sends HTTP style messages and
files to a server that simply displays messages and stores files.

This module defines a Client Sender and Receiver
* - The functions defined in this class are used for,
> Rceiving Messages from server
> Send messages to Server
> Upload files to Server
> Analyse messages received from Servers
> Socket and HTTP style message creation


Private Interface:
=================
MsgServer Class > ---- >

* std::string noParentString									- String to hold files with no parent files
* std::vector<std::string> filesForLazyD_MS						- Vector to store files, for lazy Download
* HttpMessage makeMessage										- Function to Make message
* void sendMessage												- Function to Send Message
* bool sendFile													- Function to send file

ClientHandler Class > ---- >

* std::string noParentFiles;									- String to hold files with no parent files
* lazyMap depMapCategory1;										- Dependency map of Category1
* lazyMap depMapCategory2;										- Dependency map of Category2
* lazyMap depMapCategory3;										- Dependency map of Category3
* bool connectionClosed_;										- Boolean variable used to close clientHander
* HttpMessage readMessage										- Function to Read message
* bool readFile													- Function to read File
* Async::BlockingQueue<HttpMessage>& msgQ_;						- Receiver blocking Queue
* std::vector<std::string> filesForLazyD_CH;					- Vector to store files, for lazy Download
* std::stack<std::string> stackForLazy;							- Stack used for Lazy download function


* Required Files:
*   HttpMessage.h, HttpMessage.cpp
*   Cpp11-BlockingQueue.h
*   Sockets.h, Sockets.cpp
*   FileSystem.h, FileSystem.cpp
*   Logger.h, Logger.cpp
*   Utilities.h, Utilities.cpp


Build commands
- devenv RemoteCodePublisher_OOD.sln

Maintenance History:
====================
ver 1.0 : 04 May 17
- first release
*/


#include "../HttpMessage/HttpMessage.h"
#include "../Sockets/Sockets.h"
#include "../FileSystem/FileSystem.h"
#include "../Cpp11-BlockingQueue/Cpp11-BlockingQueue.h"
#include "../Logger/Logger.h"
#include "../Utilities/Utilities.h"
#include "../Analyzer/Executive.h"
#include <stack> 
#include <string>
#include <stdio.h>
#include <iostream>

using Show = Logging::StaticLogger<1>;
using namespace Utilities;
using lazyMap = std::unordered_map<std::string, std::vector<std::string>>;

class ClientHandler
{
public:
	ClientHandler(Async::BlockingQueue<HttpMessage>& msgQ) : msgQ_(msgQ) {}
	void operator()(Socket socket);

	int findFromPort(HttpMessage msg);
	void analyseMsgContent(HttpMessage msg);
	bool deleteCategory(std::string category);
	void publishCategory(std::string category);
	void LazyDownloadRecursive(std::string file, lazyMap mapUsed);
	std::vector<std::string> returnLazyVector();

private:
	std::string noParentFiles;
	lazyMap depMapCategory1;
	lazyMap depMapCategory2;
	lazyMap depMapCategory3;
	std::vector<std::string> filesForLazyD_CH;
	std::stack<std::string> stackForLazy;
	bool connectionClosed_;
	HttpMessage readMessage(Socket& socket);
	bool readFile(const std::string& filename, size_t fileSize, Socket& socket, std::string category);
	Async::BlockingQueue<HttpMessage>& msgQ_;
};


class MsgServer
{
public:
	using EndPoint = std::string;
	std::string getAllhtmlFiles(std::string category);
	void sendFiles2Client(Socket& socket, std::string category, std::string cPort);
	void sendLazyFiles2Client(Socket& socket, std::string category, std::string cPort);
	void execute(const size_t TimeBetweenMessages, const size_t NumMessages, int toPort, std::string type, std::string category);
	void setLazyVector(std::vector<std::string> x);
	void setNoParentString(std::string x);
private:
	std::string noParentString;
	std::vector<std::string> filesForLazyD_MS;
	HttpMessage makeMessage(size_t n, const std::string& msgBody, const EndPoint& ep, std::string category, std::string type);
	void sendMessage(HttpMessage& msg, Socket& socket);
	bool sendFile(const std::string& fqname, Socket& socket, std::string category, std::string cPort);
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
