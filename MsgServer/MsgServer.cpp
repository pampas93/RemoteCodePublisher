/////////////////////////////////////////////////////////////////////////
// MsgServer.cpp - Demonstrates simple one-way HTTP style messaging    //
//                 and file transfer                                   //
//                                                                     //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2016           //
// Application: OOD Project #4                                         //
// Platform:    Visual Studio 2015, Dell XPS 8900, Windows 10 pro      //
/////////////////////////////////////////////////////////////////////////
/*
* This package implements a server that receives HTTP style messages and
* files from multiple concurrent clients and simply displays the messages
* and stores files.
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
 * - pull the receiving code into a Receiver class
 * - Receiver should own a BlockingQueue, exposed through a
 *   public method:
 *     HttpMessage msg = Receiver.GetMessage()
 * - You will start the Receiver instance like this:
 *     Receiver rcvr("localhost:8080");
 *     ClientHandler ch;
 *     rcvr.start(ch);
 */


/////////////////////////////////////////////////////////////////////
// ClientHandler class
/////////////////////////////////////////////////////////////////////
// - instances of this class are passed by reference to a SocketListener
// - when the listener returns from Accept with a socket it creates an
//   instance of this class to manage communication with the client.
// - You no longer need to be careful using data members of this class
//   because each client handler thread gets its own copy of this 
//   instance so you won't get unwanted sharing.
// - I changed the SocketListener semantics to pass
//   instances of this class by value for version 5.2.
// - that means that all ClientHandlers need copy semantics.
//

#include "MsgServer.h"


//----< this defines processing to frame messages >------------------

