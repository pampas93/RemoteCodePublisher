#pragma warning(disable: 4221)
/////////////////////////////////////////////////////////////////////
//  HtmlBuilder.cpp - Webpage CodePublisher					       //
//																   //
//  Language:      Visual C++ 2015                                 //
//  Platform:      Dell Inspiron, Windows 8.1			           //
//  Application:   Dependency Analysis - CIS 687 Project 4         //
//  Author:        Abhijit Srikanth SUID:864888072			       //
//////////////////////////////////////////////////////////////////////*
//* This package implements a client that sends HTTP style messages and
//* files to a server that simply displays messages and stores files.
//*
//* It's purpose is to provide a very simple illustration of how to use
//* the Socket Package provided for Project #4.
//*/
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


#include "MsgClient.h"
#include <shellapi.h>
#pragma comment(lib,"shell32.lib") 
size_t ClientCounter::clientCount = 0;


//----< factory for creating messages >------------------------------
/*
* This function only creates one type of message for this demo.
* - To do that the first argument is 1, e.g., index for the type of message to create.
* - The body may be an empty string.
* - EndPoints are strings of the form ip:port, e.g., localhost:8081. This argument
*   expects the receiver EndPoint for the toAddr attribute.
*/
//---- Make HTTP style message----------------------------------
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
	try
	{
		SocketSystem ss;
		SocketConnecter si;
		while (!si.connect("localhost", 8080))
		{
			//Show::write("\n client waiting to connect");
			::Sleep(100);
		}
		HttpMessage msg;

		if (type == "upload") {
			msg = makeMessage(1, "uploading body; not using anywhere;", "localhost:8080", category, type);
			sendMessage(msg, si);
			sendFiles2Server(si, category);
		}
		else if (type == "delete" || type == "publish" || type == "display" || type == "download") {
			msg = makeMessage(1, type +" message", "localhost:8080", category, type);
			sendMessage(msg, si);
		}
		else if (type == "downloadlazy") {
			msg = makeMessage(1, lazyFile, "localhost:8080", category, type);
			sendMessage(msg, si);
		}

		/*Show::write("\n");
		Show::write("\n  All done folks");*/
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
	std::vector<std::string> filestoSend = split(filesFromGUI, ',');
//	std::vector<std::string> files = FileSystem::Directory::getFiles("../TestFiles", "*.cpp");
	for (size_t i = 0; i < filestoSend.size(); ++i)
	{
		Show::write("\n\n  sending file " + filestoSend[i]);
		sendFile(filestoSend[i], socket, category);
	}
	//std::vector<std::string> filesx = FileSystem::Directory::getFiles("../TestFiles", "*.h");
	/*for (size_t i = 0; i < filestoSend.size(); ++i)
	{
		Show::write("\n\n  sending file " + filestoSend[i]);
		sendFile(filestoSend[i], socket, category);
	}*/

	HttpMessage m;															//Message sent to tell server upload is over
	m = makeMessage(1, "upload over", "localhost:8080", category, "upload");
	sendMessage(m, socket);
}

//----------- Slpit function (based on delimiter) ------
std::vector<std::string> MsgClient::split(const std::string &s, char delim) {
	
		std::stringstream ss;
		ss.str(s);
		std::string item;
		std::vector<std::string> elems;
		while (std::getline(ss, item, delim)) {
			elems.push_back(item);
		}
		return elems;

}



//----------------Server Handler Functions----------------------------------------------------

//----------- Read Message function ------
HttpMessage ServerHandler::readMessage(Socket& socket) {
	connectionClosed_ = false;
	HttpMessage msg;
	while (true) {
		std::string attribString = socket.recvString('\n');
		if (attribString.size() > 1) {
			HttpMessage::Attribute attrib = HttpMessage::parseAttribute(attribString);
			msg.addAttribute(attrib);
		}
		else
			break;
	}
	if (msg.attributes().size() == 0) {
		connectionClosed_ = true;
		return msg;
	}
	if (msg.attributes()[0].first == "POST") {
		std::string category = msg.findValue("category");
		std::string filename = msg.findValue("file");
		if (filename != "") {
			size_t contentSize;
			std::string sizeString = msg.findValue("content-length");
			if (sizeString != "")
				contentSize = Converter<size_t>::toValue(sizeString);
			else
				return msg;
			readFile(filename, contentSize, socket, category);
		}
		if (filename != "") {
			msg.removeAttribute("content-length");
			std::string bodyString = "<file>" + filename + "</file>";
			std::string sizeString = Converter<size_t>::toString(bodyString.size());
			msg.addAttribute(HttpMessage::Attribute("content-length", sizeString));
			msg.addBody(bodyString);
		}
		else {
			size_t numBytes = 0;
			size_t pos = msg.findAttribute("content-length");
			if (pos < msg.attributes().size()) {
				numBytes = Converter<size_t>::toValue(msg.attributes()[pos].second);
				Socket::byte* buffer = new Socket::byte[numBytes + 1];
				socket.recv(numBytes, buffer);
				buffer[numBytes] = '\0';
				std::string msgBody(buffer);
				msg.addBody(msgBody);
				delete[] buffer;
		} }	}
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
	std::string fqname = "../MsgClient/DownloadFiles/"+ category + "/" +filename ;
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


HttpMessage MsgClient::listenerFunction()
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
			//std::cout << msg.bodyString();
			
			std::cout << "\nResponse from server:     ";
			std::cout << msg.bodyString() << "\n";		// response from server
			if (!analyzeMessageFromServer(msg)) {
				sl.close();
				return msg;
				break;
			}
		}
		return HttpMessage();
	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}
	return HttpMessage();
}

