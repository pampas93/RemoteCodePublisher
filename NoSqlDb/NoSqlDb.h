#pragma once
/////////////////////////////////////////////////////////////////////
//  NoSqlDb.h - Key-value pair in Db, and corresponding functions  //
//  ver 1.0                                                        //
//                                                                 //
//  Language:      Visual C++ 2015, SP1                            //
//  Platform:      Dell Inspiron, Windows 8.1			           //
//  Application:   NoSql Database - CIS 687 Project 1              //
//  Author:        Abhijit Srikanth SUID:864888072			       //
/////////////////////////////////////////////////////////////////////
/*
Module Operations:
==================

This module defines a No SQL database class and an Element class.
* - NoSqlDb instance creates an underoreded map in the Heap memory
and itsfunctions is used to add, delete and modify the values,
persist into XML file and also persisting from the XML file.
* - The Element instance defines a the value part of the map and
holds the meta data, data for the record stored.

Both the above clases are Template classes,  that use one or more
unspecified types, defined by arguments in the template declaration.

Private Interface:
=================
NoSqlDb Class > ---- >

* std::string path;													- string to store path file for xml persisting
* std::unordered_map<Key, Element<Data>> dependencyStore;			- Database to store dependency analysis

shared_ptr<AbstractXmlElement> sptr		//Shared pointer used to create nodes in XML Document

Element Class > ---- >

dependencies														- Children of value in database

Build Process:
==============
Required files
- Cppproperties.h,
- XmlDocument.h, XmlElement.h,
- NoSqlDb.cpp

Build commands
- devenv CodeAnalyzerEx.sln

Maintenance History:
====================
ver 1.0 : 07 Mar 17
- first release
*/

 #include <unordered_map>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <iostream>
#include <fstream>
#include "../CppProperties/CppProperties.h"
//#include "../XmlDocument/XmlDocument/XmlDocument.h"
//#include "../XmlDocument/XmlElement/XmlElement.h"

//using namespace XmlProcessing;

//using sptr = std::shared_ptr<AbstractXmlElement>;

namespace NoSQLDB
{
  using Key = std::string;
  using Children = std::vector<Key>;

  /////////////////////////////////////////////////////////////////////
  // Element class represents a data record in our NoSql database
  // - in our NoSql database that is just the value in a key/value pair
  // - it needs to store child data, something for you to implement
  //
  template<typename Data>
  class Element
  {
  public:

    Element() {}
	Property<Children> dependencies;          // Dependencies
    void saveChild(Key key);
    std::string show();

  };

  //<---------------------- Function to append child to perticular key ------------------->
  template<typename Data>
  void Element<Data>::saveChild(Key key)
  {
	  Children temp = dependencies.getValue();
	  for (Key x : temp)
	  {
		  if (x == key)
			  return;
	  }
    dependencies.getValue().push_back(key);
	return;
  }

  //<---------------------- Function to Display the dependencies (children) ------------------->
  template<typename Data>
  std::string Element<Data>::show()
  {
    std::ostringstream out;
    out.setf(std::ios::adjustfield, std::ios::left);
    Children dependencies_ = static_cast<Children>(dependencies);
    if (dependencies_.size() > 0)
    {
		out << "\n" << std::setw(8) << " Dependencies " << " : \n";
      for (size_t i = 0; i < dependencies_.size(); ++i)
      {
		  out <<"\t -> " << dependencies_[i] << std::endl;
      }
	  out << "\n\n" ;
    }
    return out.str();
  }

  /////////////////////////////////////////////////////////////////////
  // NoSqlDb class is a key/value pair in-memory database
  //	- stores and retrieves elements
  //	- persists into and xml
  //	

  template<typename Data>
  class NoSqlDb
  {
  public:
    using Key = std::string;
    using Keys = std::vector<Key>;
	using xmlstring = std::string;

