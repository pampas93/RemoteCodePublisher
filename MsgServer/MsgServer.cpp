/////////////////////////////////////////////////////////////////////
//  HtmlBuilder.cpp - Webpage CodePublisher					       //
//																   //
//  Language:      Visual C++ 2015                                 //
//  Platform:      Dell Inspiron, Windows 8.1			           //
//  Application:   Dependency Analysis - CIS 687 Project 4         //
//  Author:        Abhijit Srikanth SUID:864888072			       //
/////////////////////////////////////////////////////////////////////*
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

HttpMessage ClientHandler::readMessage(Socket& socket) {
  connectionClosed_ = false;
  HttpMessage msg;
  while (true)
  {
    std::string attribString = socket.recvString('\n');
	if (attribString.size() > 1){
		HttpMessage::Attribute attrib = HttpMessage::parseAttribute(attribString);
		msg.addAttribute(attrib); }
	else
		break;
  }
  if (msg.attributes().size() == 0){
    connectionClosed_ = true;
    return msg; }
  if (msg.attributes()[0].first == "POST"){
    std::string filename = msg.findValue("file");
	std::string category = msg.findValue("category");
    if (filename != "") {
      size_t contentSize;
      std::string sizeString = msg.findValue("content-length");
      if (sizeString != "")
        contentSize = Converter<size_t>::toValue(sizeString);
      else
        return msg;

      readFile(filename, contentSize, socket, category);  }
    if (filename != ""){
      msg.removeAttribute("content-length");
      std::string bodyString = "<file>" + filename + "</file>";
      std::string sizeString = Converter<size_t>::toString(bodyString.size());
      msg.addAttribute(HttpMessage::Attribute("content-length", sizeString));
      msg.addBody(bodyString);  }
    else{
      size_t numBytes = 0;
      size_t pos = msg.findAttribute("content-length");
      if (pos < msg.attributes().size()){
        numBytes = Converter<size_t>::toValue(msg.attributes()[pos].second);
        Socket::byte* buffer = new Socket::byte[numBytes + 1];
        socket.recv(numBytes, buffer);
        buffer[numBytes] = '\0';
        std::string msgBody(buffer);
        msg.addBody(msgBody);
        delete[] buffer;
      }  }  }
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
  std::string fqname = "../MsgServer/ServerFiles/SourceFiles/" +category+"/"+ filename;	
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

//-------------------- Make HTTP style message  -----------------------
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

//-------------------- Send file to Clients-----------------------
bool MsgServer::sendFile(const std::string& filename, Socket& socket, std::string category, std::string cPort)
{

	std::string fqname = "../MsgServer/ServerFiles/PublishedFiles/" +category+ "/"+filename;
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
	ServerCounter counter;
	size_t myCount = counter.count();
	std::string myCountString = Utilities::Converter<size_t>::toString(myCount);
	std::string cPort = "localhost:" + std::to_string(toPort);
	try
	{
		SocketSystem ss;
		SocketConnecter si;
		while (!si.connect("localhost", toPort)) {
			Show::write("\n Server waiting to connect");
			::Sleep(100);
		}
		HttpMessage msg;
		if (type == "upload") {
			msg = makeMessage(1, "Upload Successful", cPort, category, type);
			sendMessage(msg, si);		}
		else if (type == "publish") {
			std::string publishBody = "Publish Successful," + noParentString;
			msg = makeMessage(1, publishBody, cPort, category, type);
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
		else if (type == "downloadlazy") {
			msg = makeMessage(1, "Lazy Downloading into client", cPort, category, type);
			sendMessage(msg, si);
			sendLazyFiles2Client(si, category, cPort);
		}
	}
	catch (std::exception& exc) {
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}
}

//-------------------- Setter for Dependency map  -----------------------
void MsgServer::setLazyVector(std::vector<std::string> x)
{
	filesForLazyD_MS = x;
}

//-------------------- Setting NoParentString -----------------------
void MsgServer::setNoParentString(std::string x)
{
	noParentString = x;
}

//-------------------- Getting all HTML files for delete -----------------------
std::string MsgServer::getAllhtmlFiles(std::string category)
{

		std::string allFiles;
		allFiles = "Display Successful,";
		std::vector<std::string> files = FileSystem::Directory::getFiles("../MsgServer/ServerFiles/PublishedFiles/" + category + "/", "*.html");
		for (size_t i = 0; i < files.size(); ++i)
		{
			if (files[i] == "indexPage.html")
				continue;
			allFiles.append(files[i]);
			allFiles.append(",");
		}
		return allFiles;
	
	
}

//-------------------- Function to send files to Client -----------------------
void MsgServer::sendFiles2Client(Socket & socket, std::string category, std::string cPort)
{
	try {
		std::vector<std::string> files = FileSystem::Directory::getFiles("../MsgServer/ServerFiles/PublishedFiles/" + category + "/", "*.*");
		for (size_t i = 0; i < files.size(); ++i)
		{
			Show::write("\n\n  Downloading file " + files[i]);
			sendFile(files[i], socket, category, cPort);
		}

		HttpMessage m;															//Message sent to tell server upload is over
		m = makeMessage(1, "Download over", cPort, category, "upload");
		sendMessage(m, socket);
	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}
}

//-------------------- Function to send files to Client (for Lazy Download) -----------------------
void MsgServer::sendLazyFiles2Client(Socket & socket, std::string category, std::string cPort)
{
	//std::vector<std::string> files = FileSystem::Directory::getFiles("ServerFiles/PublishedFiles/" + category + "/", "*.*");
	for (size_t i = 0; i < filesForLazyD_MS.size(); ++i)
	{
		Show::write("\n\n  Downloading file " + filesForLazyD_MS[i]);
		sendFile(filesForLazyD_MS[i], socket, category, cPort);
	}
	Show::write("\n\n  Downloading JS ");
	sendFile("jquery-2.2.4.js", socket, category, cPort);
	sendFile("myJS.js", socket, category, cPort);
	Show::write("\n\n  Downloading CSS ");
	sendFile("myStyle.css", socket, category, cPort);

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

//---------- Analysing received file messages ------ 
void ClientHandler::analyseMsgContent(HttpMessage msg) {
	std::string type = msg.findValue("type");
	std::string body = msg.bodyString();
	std::string category = msg.findValue("category");
	size_t fromPort = findFromPort(msg);
	MsgServer c1;
	if (type == "upload" ) {
		if (body.find("upload over") != std::string::npos) {
			std::thread t1(	[&]() { c1.execute(100, 1, fromPort, type, category); } );
			t1.join();
		}
		return; }
	else if (type == "delete" ) {
		deleteCategory(category);
		std::thread t1(	[&]() { c1.execute(100, 1, fromPort, type, category); } );
		t1.join();
		return; }
	else if (type == "display") {
		std::thread t1([&]() { c1.execute(100, 1, fromPort, type, category); });
		t1.join();
		return; }
	else if (type == "publish") {
		publishCategory(category);
		std::thread t1(	[&]() { c1.execute(100, 1, fromPort, type, category); } );
		c1.setNoParentString(noParentFiles);
		t1.join();
		return; }
	else if (type == "downloadlazy") {
		filesForLazyD_CH.clear();
		if (category == "category1")
			LazyDownloadRecursive(body, depMapCategory1);
		else if (category == "category2")
			LazyDownloadRecursive(body, depMapCategory2);
		else if (category == "category3")
			LazyDownloadRecursive(body, depMapCategory3);

		c1.setLazyVector(filesForLazyD_CH);
		std::thread t1([&]() { c1.execute(100, 1, fromPort, type, category); });
		t1.join();
		return;
	}
}

//else if (type == "download") {
//	std::thread t1([&]() { c1.execute(100, 1, fromPort, type, category); });
//	t1.join();
//	return;
//}

//---------- Function to delete category --------------- 
bool ClientHandler::deleteCategory(std::string category)
{
	std::vector<std::string> files = FileSystem::Directory::getFiles("../MsgServer/ServerFiles/SourceFiles/"+category+"/", "*.*");
	for (size_t i = 0; i < files.size(); ++i)
	{
		std::string fileFullPath = "../MsgServer/ServerFiles/SourceFiles/" + category + "/" + files[i];
		std::remove(fileFullPath.c_str());
	}
	std::vector<std::string> htmlfiles = FileSystem::Directory::getFiles("../MsgServer/ServerFiles/PublishedFiles/" + category + "/", "*.html");
	for (size_t i = 0; i < htmlfiles.size(); ++i)
	{
		std::string fileFullPath = "../MsgServer/ServerFiles/PublishedFiles/" + category + "/" + htmlfiles[i];
		std::remove(fileFullPath.c_str());
	}
	return true;
}

//---------- Function to publish category ------ 
void ClientHandler::publishCategory(std::string category)
{
	noParentFiles.clear();
	noParentFiles.append("Files with no Parent files are::\n");

	char * array[7];
	std::string path = "../MsgServer/ServerFiles/SourceFiles/" + category + "/";
	std::string x[] = { "simbly",path,"*.h","*.cpp","/m","/f","/r" };
	for (int i = 0; i < 7; i++) {
		const char* xx = x[i].c_str();
		array[i] = _strdup(xx);
	}
	using Item = std::pair<std::string, std::vector<std::string>>;
	CodeAnalysis::CodeAnalysisExecutive exexec;
	exexec.ProcessCommandLine(7, array);
	lazyMap temp = exexec.Dependency4RemoteCodePublisher(category);
	for (Item x : temp)
	{
		std::vector<std::string> depFiles = x.second;
		if (depFiles.size() == 0)
		{
			noParentFiles.append(x.first);
			noParentFiles.append("\n");
		}
	}

	if (category == "category1")
		depMapCategory1 = temp;
	else if (category == "category2")
		depMapCategory2 = temp;
	else if (category == "category3")
		depMapCategory3 = temp;

	return ;
}

//-------- For Lazy Download. file initially the selected file; mapUsed is the category based map; Recursive Function --------
void ClientHandler::LazyDownloadRecursive(std::string file, lazyMap mapUsed)
{
	if (std::find(filesForLazyD_CH.begin(), filesForLazyD_CH.end(), file) == filesForLazyD_CH.end())
	{
		filesForLazyD_CH.push_back(file);
	}
	std::vector<std::string> temp = mapUsed[file];
	for (std::string x : temp)
	{
		if (std::find(filesForLazyD_CH.begin(), filesForLazyD_CH.end(), x) == filesForLazyD_CH.end())
		{
			stackForLazy.push(x);
		}
	}
	if (stackForLazy.empty()) {
		return;
	}
	else
	{
		std::string x = stackForLazy.top();
		stackForLazy.pop();
		LazyDownloadRecursive(x, mapUsed);
	}
}

//---------- Getter for the Dependency map ------ 
std::vector<std::string> ClientHandler::returnLazyVector()
{
	return filesForLazyD_CH;
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
	//MsgServer c1;

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
		//Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		//Show::write(exMsg);
		std::cout << exMsg;
	}

}