//-----------Returns false if msg is the last message from server. If more responses are yet to come, returns true ------
bool MsgClient::analyzeMessageFromServer(HttpMessage m)
{
	std::string message_body = m.bodyString();
	std::string type = m.findValue("type");
	std::string filename = m.findValue("file");

	if (message_body.find("Download over") != std::string::npos)		//end of Download option, server sends a message with body "upload over"
		return false;
	if (type == "display" || type == "delete" || type == "publish" || type == "upload")
		return false;

	return true;
}



//------------------------ GUI Functions ----------------------//

std::string MsgClient::getCategory(int c) {
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

//----------- Executed when Upload button clicked on GUI (Upload functionality) ------
std::string MsgClient::uploadFunction(int cat, std::string files)
{
	
		//filestoOpen = files;
		filesFromGUI = files;
		std::string category = getCategory(cat);
		std::thread t1(
			[&]() { execute(200, 1, "upload", category, files); }
		);
		t1.join();

		HttpMessage x = listenerFunction();
		//std::cout << "\n\n" << x.bodyString();
		return x.bodyString();


}

//----------- Executed when Delete button clicked on GUI (delete functionality) ------
std::string MsgClient::deleteFunction(int cat)
{
	std::string category = getCategory(cat);
	std::thread t1(
		[&]() { execute(200, 1, "delete", category); } 
	);
	t1.join();

	HttpMessage x = listenerFunction();
	//std::cout << "\n\n" << x.bodyString();
	return x.bodyString();

}

//----------- Executed when Publish button clicked on GUI (Publish functionality) ------
std::string MsgClient::publishFunction(int cat)
{
	std::string category = getCategory(cat);
	std::thread t1(
		[&]() { execute(200, 1, "publish", category); } 
	);
	t1.join();

	HttpMessage x = listenerFunction();
	//std::cout << "\n\n" << x.bodyString();
	return x.bodyString();
}

//----------- Executed when Display button clicked on GUI (Display functionality) ------
std::string MsgClient::displayFunction(int cat)
{
	std::string category = getCategory(cat);
	std::thread t1(
		[&]() { execute(300, 1, "display", category); } 
	);
	t1.join();

	HttpMessage x = listenerFunction();
	std::cout << "\n\nList of Files in category\n\n" << x.bodyString();
	return x.bodyString();;

}

//----------- Executed when Download button clicked on GUI (All Download functionality) ------
std::string MsgClient::downloadFunction(int cat, std::string filesToOpen)
{
	std::string category = getCategory(cat);
	std::thread t1(
		[&]() { execute(300, 1, "download", category); } 
	);
	t1.join();
	HttpMessage x = listenerFunction();
	//std::cout << "\n\n" << x.bodyString();
	return x.bodyString();
}

//----------- Executed when Download button clicked on GUI (Lazy download functionality) ------
std::string MsgClient::downloadLazyFunction(int cat, std::string file)
{
	lazyFile = file;
	std::string category = getCategory(cat);
	std::thread t1(
		[&]() { execute(300, 1, "downloadlazy", category); }
	);
	t1.join();

	HttpMessage x = listenerFunction();
	//std::cout << "\n\n" << x.bodyString();
	openBrowser("../MsgClient/DownloadFiles/" + category + "/" + file);

	return x.bodyString();

}

//----------- Test Executive ------
void MsgClient::testExecutive()
{
	std::cout << "\n\n****************************** Unit Test Executive *************************** \n";
	std::cout << "*********** (Requirement 9) ********** \n";
	std::cout << "\n************** (Requirement 1) - Using Visual Studio 2015 and its C++ Windows Console Projects ********** \n";
	std::cout << "\n************** (Requirement 1) -  Using WPF to provide a required client GUI ***************** \n";
	std::cout << "\n************** (Requirement 2) - Using C++ standard library streams and managing heap based memory with new & delete ********** \n";
	std::cout << "\n************** (Requirement 3) - Provided a Publish Button on GUI with Linking Webpages ********** \n";
	std::cout << "\n ------ Implemented Code publisher on Uploaded files(in the specified category)\n ------ Implemented Lazy Download\n ---------- Click Display after Publish\n ---------- Select file and click download\n ---------- Dependent files and its dependent and so on, are downloaded\n ---------- Selected File opens on Chrome\n ";
	std::cout << "\n*******( Uploaded XMLDocument files, Published it, and Lzy downloaded XmlElement.h.html )*******\n";
	std::cout << "\n************** (Requirement 4) - Requirements of Code Publisher (Project3) are satisfied ***************** \n";
	std::cout << "\n************** (Requirement 5) - Can Upload, View repository published content on the Client GUI ***************** \n";
	std::cout << "\n************** (Requirement 6) - Message-passing communication system is used, for Client to access Repository ***************** \n";
	std::cout << "\n************** (Requirement 7) - Http style messages used for communication between Clients and Servers  ***************** \n";
	std::cout << "\n******** (Requirement 8) - Communication system supports File transfer between Client and Server trough Stream Bytes  ********* \n";
	std::cout << "\n************** (Requirement 10) - !!Bonus!! Lazy Download Implemented  ***************** \n";
	std::cout << "\n ------ Once published, select file and click Download. (On GUI)\n\n\n";
	std::cout << "\n\n ------ Other Project Requirements satisfied\n-> HTTP style messages, File stream transmission and Asynchronous messaging";
	std::cout << "\n->Building dependency relationships, HTML file creation\n->Accept Source codes, and Delete stored files";
	std::cout << "\n->Categories used, Display of all published files, and files with no Parent\n->Download webpages, with CSS Stylesheet and JS scripts\n";
	std::cout << "\n************** Functionalities of Clients are, Upload, delete, display, publish and download   ***************** \n\n";
	std::cout << "\n ++++ Upload Functionality -- Files of XML Document Directory ****\n";
	std::cout << "----Using Category 1 now. (Application can support 3 categories)";
	try {
		std::string toUpload = "../TestFiles/itokcollection.h,../TestFiles/Tokenizer.cpp,../TestFiles/Utilities.cpp,../TestFiles/Utilities.h,../TestFiles/Tokenizer.h,../TestFiles/XmlDocument.cpp,../TestFiles/XmlDocument.h,";
		toUpload.append("../TestFiles/XmlElement.cpp,../TestFiles/XmlElement.h,../TestFiles/XmlElementParts.cpp,../TestFiles/XmlElementParts.h,../TestFiles/XmlParser.cpp,../TestFiles/XmlParser.h");
		uploadFunction(1, toUpload);
		/*std::cout << "\ @@ Display Functionality ***** \n";
		displayFunction(1);*/
		std::cout << "\n ++++ Delete Functionality ***** \n";
		deleteFunction(1);
		std::cout << "\n ++++ Upload again (To demonstrate Publish and Download ) ***** \n";
		uploadFunction(1, toUpload);
		std::cout << "\n ++++ Publish Functionality (View Server console for Processing and dependency analysis) ***** \n";
		std::cout << "---- Also Displaying files with no parents\n";
		publishFunction(1);
		::Sleep(15000);
		std::cout << "\n ++++ Display Functionality ***** \n";
		displayFunction(1);
		std::cout << "\n\n\n ++++ (Lazy)Download Functionality (Selected XmlElement.h.html to download)***** \n";
		downloadLazyFunction(1, "XmlElement.h.html");
	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}
}

//------- Function to open browser----
void MsgClient::openBrowser(std::string fpath)
{
	std::string f = "file:///" + FileSystem::Path::getFullFileSpec(fpath);
	std::wstring ff = std::wstring(f.begin(), f.end());
	LPCWSTR lpcff = ff.c_str();
	LPCWSTR a = L"open";
	LPCWSTR browser = L"chrome.exe";
	ShellExecute(NULL, a, browser, lpcff, NULL, SW_SHOWDEFAULT);

}




#ifdef TEST_CLIENTONE

int main()
{
	::SetConsoleTitle(L"Clients Running on Threads");

	//Show::title("Demonstrating two HttpMessage Clients each running on a child thread");

	MsgClient c;
	//c.deleteFunction(2);
	
	//c.uploadFunction("../TestFiles",1);
	c.publishFunction(1);
	//c.displayFunction(1);
	c.downloadLazyFunction(1, "XmlElement.h.html");
	     
	while (true) {

		//MsgClient c1;

		//std::cout << "\nClient is connected to Server";
	
	}

	getchar();
}

#endif

