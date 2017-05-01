#pragma once
/////////////////////////////////////////////////////////////////////
//  Display.h     -	Main Display functions				           //
//																   //
//  Language:      Visual C++ 2015                                 //
//  Platform:      Dell Inspiron, Windows 8.1			           //
//  Application:   Dependency Analysis - CIS 687 Project 2         //
//  Author:        Abhijit Srikanth SUID:864888072			       //
/////////////////////////////////////////////////////////////////////
/*
Module Operations:
==================

This module defines a Show class
* - The functions defined in this class are used for,
> Display Type Table
> Display Dependency database
> Display Strongly connected components


Build Process:
==============
Required files
-AbstrSynTree.h
-ActionAndRules.h

Build commands
- devenv CodeAnalyzerEx.sln

Maintenance History:
====================
ver 1.0 : 07 Mar 17
- first release
*/

#include <string>
#include <unordered_map>
#include <iostream>
#include <vector>
#include <stack>
#include <unordered_set>

#include "../DependencyAnalyzer/DependencyAnalysis.h"
#include "../NoSqlDb/NoSqlDb.h"
#include "../StrongComponents/SCC.h"
#include "../TypeTable/typetable.h"

using namespace CodeAnalysis;

class Show
{
public:
	using Key = std::string;
	using Keys = std::vector<Key>;
	using File = std::string;
	using Files = std::vector<File>;

	using Value_Vector = std::vector<Values>;
	using tableType = std::unordered_map<Key, Value_Vector>;

	void showTypeTable(tableType tb_temp);
	void showDependencyDB(NoSQLDB::NoSqlDb<std::string> db_show_temp);
	void showSCC(std::unordered_map<int, std::vector<std::string>> scc_matrix_temp);
	void showXML(std::string xmlString);

};