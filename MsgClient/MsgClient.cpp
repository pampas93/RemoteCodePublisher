/////////////////////////////////////////////////////////////////////////
// MsgClient.cpp - Demonstrates simple one-way HTTP messaging          //
//                                                                     //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2016           //
// Application: OOD Project #4                                         //
// Platform:    Visual Studio 2015, Dell XPS 8900, Windows 10 pro      //
/////////////////////////////////////////////////////////////////////////
/*
* This package implements a client that sends HTTP style messages and
* files to a server that simply displays messages and stores files.
*
* It's purpose is to provide a very simple illustration of how to use
* the Socket Package provided for Project #4.
*/
/*
* Required Files:
*   MsgClient.cpp, MsgServer.cpp
*   HttpMessage.h, HttpMessage.cpp
*   Cpp11-BlockingQueue.h
*   Sockets.h, Sockets.cpp
*   FileSystem.h, FileSystem.cpp
*   Logger.h, Logger.cpp
*   Utilities.h, Utilities.cpp
*/
/*
* ToDo:
* - pull the sending parts into a new Sender class
* - You should create a Sender like this:
*     Sender sndr(endPoint);  // sender's EndPoint
*     sndr.PostMessage(msg);
*   HttpMessage msg has the sending adddress, e.g., localhost:8080.
*/

/////////////////////////////////////////////////////////////////////
// ClientCounter creates a sequential number for each client
//

#include "MsgClient.h"
size_t ClientCounter::clientCount = 0;


//----< factory for creating messages >------------------------------
/*
* This function only creates one type of message for this demo.
* - To do that the first argument is 1, e.g., index for the type of message to create.
* - The body may be an empty string.
* - EndPoints are strings of the form ip:port, e.g., localhost:8081. This argument
*   expects the receiver EndPoint for the toAddr attribute.
*/
HttpMessage MsgClient::makeMessage(size_t n, const std::string& body, const EndPoint& ep, std::string category, std::string type)
{
	HttpMessage msg;
	HttpMessage::Attribute attrib;
	EndPoint myEndPoint = "localhost:8082";  
	switch (n)
	{
	case 1:
		msg.clear();
		msg.addAttribute(HttpMessage::attribute("POST", "Message"));
		msg.addAttribute(HttpMessage::Attribute("mode", "oneway"));
		msg.addAttribute(HttpMessage::Attribute("category", category));
		msg.addAttribute(HttpMessage::Attribute("type", type));
		msg.addAttribute(HttpMessage::parseAttribute("toAddr:" + ep));
		msg.addAttribute(HttpMessage::parseAttribute("fromAddr:" + myEndPoint));

		msg.addBody(body);
		if (body.size() > 0)
		{
			attrib = HttpMessage::attribute("content-length", Converter<size_t>::toString(body.size()));
			msg.addAttribute(attrib);
		}
		break;
	default:
		msg.clear();
		msg.addAttribute(HttpMessage::attribute("Error", "unknown message type"));
	}
	return msg;
}

//----< send message using socket >----------------------------------
void MsgClient::sendMessage(HttpMessage& msg, Socket& socket)
{
	std::string msgString = msg.toString();
	socket.send(msgString.size(), (Socket::byte*)msgString.c_str());
}

//--------------------Sending file to server -----------------------
bool MsgClient::sendFile(const std::string& filenameAbs, Socket& socket, std::string category)
{
	std::string fqname = filenameAbs;
	FileSystem::FileInfo fi(fqname);
	size_t fileSize = fi.size();
	std::string sizeString = Converter<size_t>::toString(fileSize);
	FileSystem::File file(fqname);
	file.open(FileSystem::File::in, FileSystem::File::binary);
	if (!file.isGood())
		return false;

	HttpMessage msg = makeMessage(1, "", "localhost::8080", category, "upload");
	std::string filename = FileSystem::Path::getName(filenameAbs);
	msg.addAttribute(HttpMessage::Attribute("file", filename));
	msg.addAttribute(HttpMessage::Attribute("content-length", sizeString));
	sendMessage(msg, socket);
	const size_t BlockSize = 2048;
	Socket::byte buffer[BlockSize];
	while (true)
	{
		FileSystem::Block blk = file.getBlock(BlockSize);
		if (blk.size() == 0)
			break;
		for (size_t i = 0; i < blk.size(); ++i)
			buffer[i] = blk[i];
		socket.send(blk.size(), buffer);
		if (!file.isGood())
			break;
	}
	file.close();
	return true;
}

