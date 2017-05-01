#pragma once
/////////////////////////////////////////////////////////////////////
//  DependencyAnalysis.h - Creates Dependency database			   //
//																   //
//  Language:      Visual C++ 2015                                 //
//  Platform:      Dell Inspiron, Windows 8.1			           //
//  Application:   Dependency Analysis - CIS 687 Project 3         //
//  Author:        Abhijit Srikanth SUID:864888072			       //
/////////////////////////////////////////////////////////////////////
/*
Module Operations:
==================

This module defines a DependencyTable class
* - The functions defined in this class are used for,
> Stores the typetable from Type_table class
> Uses this type table, and tokenizer on the whole project to detect hits
> Checks for dependencies, And stores it into Database

Private Interface:
=================
DependencyTable Class > ---- >

* std::unordered_map<Key, Value_Vector> tempTypeTable;		- Stores the populated type table
* Files filePaths;											- Stores all the .cpp, .h and .cs files in directory
* NoSQLDB::NoSqlDb<std::string> dependency_Store;			- Database to hold The dependencies


Build Process:
==============
Required files
-AbstrSynTree.h
-ActionAndRules.h
-Executive.h
-typetable.h
-Tokenizer.h
-FileSystem.h
-NoSqlDb.h

Build commands
- devenv Project3_OOD.sln

Maintenance History:
====================
ver 1.0 : 04 Apr 17
- first release
*/

#include <string>
#include <unordered_map>
#include <iostream>
#include <vector>
#include <string>

#include "../AbstractSyntaxTree/AbstrSynTree.h"
#include "../Parser/ActionsAndRules.h"
#include "../Analyzer/Executive.h"
#include "../TypeTable/typetable.h"
#include "../NoSqlDb/NoSqlDb.h"
#include "../Tokenizer/Tokenizer.h"
#include "../FileSystem/FileSystem.h"

using namespace CodeAnalysis;

class DependencyTable
{
public:
	using Key = std::string;
	using Keys = std::vector<Key>;
	using Value_Vector = std::vector<Values>;
	using tableType = std::unordered_map<Key, Value_Vector>;
	using File = std::string;
	using Files = std::vector<File>;

	DependencyTable();
	DependencyTable(tableType temp, Files allFiles);

	void setTypeTable(tableType t);
	std::string dependencyMain(std::string path);
	bool TokensInFile(File eachFile);
	bool checkIfDependent(std::string token);
	bool storeIntoDep(std::string token, File dep_file);
	//void displayDependency();
	std::vector<std::string> getAllFiles();
	NoSQLDB::NoSqlDb<std::string>& getDb();
	std::string returnXML(std::string path);
	std::unordered_map<std::string, std::string> funcFile2Path();
	void cppAndHeaderDependency();
	void showDep();

private:
	std::unordered_map<Key, Value_Vector> tempTypeTable;
	Files filePaths;
	NoSQLDB::NoSqlDb<std::string> dependency_Store;
	std::unordered_map<std::string, std::string> file2PathMap;

};


