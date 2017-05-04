/////////////////////////////////////////////////////////////////////
//  DependencyAnalyzer.cpp - Creates the Dependency Table          //
//																   //
//  Language:      Visual C++ 2015                                 //
//  Platform:      Dell Inspiron, Windows 8.1			           //
//  Application:   Dependency Analysis - CIS 687 Project 2         //
//  Author:        Abhijit Srikanth SUID:864888072			       //
/////////////////////////////////////////////////////////////////////

#include "../DependencyAnalyzer/DependencyAnalysis.h"

using namespace CodeAnalysis;

//<---------------------- Default Constructor ------------------->
DependencyTable::DependencyTable()
{
}

//<---------------------- Parameterised Constructor ------------------->
DependencyTable::DependencyTable(tableType temp, Files allFiles)
{
	try {
		setTypeTable(temp);
		filePaths = allFiles;
	}
	catch (const std::exception& e)
	{
		std::cout << e.what();
	}
}

//<---------------------- Setting Type Table(private member) ------------------->
void DependencyTable::setTypeTable(tableType t)
{
	tempTypeTable = t;
}

//<---------------------- Store all File paths in vector------------------->
std::vector<std::string> DependencyTable::getAllFiles()
{
	std::vector<File> tempVector;
	for (File x : filePaths)
	{
		tempVector.push_back(FileSystem::Path::getPath(x) + FileSystem::Path::getName(x));
	}
	return tempVector;
}

//<---------------------- Returning The dependency Table (database) ------------------->
NoSQLDB::NoSqlDb<std::string>& DependencyTable::getDb()
{
	return dependency_Store;
}

//<---------------------Creates a map of File to its File path and returns that map -------->
std::unordered_map<std::string, std::string> DependencyTable::funcFile2Path()
{
	File fileName, filePath;
	for (File f : filePaths)
	{
		filePath = FileSystem::Path::getPath(f) + FileSystem::Path::getName(f);
		fileName = FileSystem::Path::getName(f);
		file2PathMap[fileName] = filePath;
	}
	return file2PathMap;
}

//std::string DependencyTable::returnXML(std::string path)
//{
//	return dependency_Store.persistIntoXml(path);
//}

//<---------------------- Main Depdendency function -To build the dependency store ------------->
std::string DependencyTable::dependencyMain(std::string path)
{
	//std::cout << "\n\n---------Dependency Package--------\n\n";
	try {

		for (File each_file : filePaths)
		{
			TokensInFile(each_file);
		}
		cppAndHeaderDependency();
		//displayDependency();
		//return dependency_Store.persistIntoXml(path);
		return std::string();
	}
	catch (std::exception& e)
	{
		std::cout << e.what();
		return "Exception";
	}
}

//<--------------------- Checks for dependency with the tokens in each file ------------------->
bool DependencyTable::TokensInFile(File eachFile)
{
	try {
		std::string just_file = FileSystem::Path::getName(eachFile);
		std::ifstream in(eachFile);
		if (!in.good())
		{
			std::cout << "\n  can't open " << eachFile << "\n\n";
			return false;
		}
		Scanner::Toker toker;
		toker.returnComments(false);
		toker.attach(&in);
		//std::cout << "\n  current line count = " << toker.currentLineCount();
		do
		{
			std::string tok = toker.getTok();
			if (tok == "\n" || tok == "(" || tok == ")" || tok == "[" || tok == "]" || tok == "{" || tok == "}" || tok == ";")
				continue;
			//std::cout << "\n -- " << tok;
			storeIntoDep(tok, eachFile);
		} while (in.good());
		in.close();
	}
	catch (std::logic_error& ex)
	{
		std::cout << "\n  " << ex.what();
	}
	return true;
}

//<---------------------- Checks for dependency, matching with typeTable ------------------->
bool DependencyTable::checkIfDependent(std::string token)
{
	if (tempTypeTable.find(token) == tempTypeTable.end())
		return false;
	return true;
}

//<---------------------- Storing into Dependency Database table ------------------->
bool DependencyTable::storeIntoDep(std::string token, File dep_file)
{
	if (checkIfDependent(token))
	{
		for (Values vx : tempTypeTable[token])
		{
			if (vx.file_name != dep_file)
			{
				NoSQLDB::Element<std::string> tempObj = dependency_Store.value(dep_file);
				tempObj.saveChild(vx.file_name);
				dependency_Store.saveValue(dep_file, tempObj);
			}
		}
		return true;
	}
	return false;
}

//<-------------------- Adding Header to cpp as dependency ---------------------->
void DependencyTable::cppAndHeaderDependency()
{
	for (File eachFile : filePaths)
	{
		if (FileSystem::Path::getExt(eachFile) == "h")
		{
			std::string cppFile = eachFile;
			cppFile.pop_back();
			cppFile.pop_back();
			cppFile.append(".cpp");
			if (std::find(filePaths.begin(), filePaths.end(), cppFile) != filePaths.end())
			{
				NoSQLDB::Element<std::string> tempObj = dependency_Store.value(eachFile);
				tempObj.saveChild(cppFile);
				dependency_Store.saveValue(eachFile, tempObj);
			}
		}
	}
}

//<-------------------- Displaying Dependency table ---------------------->
void DependencyTable::showDep()
{
	Files keys = dependency_Store.keys();
	std::cout << "\n\n\n  " << "------------------------ Dependency Table -------------------------" << "\n\n";
	for (Key key : keys)
	{
		std::cout << std::setw(13) << " File Name :" << key << std::endl;
		std::cout << dependency_Store.value(key).show();
		std::cout << std::endl;
	}
}


//void DependencyTable::displayDependency()
//{
//
//	Keys keys = dependency_Store.keys();
//	for (Key key : keys)
//	{
//		std::cout << "\n  " << key << ":";
//		std::cout << dependency_Store.value(key).show();
//	}
//}

#ifdef TEST_DPAnalyzer

#include <vector>
#include <string>

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

	DependencyTable dp_temp(tb.returnTypeTable(), allfilePaths);
	dp_temp.dependencyMain();

	Files keys = dp_temp.getDb().keys();
	for (Key key : keys)
	{
		std::cout << "\n  " << key << ":";
		std::cout << dp_temp.getDb().value(key).show();
	}
}

#endif
