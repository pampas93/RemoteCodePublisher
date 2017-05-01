/////////////////////////////////////////////////////////////////////
//  HtmlBuilder.cpp - Webpage CodePublisher					       //
//																   //
//  Language:      Visual C++ 2015                                 //
//  Platform:      Dell Inspiron, Windows 8.1			           //
//  Application:   Dependency Analysis - CIS 687 Project 3         //
//  Author:        Abhijit Srikanth SUID:864888072			       //
/////////////////////////////////////////////////////////////////////

#include "htmlBuilder.h"

using namespace CodeAnalysis;

//<---------------------- Main Function to begin the CodePublisher ------------------->
std::string htmlClass::htmlMainClass(DependencyTable dp, std::string dirPath, std::unordered_map < std::string, lineVector> linemap)
{
	lineMap = linemap;
	std::string indexFile;
	try {
		directoryPath = dirPath;
		const char *c = directoryPath.c_str();
		_mkdir(c);
		file2Path = dp.funcFile2Path();
		dp_Store = dp.getDb();				//dp_store is a noSql db

		addFilesToDir();
		indexFile = indexPage();

	}
	catch (const std::exception& e)
	{
		std::cout << e.what();
	}
	return indexFile;
}

//<---------------------- Function to add Files into new Directory ------------------->
void htmlClass::addFilesToDir()
{
	std::string newPath;
	for (Item item : file2Path)
	{
		newPath = directoryPath +"/"+ item.first + ".html";
		createHTMLfile(newPath, item.second);
		std::string f = FileSystem::Path::getName(newPath);
		newPath = "file:///" + FileSystem::Path::getFullFileSpec(newPath);
		htmlFile2Path[f] = newPath;
	}
	
}

//<---------------------- Function to copy source code into html file ------------------->
std::string htmlClass::copyFile(Path path1)
{
	lineNumber = 1;
	std::ifstream document1(path1);
	//std::ofstream document2(path2);
	std::string sourceCode;
	try {
		
		std::string str;
		while (getline(document1, str))
		{
			sourceCode.append(replaceEscSeq(str, path1));
			sourceCode.append("\n");
			lineNumber++;
			//document2 << replaceEscSeq(str) << std::endl;
		}
		document1.close();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what();
	}
	//	document2.close();
	return sourceCode;
}

//<---------------------- Function to check and replace escapse sequence characters ------------------->
std::string htmlClass::replaceEscSeq(std::string str, std::string path)
{
	bool flag = true;
	std::string temp;
	for (char a : str)
	{
		if (a == '<')
			temp.append("&lt;");
		else if (a == '>')
			temp.append("&gt;");
		else if (a == '{') {
			if (isOpenBracePresent(path)) {
				temp.append("<div class=\"header\">-  {</div>");
				temp.append("<div class=\"content\">");
				myStack.push('{');
			}
			else
				temp += a;
		}
		else if (a == '}') {
			if (isCloseBracePresent(path)) {
				temp.append("</div>}");
			}
			else
				temp += a;
		}
		else
			temp += a;
	}
	return temp;
}

//<---------------------- Function to create html file and add the required tags ------------------->
void htmlClass::createHTMLfile(Path htmlPath, Path sourcePath)
{
	try {
		std::string openHtmlTags;
		openHtmlTags = "<html>\n<head>\n<link rel = \"stylesheet\"type = \"text/css\"href = \"../CodePublishHTML/myStyle.css\" /></head>\n<body class = \"indent\">\n<script type=\"text/javascript\" src=\"../CodePublishHTML/jquery-2.2.4.js\"></script>\n<script type=\"text/javascript\" src=\"../CodePublishHTML/myJS.js\"></script>";
		std::string closeHtmlTags;
		closeHtmlTags = "\n</body>\n</html>";

		std::ofstream doc(htmlPath);
		doc << htmlPrologues(FileSystem::Path::getName(sourcePath));
		doc << openHtmlTags;

		doc << "<h3>" << FileSystem::Path::getName(sourcePath) << "</h3>";

		doc << "\n<hr><div class = \"indent\"><h4>" << "Dependencies :" << "</h4>";
		std::string deplist = returnDependencyHRef(sourcePath);

		doc << "<ul>" << deplist << "\n</ul></div><hr>";

		std::string sourceCode = copyFile(sourcePath);
		doc << std::endl;
		doc << "<pre>\n" << sourceCode << "</pre>";

		doc << closeHtmlTags;
		doc.close();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what();
	}
}