HttpMessage ClientHandler::readMessage(Socket& socket)
{
  connectionClosed_ = false;
  HttpMessage msg;

  // read message attributes

  while (true)
  {
    std::string attribString = socket.recvString('\n');
    if (attribString.size() > 1)
    {
      HttpMessage::Attribute attrib = HttpMessage::parseAttribute(attribString);
      msg.addAttribute(attrib);
    }
    else
    {
      break;
    }
  }
  // If client is done, connection breaks and recvString returns empty string
  if (msg.attributes().size() == 0)
  {
    connectionClosed_ = true;
    return msg;
  }
  // read body if POST - all messages in this demo are POSTs

  if (msg.attributes()[0].first == "POST")
  {
    std::string filename = msg.findValue("file");
	std::string category = msg.findValue("category");
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
      // construct message body
      msg.removeAttribute("content-length");
      std::string bodyString = "<file>" + filename + "</file>";
      std::string sizeString = Converter<size_t>::toString(bodyString.size());
      msg.addAttribute(HttpMessage::Attribute("content-length", sizeString));
      msg.addBody(bodyString);
    }
    else
    {
      // read message body

      size_t numBytes = 0;
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
bool ClientHandler::readFile(const std::string& filename, size_t fileSize, Socket& socket,std::string category)
{
  std::string fqname = "ServerFiles/SourceFiles/" +category+"/"+ filename;	//Mention category also here
  FileSystem::File file(fqname);
  file.open(FileSystem::File::out, FileSystem::File::binary);
  if (!file.isGood())
  {
    std::cout<<("\n\n  can't open file " + fqname);
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

void ClientHandler::operator()(Socket socket)
{
  while (true)
  {
    HttpMessage msg = readMessage(socket);
    if (connectionClosed_ || msg.bodyString() == "quit")
    {
		std::cout << ("\n\n  clienthandler thread is terminating");
      break;
    }
    msgQ_.enQ(msg);
  }
}


//------starting Msg Server-------------


HttpMessage MsgServer::makeMessage(size_t n, const std::string& body, const EndPoint& ep, std::string category, std::string type)
{
	HttpMessage msg;
	HttpMessage::Attribute attrib;
	EndPoint myEndPoint = "localhost:8080";  // ToDo: make this a member of the sender
											 // given to its constructor.
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
void MsgServer::sendMessage(HttpMessage& msg, Socket& socket)
{
	std::string msgString = msg.toString();
	socket.send(msgString.size(), (Socket::byte*)msgString.c_str());
}

bool MsgServer::sendFile(const std::string& filename, Socket& socket, std::string category, std::string cPort)
{
	// assumes that socket is connected

	std::string fqname = "ServerFiles/PublishedFiles/" +category+ "/"+filename;
	FileSystem::FileInfo fi(fqname);
	size_t fileSize = fi.size();
	std::string sizeString = Converter<size_t>::toString(fileSize);
	FileSystem::File file(fqname);
	file.open(FileSystem::File::in, FileSystem::File::binary);
	if (!file.isGood())
		return false;

	HttpMessage msg = makeMessage(1, "", cPort, category, "download");
	std::string filenn = FileSystem::Path::getName(filename);
	msg.addAttribute(HttpMessage::Attribute("file", filenn));
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
void MsgServer::execute(const size_t TimeBetweenMessages, const size_t NumMessages, int toPort, std::string type, std::string category)
{
	// send NumMessages messages

	ServerCounter counter;
	size_t myCount = counter.count();
	std::string myCountString = Utilities::Converter<size_t>::toString(myCount);
	std::string cPort = "localhost:" + std::to_string(toPort);

	try
	{
		SocketSystem ss;
		SocketConnecter si;
		while (!si.connect("localhost", toPort))
		{
			Show::write("\n Server waiting to connect");
			::Sleep(100);
		}
		HttpMessage msg;
		if (type == "upload") {
			msg = makeMessage(1, "Upload Successful", cPort, category, type);
			sendMessage(msg, si);		}
		else if (type == "publish") {
			msg = makeMessage(1, "Publish Successful", cPort, category, type);
			sendMessage(msg, si);		}
		else if (type == "delete") {
			msg = makeMessage(1, "Delete Successful", cPort, category, type);
			sendMessage(msg, si);		}
		else if (type == "display") {
			std::string allSourceFiles = getAllhtmlFiles(category);
			msg = makeMessage(1, allSourceFiles, cPort, category, type);
			sendMessage(msg, si);
		}
		else if (type == "download") {
			msg = makeMessage(1, "Downloading into client", cPort, category, type);
			sendMessage(msg, si);
			sendFiles2Client(si, category, cPort);
		}
		
	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}
}


std::string MsgServer::getAllhtmlFiles(std::string category)
{
	std::string allFiles;
	std::vector<std::string> files = FileSystem::Directory::getFiles("ServerFiles/PublishedFiles/" + category + "/", "*.*");
	for (size_t i = 0; i < files.size(); ++i)
	{
		allFiles.append(files[i]);
		allFiles.append("\n");
	}
	return allFiles;
}

void MsgServer::sendFiles2Client(Socket & socket, std::string category, std::string cPort)
{
	std::vector<std::string> files = FileSystem::Directory::getFiles("ServerFiles/PublishedFiles/"+category+"/","*.*");
	for (size_t i = 0; i < files.size(); ++i)
	{
		Show::write("\n\n  Downloading file " + files[i]);
		sendFile(files[i], socket, category, cPort);
	}

	HttpMessage m;															//Message sent to tell server upload is over
	m = makeMessage(1, "Download over", cPort, category, "upload");
	sendMessage(m, socket);

}




//------------------- Analyse received message functions below --------


//---------- Finds and returns from Port in Msg ------ 
int ClientHandler::findFromPort(HttpMessage msg)
{
	//size_t port;
	std::string temp = msg.findValue("fromAddr").substr(10, 4);
	int x = atoi(temp.c_str());

	return x;
}

void ClientHandler::analyseMsgContent(HttpMessage msg)
{
	std::string type = msg.findValue("type");
	std::string body = msg.bodyString();
	std::string category = msg.findValue("category");
	size_t fromPort = findFromPort(msg);

	MsgServer c1;

//	Call each Upload or delete or other type functions inside this; and call execute inside that function
	if (type == "upload" ) {
		//Done!
		if (body.find("upload over") != std::string::npos) {
			std::thread t1(	[&]() { c1.execute(100, 1, fromPort, type, category); } );
			t1.join();
		}
		return;
		//need to send message now, saying successful
		//&& (body.find("upload over") != std::string::npos inside that function
	}
	else if (type == "delete" ) {

		deleteCategory(category);
		std::thread t1(	[&]() { c1.execute(100, 1, fromPort, type, category); } );
		t1.join();
		
	}
	else if (type == "display") {

		std::thread t1([&]() { c1.execute(100, 1, fromPort, type, category); });
		t1.join();

	}
	else if (type == "publish") {
		publishCategory(category);
		std::thread t1(	[&]() { c1.execute(100, 1, fromPort, type, category); } );
		t1.join();
		//need to send message now, saying successful

	}
	else if (type == "download") {
		//downloadCategory(category);
		std::thread t1([&]() { c1.execute(100, 1, fromPort, type, category); });
		t1.join();
		//need to send message now, saying successful

	}
}

bool ClientHandler::deleteCategory(std::string category)
{
	std::vector<std::string> files = FileSystem::Directory::getFiles("ServerFiles/SourceFiles/"+category+"/", "*.*");
	for (size_t i = 0; i < files.size(); ++i)
	{
		std::string fileFullPath = "ServerFiles/SourceFiles/" + category + "/" + files[i];
		std::remove(fileFullPath.c_str());
	}
	std::vector<std::string> htmlfiles = FileSystem::Directory::getFiles("ServerFiles/PublishedFiles/" + category + "/", "*.html");
	for (size_t i = 0; i < htmlfiles.size(); ++i)
	{
		std::string fileFullPath = "ServerFiles/PublishedFiles/" + category + "/" + htmlfiles[i];
		std::remove(fileFullPath.c_str());
	}
	return true;
}

bool ClientHandler::publishCategory(std::string category)
{
	char * array[7];
	std::string path = "ServerFiles/SourceFiles/" + category + "/";
	std::string x[] = { "simbly",path,"*.h","*.cpp","/m","/f","/r" };
	for (int i = 0; i < 7; i++) {
		const char* xx = x[i].c_str();
		array[i] = _strdup(xx);
	}

	CodeAnalysis::CodeAnalysisExecutive exexec;
	exexec.ProcessCommandLine(7, array);

	exexec.Dependency4RemoteCodePublisher(category);
	return true;
}







int main()
{

	::SetConsoleTitle(L"HttpMessage Server - Runs Forever");

	/*Show::attach(&std::cout);
	Show::start();
	Show::title("\n  HttpMessage Server started");
*/
	//using namespace CodeAnalysis;

	CodeAnalysis::CodeAnalysisExecutive x;
	
	Async::BlockingQueue<HttpMessage> msgQ;
	MsgServer c1;

	try
	{
		SocketSystem ss;
		SocketListener sl(8080, Socket::IP6);
		ClientHandler cp(msgQ);
		sl.start(cp);
		while (true)
		{
			HttpMessage msg = msgQ.deQ();
			std::cout << "\nMessage received from Client";
			std::cout << std::endl << msg.bodyString()<<std::endl;

			cp.analyseMsgContent(msg);

			/*size_t fromPort = findFromPort(msg);
			analyseMsgContent(msg);

			//Below thread to be called in appropriate functions
			std::thread t1(
				[&]() { c1.execute(500, 1, fromPort); } // 20 messages 100 millisec apart
			);
			t1.detach();*/
		}
	}
	catch (std::exception& exc)
	{
		/*Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);*/
	}


	//MsgServer c1;
	//std::thread t1(
	//	[&]() { c1.execute(1000, 5); } // 20 messages 100 millisec apart
	//);

	//t1.join();


	//getchar();

}