//----< this defines the behavior of the client >--------------------
void MsgClient::execute(const size_t TimeBetweenMessages, const size_t NumMessages, std::string type, std::string category, std::string path )
{
	Show::attach(&std::cout);
	Show::start();
	Show::title("Starting HttpMessage client on thread " + Utilities::Converter<std::thread::id>::toString(std::this_thread::get_id()));
	try
	{
		SocketSystem ss;
		SocketConnecter si;
		while (!si.connect("localhost", 8080))
		{
			Show::write("\n client waiting to connect");
			::Sleep(100);
		}
		HttpMessage msg;

		if (type == "upload") {
			msg = makeMessage(1, "uploading body; not using anywhere;", "localhost:8080", category, type);
			sendMessage(msg, si);
			sendFiles2Server(si, category);
		}
		else if (type == "delete" || type == "publish" || type == "display" || type == "download") {
			msg = makeMessage(1, type +"message", "localhost:8080", category, type);
			sendMessage(msg, si);
		}

		Show::write("\n");
		Show::write("\n  All done folks");
	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}
}

//--------------------Send each file to server --------------
void MsgClient::sendFiles2Server(Socket & socket, std::string category)
{
	std::vector<std::string> files = FileSystem::Directory::getFiles("../TestFiles", "*.cpp");
	for (size_t i = 0; i < files.size(); ++i)
	{
		Show::write("\n\n  sending file " + files[i]);
		sendFile(files[i], socket, category);
	}
	HttpMessage m;															//Message sent to tell server upload is over
	m = makeMessage(1, "upload over", "localhost:8080", category, "upload");
	sendMessage(m, socket);
}



//----------------Server Handler Functions----------------------------------------------------


HttpMessage ServerHandler::readMessage(Socket& socket)
{
	connectionClosed_ = false;
	HttpMessage msg;

	while (true)
	{
		std::string attribString = socket.recvString('\n');
		if (attribString.size() > 1)
		{
			HttpMessage::Attribute attrib = HttpMessage::parseAttribute(attribString);
			msg.addAttribute(attrib);
		}
		else
			break;
	}
	if (msg.attributes().size() == 0)
	{
		connectionClosed_ = true;
		return msg;
	}
	if (msg.attributes()[0].first == "POST")
	{
		std::string category = msg.findValue("category");
		std::string filename = msg.findValue("file");
		if (filename != "")
		{
			size_t contentSize;
			std::string sizeString = msg.findValue("content-length");
			if (sizeString != "")
				contentSize = Converter<size_t>::toValue(sizeString);
			else
				return msg;
			readFile(filename, contentSize, socket, category);
		}

		if (filename != "")
		{
			msg.removeAttribute("content-length");
			std::string bodyString = "<file>" + filename + "</file>";
			std::string sizeString = Converter<size_t>::toString(bodyString.size());
			msg.addAttribute(HttpMessage::Attribute("content-length", sizeString));
			msg.addBody(bodyString);
		}
		else
		{	size_t numBytes = 0;
			size_t pos = msg.findAttribute("content-length");
			if (pos < msg.attributes().size())
			{
				numBytes = Converter<size_t>::toValue(msg.attributes()[pos].second);
				Socket::byte* buffer = new Socket::byte[numBytes + 1];
				socket.recv(numBytes, buffer);
				buffer[numBytes] = '\0';
				std::string msgBody(buffer);
				msg.addBody(msgBody);
				delete[] buffer;
			}
		}
	}
	return msg;
}

//----< read a binary file from socket and save >--------------------
/*
* This function expects the sender to have already send a file message,
* and when this function is running, continuosly send bytes until
* fileSize bytes have been sent.
*/


bool ServerHandler::readFile(const std::string& filename, size_t fileSize, Socket& socket, std::string category)
{
	std::string fqname = "DownloadFiles/"+ category + "/" +filename ;
	FileSystem::File file(fqname);
	file.open(FileSystem::File::out, FileSystem::File::binary);
	if (!file.isGood())
	{
		Show::write("\n\n  can't open file " + fqname);
		return false;
	}

	const size_t BlockSize = 2048;
	Socket::byte buffer[BlockSize];

	size_t bytesToRead;
	while (true)
	{
		if (fileSize > BlockSize)
			bytesToRead = BlockSize;
		else
			bytesToRead = fileSize;

		socket.recv(bytesToRead, buffer);

		FileSystem::Block blk;
		for (size_t i = 0; i < bytesToRead; ++i)
			blk.push_back(buffer[i]);

		file.putBlock(blk);
		if (fileSize < BlockSize)
			break;
		fileSize -= BlockSize;
	}
	file.close();
	return true;
}
//----< receiver functionality is defined by this function >---------

