#pragma once
/////////////////////////////////////////////////////////////////////
//  typetable.h - Creates the Type Table				           //
//																   //
//  Language:      Visual C++ 2015                                 //
//  Platform:      Dell Inspiron, Windows 8.1			           //
//  Application:   Dependency Analysis - CIS 687 Project 3         //
//  Author:        Abhijit Srikanth SUID:864888072			       //
/////////////////////////////////////////////////////////////////////
/*
Module Operations:
==================

This module defines a Type_Table class
* - The functions defined in this class are used for,
> Creates an unordered map to store the types
> Checks for Classes, structs, enums and global functions in ASTree
> Populates the unordered map
> Returns the populated type table (unordered map)

Private Interface:
=================
Type_table Class > ---- >

* AbstrSynTree& ASTref_;									- AbstractSyntaxTree
* std::unordered_map<Key, Value_Vector> table;				- Type Table
* std::string nameSpace = "Global";							- Current namespace for pNode				
* std::stack<nSpace> namespaceStack;						- Keep track of current namespace
* std::string parent_type;									- Keeping track of parent type of pNode


Build Process:
==============
Required files
-AbstrSynTree.h
-ActionAndRules.h

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
#include <stack>
#include <unordered_set>

#include "../AbstractSyntaxTree/AbstrSynTree.h"
#include "../Parser/ActionsAndRules.h"

using namespace CodeAnalysis;

	using file = std::string;
	using nSpace = std::string;
	using type = std::string;
	using Key = std::string;

	class Values
	{
	public:
		file file_name;
		nSpace nameSpace_name;
		type node_type;
	};

	class Type_Table
	{
	public:
		using Value_Vector = std::vector<Values>;
		using lineVector = std::vector<std::vector<int>>;
		Type_Table();

		bool saveIntoTable(Key key, Value_Vector v);
		size_t count();
		void traversal(ASTNode* pNode);
		bool isType(ASTNode* pNode);
		void populateTypeTable();
		std::vector<std::string> keys();
		//void displayMap();
		std::unordered_map<Key, Value_Vector>& returnTypeTable();
		std::unordered_map < std::string, lineVector>& returnLineMap();

	private:
		using Item = std::pair<Key, Value_Vector>;
		AbstrSynTree& ASTref_;
		std::unordered_map<Key, Value_Vector> table;
		std::string nameSpace = "Global";
		std::stack<nSpace> namespaceStack;
		std::string parent_type;
		std::unordered_map < std::string, lineVector> lineMap;
		
	};





	



