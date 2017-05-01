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
    // is this a file message?

    std::string filename = msg.findValue("file");
    if (filename != "")
    {
      size_t contentSize;
      std::string sizeString = msg.findValue("content-length");
      if (sizeString != "")
        contentSize = Converter<size_t>::toValue(sizeString);
      else
        return msg;

      readFile(filename, contentSize, socket);
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
bool ClientHandler::readFile(const std::string& filename, size_t fileSize, Socket& socket)
{
  std::string fqname = "../TestFiles/" + filename ;
  FileSystem::File file(fqname);
  file.open(FileSystem::File::out, FileSystem::File::binary);
  if (!file.isGood())
  {
    /*
     * This error handling is incomplete.  The client will continue
     * to send bytes, but if the file can't be opened, then the server
     * doesn't gracefully collect and dump them as it should.  That's
     * an exercise left for students.
     */
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

void ClientHandler::operator()(Socket socket)
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




//------starting Msg Server-------------


//----< factory for creating messages >------------------------------
/*
* This function only creates one type of message for this demo.
* - To do that the first argument is 1, e.g., index for the type of message to create.
* - The body may be an empty string.
* - EndPoints are strings of the form ip:port, e.g., localhost:8081. This argument
*   expects the receiver EndPoint for the toAddr attribute.
*/
HttpMessage MsgServer::makeMessage(size_t n, const std::string& body, const EndPoint& ep)
{
	HttpMessage msg;
	HttpMessage::Attribute attrib;
	EndPoint myEndPoint = "localhost:8081";  // ToDo: make this a member of the sender
											 // given to its constructor.
	switch (n)
	{
	case 1:
		msg.clear();
		msg.addAttribute(HttpMessage::attribute("POST", "Message"));
		msg.addAttribute(HttpMessage::Attribute("mode", "oneway"));
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


//----< send file using socket >-------------------------------------
/*
* - Sends a message to tell receiver a file is coming.
* - Then sends a stream of bytes until the entire file
*   has been sent.
* - Sends in binary mode which works for either text or binary.
*/
bool MsgServer::sendFile(const std::string& filename, Socket& socket)
{
	// assumes that socket is connected

	std::string fqname = "../TestFiles/" + filename;
	FileSystem::FileInfo fi(fqname);
	size_t fileSize = fi.size();
	std::string sizeString = Converter<size_t>::toString(fileSize);
	FileSystem::File file(fqname);
	file.open(FileSystem::File::in, FileSystem::File::binary);
	if (!file.isGood())
		return false;

	HttpMessage msg = makeMessage(1, "", "localhost::8082");
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

void MsgServer::execute(const size_t TimeBetweenMessages, const size_t NumMessages)
{
	// send NumMessages messages

	ServerCounter counter;
	size_t myCount = counter.count();
	std::string myCountString = Utilities::Converter<size_t>::toString(myCount);

	Show::attach(&std::cout);
	Show::start();

	Show::title(
		"Starting HttpMessage client" + myCountString +
		" on thread " + Utilities::Converter<std::thread::id>::toString(std::this_thread::get_id())
	);
	try
	{
		SocketSystem ss;
		SocketConnecter si;
		while (!si.connect("localhost", 8082))
		{
			Show::write("\n client waiting to connect");
			::Sleep(100);
		}

		// send a set of messages

		HttpMessage msg;

		try {

			for (size_t i = 0; i < NumMessages; ++i)
			{
				std::string msgBody =
					"<msg>Message #" + Converter<size_t>::toString(i + 1) +
					" from Server #" + myCountString + "</msg>";
				msg = makeMessage(1, msgBody, "localhost:8082");
				/*
				* Sender class will need to accept messages from an input queue
				* and examine the toAddr attribute to see if a new connection
				* is needed.  If so, it would either close the existing connection
				* or save it in a map[url] = socket, then open a new connection.
				*/
				sendMessage(msg, si);
				Show::write("\n\n  client" + myCountString + " sent\n" + msg.toIndentedString());
				::Sleep(TimeBetweenMessages);
			}
		}
		catch (std::exception& exc)
		{
			Show::write("\n  Exeception caught: ");
			std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
			Show::write(exMsg);
		}
		//  send all *.cpp files from TestFiles folder

		//std::vector<std::string> files = FileSystem::Directory::getFiles("../TestFiles", "*.cpp");
		////std::vector<std::string> files = FileSystem::Directory::getFiles(directory, "*.h");
		//for (size_t i = 0; i < files.size(); ++i)
		//{
		//	Show::write("\n\n  sending file " + files[i]);
		//	sendFile(files[i], si);
		//}

		// shut down server's client handler

		msg = makeMessage(1, "quit", "toAddr:localhost:8082");
		sendMessage(msg, si);
		Show::write("\n\n  client" + myCountString + " sent\n" + msg.toIndentedString());

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













int main()
{

	::SetConsoleTitle(L"HttpMessage Server - Runs Forever");

	/*Show::attach(&std::cout);
	Show::start();
	Show::title("\n  HttpMessage Server started");
*/
	MsgServer c1;
	std::thread t1(
		[&]() { c1.execute(1000, 5); } // 20 messages 100 millisec apart
	);

	t1.join();

	
	Async::BlockingQueue<HttpMessage> msgQ;

	try
	{
		SocketSystem ss;
		SocketListener sl(8081, Socket::IP6);
		ClientHandler cp(msgQ);
		sl.start(cp);
		while (true)
		{
			HttpMessage msg = msgQ.deQ();
			std::cout << msg.bodyString();
			Show::write("\n\n  server recvd message with body contents:\n" + msg.bodyString());
		}
	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}
	//getchar();

}
