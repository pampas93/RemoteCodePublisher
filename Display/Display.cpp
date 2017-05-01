/////////////////////////////////////////////////////////////////////
//  Display.cpp - Display Typetable, DependencyDB, SCC		       //
//																   //
//  Language:      Visual C++ 2015                                 //
//  Platform:      Dell Inspiron, Windows 8.1			           //
//  Application:   Dependency Analysis - CIS 687 Project 2         //
//  Author:        Abhijit Srikanth SUID:864888072			       //
/////////////////////////////////////////////////////////////////////

#include "Display.h"
using namespace std;

//<---------------------- Function to display Type Table analysis ------------------->
void Show::showTypeTable(tableType tb_temp)
{
	try {
		using Item = std::pair<Key, Value_Vector>;
		std::cout << "\n\n\n  " << "---------------------------- Type Table --------------------------" << "\n\n";
		for (Item item : tb_temp)
		{
			std::cout << setw(13) << " Type Name :" << item.first << endl;
			for (Values x : item.second)
			{
				cout << "\t" << setw(12) << "Type " << ":" << x.node_type << endl;
				cout << "\t" << setw(12) << "NameSpace " << ":" << x.nameSpace_name << endl;
				cout << "\t" << setw(12) << "File Path " << ":" << x.file_name << endl;
			}
			std::cout << "\n\n";
		}
	}
	catch (const std::exception& e)
	{
		std::cout << e.what();
	}
}

//<---------------------- Function to display Dependency Table analysis ------------------->
void Show::showDependencyDB(NoSQLDB::NoSqlDb<std::string> db_temp)
{
	try {
		Files keys = db_temp.keys();
		std::cout << "\n\n\n  " << "------------------------ Dependency Table -------------------------" << "\n\n";
		for (Key key : keys)
		{
			std::cout << setw(13) << " File Name :" << key << endl;
			std::cout << db_temp.value(key).show();
			std::cout << endl;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << e.what();
	}
}

//<---------------------- Function to display Strongly connected graph files ------------------->
void Show::showSCC(std::unordered_map<int, std::vector<std::string>> scc_matrix_temp)
{
	try {
		std::cout << "\n\n\n  " << "---------------- Strongly Connected Components -------------------" << "\n\n";
		int i = 1;
		for (auto toD : scc_matrix_temp)
		{
			cout << " Strong Component " << setw(2) << i << ":" << endl << endl;
			i++;
			for (auto xx : toD.second)
			{
				cout << "\t -> " << xx << endl;
			}
			std::cout << "\n\n\n";
		}
	}
	catch (const std::exception& e)
	{
		std::cout << e.what();
	}
}

//<---------------------- Function to display XML structure after persisting ------------------->
void Show::showXML(std::string xmlString)
{
	std::cout << "\n" << xmlString << "\n";
}


#ifdef TEST_DISPLAY

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