    Keys keys();
    bool hasKey(Key key);
    bool saveRecord(Key key, Element<Data> elem);
    bool saveValue(Key key, Element<Data> elem);
    bool removeRecord(Key key);
    Element<Data> value(Key key);
    size_t count();
	//void display();
	//std::string persistIntoXml(std::string path);

  private:
    using Item = std::pair<Key, Element<Data>>;
	std::string path;
    std::unordered_map<Key, Element<Data>> dependencyStore;
  };

  //<---------------------- Function to return keys of database ------------------->
  template<typename Data>
  typename NoSqlDb<Data>::Keys NoSqlDb<Data>::keys()
  {
    Keys keys;
    for (Item item : dependencyStore)
    {
      keys.push_back(item.first);
    }
    return keys;
  }

  //<---------------------- Function to check if key present in database ------------------->
  template<typename Data>
  bool NoSqlDb<Data>::hasKey(Key key)
  {
    if (dependencyStore.find(key) == dependencyStore.end())
      return false;
    return true;
  }

  //<---------------------- Function to save record in database ------------------->
  template<typename Data>
  bool NoSqlDb<Data>::saveRecord(Key key, Element<Data> elem)
  {
    if (dependencyStore.find(key) != dependencyStore.end())
      return false;
	dependencyStore[key] = elem;
    return true;
  }

  //<---------------------- Function to remove record from database ------------------->
  template<typename Data>
  bool NoSqlDb<Data>::removeRecord(Key key)
  {
	  if (dependencyStore.find(key) != store.end())
	  {
		  deleteInChildren(key);
		  dependencyStore.erase(key);
		  return true;
	  }
	  else
		  return false;
  }

  //<---------------------- Function to save or orverwrite value to perticular key in database ------------------->
  template<typename Data>
  bool NoSqlDb<Data>::saveValue(Key key, Element<Data> elem)
  {
    /*if (dependencyStore.find(key) == dependencyStore.end())
      return false;
	dependencyStore[key] = elem;*/
	  dependencyStore[key] = elem;
    return true;
  }

  //<---------------------- Function to return value at that key ------------------->
  template<typename Data>
  Element<Data> NoSqlDb<Data>::value(Key key)
  {
    if (dependencyStore.find(key) != dependencyStore.end())
      return dependencyStore[key];
    return Element<Data>();
  }

  template<typename Data>
  size_t NoSqlDb<Data>::count()
  {
    return dependencyStore.size();
  }

  //<---------------------- Function to persist the database into xml ------------------->
//  template<typename Data>
//  inline std::string NoSqlDb<Data>::persistIntoXml(std::string path)
//  {
//	  xmlstring xml;
//	  XmlDocument document;
//	  sptr pRoot = makeTaggedElement("Dependency_Table");
//	  document.docElement() = pRoot;
//	  Element<Data> temp;
//	  Keys setOfkeys = keys();
//	  for (Key k : setOfkeys)
//	  {
//		  sptr pKey = makeTaggedElement("Record");
//		  pRoot->addChild(pKey);
//		  sptr px = makeTaggedElement("Key");
//		  pKey->addChild(px);
//		  sptr pxkey = makeTextElement(k);
//		  px->addChild(pxkey);
//		  temp = value(k);
//		  Keys children = temp.dependencies;
//		  sptr pDp = makeTaggedElement("Dependencies");
//		  pKey->addChild(pDp);
//		  for (Key k : children)
//		  {
//			  sptr pChild = makeTaggedElement("File");
//			  pDp->addChild(pChild);
//			  sptr pFile = makeTextElement(k);
//			  pChild->addChild(pFile);
//		  }
//	  }
//	  xml = document.toString();
////	  path = "../XML_dependency.xml";
//	  try
//	  {
//		  std::ofstream sample(path);
//		  sample << xml;
//		  sample.close();
//	  }
//	  catch (const std::exception& e)
//	  {
//		  std::cout << e.what();
//	  }
//	  return xml;
//  }
}
