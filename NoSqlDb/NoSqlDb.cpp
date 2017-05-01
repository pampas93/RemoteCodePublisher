/////////////////////////////////////////////////////////////////////
// NoSqlDb.cpp - Demonsration of key/value pair in-memory database //
//                                                                 //
// --Manually adding elements into database						   //
// --Updating, deleting and adding new records					   //
// --Persisting the database into XML and back into Database	   //
//																   //
// - Test stub to demonstrate NoSqlDb.h, DateTime.h				   //
/////////////////////////////////////////////////////////////////////

#ifdef TEST_NOSQLDB

#include "../NoSqlDb/NoSqlDb.h"
#include <iostream>
#include <fstream>
#include <vector>

using namespace NoSQLDB;
using StrData = std::string;
using Keys = std::vector<Key>;

int main()
{
  std::cout << "\n  Demonstrating NoSql Helper Code";
  std::cout << "\n =================================\n";

  std::cout << "\n  Creating and saving NoSqlDb elements with string data";
  std::cout << "\n -------------------------------------------------------\n";

  NoSqlDb<StrData> db;

  Element<StrData> elem1;
  db.saveRecord("abc.cpp", elem1);

  Element<StrData> elem2;
  db.saveRecord("a.h",elem2);

  Element<StrData> elem3;
  db.saveRecord("x.h", elem3);

  elem1.saveChild("a.h");
  elem1.saveChild("x.h");
  db.saveValue("abc.cpp", elem1);

  std::cout << "\n  Retrieving elements from NoSqlDb<string>";
  std::cout << "\n ------------------------------------------\n";
  std::cout << "\n  size of db = " << db.count() << "\n";
  Keys keys = db.keys();
  for (Key key : keys)
  {
    std::cout << "\n  " << key << ":";
    std::cout << db.value(key).show();
  }
}
#endif


