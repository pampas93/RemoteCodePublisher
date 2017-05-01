#pragma once
/////////////////////////////////////////////////////////////////////
//  HtmlBuilder.cpp - Webpage CodePublisher					       //
//																   //
//  Language:      Visual C++ 2015                                 //
//  Platform:      Dell Inspiron, Windows 8.1			           //
//  Application:   Dependency Analysis - CIS 687 Project 3         //
//  Author:        Abhijit Srikanth SUID:864888072			       //
/////////////////////////////////////////////////////////////////////
/*
Module Operations:
==================

This module defines a htmlClass class
* - The functions defined in this class are used for,
> Creating a Directory to store the created web pages
> Creating html page dynamically and copying the files' source code
> Create index page
> Adding dependency links in each webpage


Private Interface:
=================
htmlClass > ---- >

* std::string directoryPath;									- Stores the Directory path
* NoSQLDB::NoSqlDb<std::string> dp_Store;						- Dependency table
* std::unordered_map<std::string, std::string> file2Path;		- Maps file name to its path
* std::stack<char> myStack;										- Used to keep track of open braces
* std::unordered_map<File, Path> htmlFile2Path;					- Maps html file name to its path
* std::unordered_map < std::string, lineVector> lineMap;		- Map which holds line number of opening closing of braces
* int lineNumber = 0;											- Keeping track of line number

Build Process:
==============
Required files
-Executive.h
-DependencyAnalyzer.h
-FileSystem.h
-NoSqlDb.h

Build commands
- devenv Project3_OOD.sln

Maintenance History:
====================
ver 1.0 : 04 Apr 17
- first release
*/

#include <iostream>
#include <locale>
#include <string>
#include <vector>
#include <iostream>
#include <ostream>
#include <fstream>
#include <direct.h>

#include "../Analyzer/Executive.h"
#include "../DependencyAnalyzer/DependencyAnalysis.h"
#include "../NoSqlDb/NoSqlDb.h"
#include "../FileSystem/FileSystem.h"

class htmlClass
{
public:
	using Path = std::string;
	using File = std::string;
	using Item = std::pair<File, Path>;
	using lineVector = std::vector<std::vector<int>>;

	Path htmlMainClass(DependencyTable dp, std::string dirPath, std::unordered_map < std::string, lineVector> linemap);
	void addFilesToDir();
	std::string copyFile(Path path1);
	std::string replaceEscSeq(std::string str, std::string path);
	void createHTMLfile(Path htmlPath, Path sourcePath);
	std::string returnDependencyHRef(Path sourcePath);
	std::string htmlPrologues(std::string fn);
	std::string indexPage();
	bool isOpenBracePresent(std::string f);
	bool isCloseBracePresent(std::string f);
	~htmlClass();


private:
	std::string directoryPath;
	NoSQLDB::NoSqlDb<std::string> dp_Store;
	std::unordered_map<std::string, std::string> file2Path;
	std::stack<char> myStack;
	std::unordered_map<File, Path> htmlFile2Path;
	std::unordered_map < std::string, lineVector> lineMap;
	int lineNumber = 1;
};