void ServerHandler::operator()(Socket socket)
{
	while (true)
	{
		HttpMessage msg = readMessage(socket);
		if (connectionClosed_ || msg.bodyString() == "quit")
		{
			Show::write("\n\n  clienthandler thread is terminating");
			break;
		}
		msgQ_.enQ(msg);
	}
}

void listenerFunction()
{

	Async::BlockingQueue<HttpMessage> msgQ;
	try
	{
		SocketSystem ss;
		SocketListener sl(8082, Socket::IP6);
		ServerHandler cp(msgQ);
		sl.start(cp);

		while (true)
		{
			HttpMessage msg = msgQ.deQ();
			
			std::cout << msg.bodyString();		// response from server

			//break if response is "upload over"

			if (!analyzeMessageFromServer(msg)) {
				break;
			}

			//Show::write("\n\n  Client Received message with body contents:\n" + msg.bodyString());
			//break;
		}
	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}
}

//-----------Returns false if msg is the last message from server. If more responses are yet to come, returns true ------
bool analyzeMessageFromServer(HttpMessage m)
{
	std::string message_body = m.bodyString();
	std::string type = m.findValue("type");
	std::string filename = m.findValue("file");

	if (message_body.find("upload over") != std::string::npos)		//end of Download option, server sends a message with body "upload over"
		return false;
	if (filename != "")		//If message is a received file
		return true;
	if (type == "display" || type == "delete" || type == "publish")
		return false;

	return false;

}



//------------------------ GUI Functions ----------------------//

std::string getCategory(int c) {
	switch (c) {
	case 1:
		return "category1";
		break;
	case 2:
		return "category2";
		break;
	case 3:
		return "category3";
		break;
	default:
		return "none";
		break;
	}
}


HttpMessage MsgClient::uploadFunction(std::string path, std::vector<std::string> files, int cat)
{
	filestoOpen = files;
	std::string category = getCategory(cat);
	std::thread t1(
		[&]() { execute(300, 1, "upload", category, path); } // 20 messages 100 millisec apart
	);
	t1.join();

	listenerFunction();

	return HttpMessage();
}

HttpMessage MsgClient::deleteFunction(int cat)
{
	std::string category = getCategory(cat);
	std::thread t1(
		[&]() { execute(300, 1, "delete", category); } // 20 messages 100 millisec apart
	);
	t1.join();

	

	return HttpMessage();

}

HttpMessage MsgClient::publishFunction(int cat)
{
	std::string category = getCategory(cat);
	std::thread t1(
		[&]() { execute(300, 1, "publish", category); } // 20 messages 100 millisec apart
	);
	t1.join();

	return HttpMessage();

}

HttpMessage MsgClient::displayFunction(int cat)
{
	std::string category = getCategory(cat);
	std::thread t1(
		[&]() { execute(300, 1, "display", category); } // 20 messages 100 millisec apart
	);
	t1.join();

	return HttpMessage();

}

HttpMessage MsgClient::downloadFunction(int cat, std::vector<std::string> filesToOpen)
{
	std::string category = getCategory(cat);
	std::thread t1(
		[&]() { execute(300, 1, "download", category); } // 20 messages 100 millisec apart
	);
	t1.join();

	return HttpMessage();	
}







int main()
{
	::SetConsoleTitle(L"Clients Running on Threads");

	Show::title("Demonstrating two HttpMessage Clients each running on a child thread");

	MsgClient c;
	c.uploadFunction("../TestFiles", c.filestoSend,1);

	while (true) {

		//MsgClient c1;

		//std::cout << "\nClient is connected to Server";
		//std::cout << "\nEnter a Message\n";

		//std::string path, category, type;
		//std::cin >> type;
		//std::cin >> category;
		//std::cin >> path;
		//std::thread t1(
		//	[&]() { c1.execute(300, 1, type, category, path ); } // 20 messages 100 millisec apart
		//);
		//t1.join();

		//Async::BlockingQueue<HttpMessage> msgQ;
		//try
		//{
		//	SocketSystem ss;
		//	SocketListener sl(8082, Socket::IP6);
		//	ServerHandler cp(msgQ);
		//	sl.start(cp);

		//	while (true)
		//	{
		//		HttpMessage msg = msgQ.deQ();
		//		std::cout << msg.bodyString();
		//		//Show::write("\n\n  Client Received message with body contents:\n" + msg.bodyString());
		//		break;
		//	}
		//}
		//catch (std::exception& exc)
		//{
		//	Show::write("\n  Exeception caught: ");
		//	std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		//	Show::write(exMsg);
		//}

	}

	getchar();
}

