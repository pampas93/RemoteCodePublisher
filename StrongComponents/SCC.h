#pragma once
/////////////////////////////////////////////////////////////////////
//  SCC.h		 - Executed Tarjan's SCC algorithm				   //
//																   //
//  Language:      Visual C++ 2015                                 //
//  Platform:      Dell Inspiron, Windows 8.1			           //
//  Application:   Dependency Analysis - CIS 687 Project 2         //
//  Author:        Abhijit Srikanth SUID:864888072			       //
/////////////////////////////////////////////////////////////////////
/*
Module Operations:
==================

This module defines a Directed_Graph class
* - The functions defined in this class are used for,
> Mapping the files in directory to corresponding unique int values
> Create an adjacency list and populate with int value from file2IntMap
> Returns this list to StronglyConnectedComps

This module defines a StronglyConnectedComps class
* - The functions defined in this class are used for,
> Recieves ans tores the file2IntMap and adjacency list
> Tarjan's Strongly connected components algoritm is executed on the adjacency list
> Stores the strongly connected components into scc_matrix map

Private Interface:
=================
Directed_Graph Class > ---- >

* int verticesCount;											- Number of files
* std::vector<File> arrayOfFiles;								- Vector of file paths
* std::unordered_map<File,int> file2IntMap;						- Map to point each file to an int value
* std::list<int> *adjacency_matrix;								- Adjacency list required as input for SCC algorithm
* DependencyTable dpx;											- Stores the the DependencyTable object
* NoSQLDB::NoSqlDb<std::string> depTable;						- Stores the populated Dependency Table

StronglyConnectedComps Class > ---- >

* DependencyTable dpTable;										- Stores the the DependencyTable object
* Directed_Graph graph;											- Stores the Directed_Graph object
* int vertices_no;												- Number of files
* std::unordered_map<File, int> file2;							- Stores the file2IntMap from Directed_Graph
* std::list<int>* adj_matrix;									- Stores the adjacency_matrix from Directed_Graph
* std::unordered_map<int, std::vector<std::string>> scc_matrix;	- Map to store the Strongly connected components
* std::vector<std::string> temp;								- Temporary vector to store each SCC component

Build Process:
==============
Required files
-Executive.h
-typetable.h
-DependencyAnalyzer.h
-FileSystem.h
-NoSqlDb.h

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
#include <list>
#include <stack>

#include "../TypeTable/typetable.h"
#include "../NoSqlDb/NoSqlDb.h"
#include "../FileSystem/FileSystem.h"
#include "../DependencyAnalyzer/DependencyAnalysis.h"
#include "../Analyzer/Executive.h"
#include "../XmlDocument/XmlDocument/XmlDocument.h"
#include "../XmlDocument/XmlElement/XmlElement.h"


using namespace CodeAnalysis;
using namespace NoSQLDB;
using namespace XmlProcessing;

using sptr = std::shared_ptr<AbstractXmlElement>;

class Directed_Graph
{
public:
	using Key = std::string;
	using Children = std::vector<Key>;
	using Item = std::pair<Key, Element<std::string>>;
	using File = std::string;
	using Files = std::vector<File>;

	Directed_Graph();
	~Directed_Graph();
	//Directed_Graph(int vertices);
	std::unordered_map<std::string, int> createGraph(DependencyTable &dptemp);
	int makeMap();
	void makeAdjMatrix();
	//int findIndex(File a);
	std::list<int>* returnAdjMat();

private:
	int verticesCount;
	std::vector<File> arrayOfFiles;
	std::unordered_map<File,int> file2IntMap;
	std::list<int> *adjacency_matrix;
	DependencyTable dpx;
	NoSQLDB::NoSqlDb<std::string> depTable;
};


//------------------------------------------------------------------------------------------------------------//
class StronglyConnectedComps
{
public:
	using File = std::string;
	using Files = std::vector<File>;


	StronglyConnectedComps();
	//StronglyConnectedComps(DependencyTable dp);
	void makeGraph(DependencyTable dp);
	void SCC_Recursive(int u, int vss[], int scc_low[], std::stack<int> *scc_stack, bool sBool[]);
	void SCCMain();
	std::string valueInMap(int w);
	std::unordered_map<int, std::vector<std::string>>& returnMatrix();
	std::string persistXml(std::string path);
	//void SccDisplay();
	
private:
	DependencyTable dpTable;
	Directed_Graph graph;
	int vertices_no;
	std::unordered_map<File, int> file2;
	std::list<int>* adj_matrix;
	std::unordered_map<int, std::vector<std::string>> scc_matrix;
	std::vector<std::string> temp;
	int ijk = 0;

};


