#pragma once
/////////////////////////////////////////////////////////////////////
//  MsgClient.h		 - Client Functionalities	   				   //
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
MsgClient Class > ---- >

* std::string filesFromGUI										- Files accepted from GUI
* std::vector<std::string> filestoOpen							- Vector to store files need to be open
* std::string lazyFile											- Vector to store files after lazy download
* HttpMessage makeMessage										- Function to Make message
* void sendMessage												- Function to Send Message
* bool sendFile													- Function to send file

ServerHandler Class > ---- >

* bool connectionClosed_;										- Boolean variable used to close Server Hander
* HttpMessage readMessage										- Function to Read message
* bool readFile													- Function to read File
* Async::BlockingQueue<HttpMessage>& msgQ_;						- Receiver blocking Queue


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
	std::string uploadFunction(int cat, std::string files);
	std::string deleteFunction(int cat);
	std::string publishFunction(int cat);
	std::string displayFunction(int cat);
	std::string downloadFunction(int cat, std::string filesToOpen);
	std::string downloadLazyFunction(int cat, std::string file);

	void testExecutive();

	HttpMessage listenerFunction();
	bool analyzeMessageFromServer(HttpMessage m);
	std::string getCategory(int c);

	void openBrowser(std::string fpath);
	void sendFiles2Server(Socket& socket, std::string category);
	std::vector<std::string> split(const std::string &s, char delim);

	
private:
	std::string filesFromGUI;
	std::vector<std::string> filestoOpen;
	std::string lazyFile;
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