//<---------------------- Function that returns a html text of dependencies of a file ------------------->
std::string htmlClass::returnDependencyHRef(Path sourcePath)
{
	std::string htmlDepTags;
	NoSQLDB::Element<std::string> depList = dp_Store.value(sourcePath);
	for (File child : depList.dependencies.getValue())
	{
		std::string filename = FileSystem::Path::getName(child);
		std::string link = directoryPath + "/" +filename+ ".html";
		htmlDepTags.append("<li> <a href=");
		htmlDepTags.append("\""+link + "\">");
		htmlDepTags.append(filename + "</a> </li>");
	}

	return htmlDepTags;
}

//<---------------------- Function to check if { is inside comment ------------------->

//<---------------------- Function to add Prologues into html file ------------------->
std::string htmlClass::htmlPrologues(std::string fn)
{
	std::string prologue;
	prologue = "<!----------------------------------------------------------------------------\n" + fn+" Help file for Project #3\n Published 4 Apr 2017\n Abhijit Srikanth, CSE687 - Object Oriented Design, Spring 2017\n";
	prologue.append("\n The below file is the webpage of the source code for " + fn + " and holds links to its dependent files\n");
	prologue.append("\n Note -Do not replace any markup characters with escape sequences\n");
	prologue.append("----------------------------------------------------------------------------->\n");
	return prologue;
}

//<---------------------- Function to create index page ------------------->
std::string htmlClass::indexPage()
{
	std::string openHtmlTags = "<html>\n<head>\n<link rel = \"stylesheet\"type = \"text/css\"href = \"../CodePublishHTML/myStyle.css\" /></head>\n<body class = \"indent\">\n<script type=\"text/javascript\" src=\"../CodePublishHTML/jquery-2.2.4.js\"></script>\n<script type=\"text/javascript\" src=\"../CodePublishHTML/myJS.js\"></script>";
	std::string closeHtmlTags = "\n</body>\n</html>";
	std::string prologue;
	prologue = "<!----------------------------------------------------------------------------\n IndexPage.html -  Index page for webPages published for Project #3\n Published 4 Apr 2017\n Abhijit Srikanth, CSE687 - Object Oriented Design, Spring 2017\n";
	prologue.append("\n The below file is the webpage of listing all the links to source codes (published as webpages) \n");
	prologue.append("----------------------------------------------------------------------------->");

	std::string index = directoryPath + "/indexPage.html";
	try {
		std::ofstream doc(index);
		doc << prologue;
		doc << openHtmlTags;

		doc << "<h2>" << "Index Page" << "</h2>";

		std::string list;
		list.append("<ul>");

		for (auto item : htmlFile2Path) {
			list.append("<li> <a href=");
			list.append("\"" + item.second + "\">");
			list.append("<h3>" + item.first + "</h3></a> </li>\n");
		}

		doc << list;
		doc << closeHtmlTags;
		doc.close();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what();
	}
	return index;
}

//<---------------------- Function to check if Open brace and line number matches ------------------->
bool htmlClass::isOpenBracePresent(std::string f)
{
	lineVector temp = lineMap[f];
	for (auto item : temp) {
		if (item.at(0) == lineNumber)
			return true;
	}
	return false;
}

//<---------------------- Function to check if Close brace and line number matches ------------------->
bool htmlClass::isCloseBracePresent(std::string f)
{
	lineVector temp = lineMap[f];
	for (auto item : temp) {
		if (item.at(1) == lineNumber)
			return true;
	}
	return false;
}

//<---------------------- htmlClass destructor ------------------->
htmlClass::~htmlClass()
{

}

#ifdef TEST_htmlBuilder

int main()
{
	using File = std::string;
	using Files = std::vector<File>;
	Files allfilePaths;

	allfilePaths.push_back("C:\\Users\\Abhijit\\Desktop\\Project2_OOD\\TestingPurpose\\TestingPurpose\\B.h");
	allfilePaths.push_back("C:\\Users\\Abhijit\\Desktop\\Project2_OOD\\TestingPurpose\\TestingPurpose\\A.h");
	allfilePaths.push_back("C:\\Users\\Abhijit\\Desktop\\Project2_OOD\\TestingPurpose\\TestingPurpose\\A.cpp");
	allfilePaths.push_back("C:\\Users\\Abhijit\\Desktop\\Project2_OOD\\TestingPurpose\\TestingPurpose\\B.cpp");

	Type_Table tb;
	tb.populateTypeTable();

	DependencyTable dp(tb.returnTypeTable(), allfilePaths);
	dp.dependencyMain();

	StronglyConnectedComps scc;
	scc.makeGraph(dp);

	Show showObj;
	showObj.showTypeTable(tb.returnTypeTable());
	showObj.showDependencyDB(dp.getDb());
	showObj.showSCC(scc.returnMatrix());
}

#endif

