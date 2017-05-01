/////////////////////////////////////////////////////////////////////
//  typetable.cpp - Creates the Type Table				           //
//																   //
//  Language:      Visual C++ 2015                                 //
//  Platform:      Dell Inspiron, Windows 8.1			           //
//  Application:   Dependency Analysis - CIS 687 Project 2         //
//  Author:        Abhijit Srikanth SUID:864888072			       //
/////////////////////////////////////////////////////////////////////

#include "../TypeTable/typetable.h"

using namespace CodeAnalysis;

//<------------------ TypeTable Class Constructor Building the AST tree ------------------>
Type_Table::Type_Table() :
	ASTref_(Repository::getInstance()->AST()) {}

//<------------------ Function to save into Unordered_map table ------------------>
bool Type_Table::saveIntoTable(Key key, Value_Vector v)
{
	/*if (table.find(key) != table.end())
	return false;*/
	table[key] = v;
	return true;
}

//<------------------ Returns count of table ------------------>
size_t Type_Table::count()
{
	return table.size();
}

//<------------------ Checks if token is of type Class, struct, enum or global func ------------------>
bool Type_Table::isType(ASTNode*pNode)
{
	static std::string types[] = {
		"class", "struct", "enum"
	};
	
	if ((pNode->type_ == "function" || pNode->type_ == "class") && (!(pNode->startLineCount_ == pNode->endLineCount_) && !(pNode->startLineCount_+1 == pNode->endLineCount_)))
	{
		std::vector<int> tint(2);
		
		tint.at(0) = pNode->startLineCount_;
		tint.at(1) = pNode->endLineCount_;
		lineVector temp = lineMap[pNode->path_];
		temp.push_back(tint);
		lineMap[pNode->path_] = temp;
	}

	if (pNode->type_ == "function" && nameSpace == "Global Namespace" && pNode->name_ != "main" && pNode->name_ != "void" && parent_type == "namespace")
	{
		return true;
	}
	for (std::string type : types)
	{
		if (pNode->type_ == type)
			return true;
	}
	return false;
}

//<------------------ DFS traversal through AST Tree ------------------>
void Type_Table::traversal(ASTNode* pNode)
{
	try
	{
		if (pNode->type_ == "namespace")
			namespaceStack.push(pNode->name_);
		nameSpace = namespaceStack.top();
		if (isType(pNode))
		{
			Values x;
			x.file_name = pNode->path_;
			x.nameSpace_name = nameSpace;
			x.node_type = pNode->type_;
			std::vector<Values> temp;
			if (table.find(pNode->name_) != table.end())
			{
				temp = table[pNode->name_];
				temp.push_back(x);
			}
			else {
				temp.push_back(x);
			}
			saveIntoTable(pNode->name_, temp);
		}
		for (auto pChild : pNode->children_)
		{

			parent_type = pNode->type_;
			traversal(pChild);
			parent_type = pNode->type_;
		}
		if (pNode->type_ == "namespace") {
			namespaceStack.pop();
		}
	}
	catch (const std::exception& e)
	{
		std::cout << e.what();
	}
}

//<------------------ Populate the type table (main function to call) ------------------>
void Type_Table::populateTypeTable()
{
	try
	{
		ASTNode* proot = ASTref_.root();
		traversal(proot);
		//displayMap();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what();
	}
}

//<------------------ Returns the keys from table ------------------>
std::vector<std::string> Type_Table::keys()
{
	std::vector<std::string> temp;
	for (Item item : table)
	{
		temp.push_back(item.first);
	}
	return temp;
}

//<------------------ Returns the keys from table ------------------>
/*void Type_Table::displayMap()
{
for (Item item : table)
{
std::cout << "\n " << item.first;
for (Values x : item.second)
std::cout << "\n\t" << x.file_name << "\t" << x.nameSpace_name << "\t" << x.node_type;
std::cout << "\n ";
}
}*/

//<------------------ Returns the typeTable back to main ------------------>
std::unordered_map<Key, std::vector<Values>>& Type_Table::returnTypeTable()
{
	return table;
}

std::unordered_map<std::string, std::vector<std::vector<int>>>& Type_Table::returnLineMap()
{
	return lineMap;
}

#ifdef TEST_TestTable

int main()
{
	Type_Table tb_temp;
	tb_temp.populateTypeTable();

	using Value_Vector = std::vector<Values>;
	using Item = std::pair<Key, Value_Vector>;

	for (Item item : tb_temp.returnTypeTable())
	{
		std::cout << "\n " << item.first;
		for (Values x : item.second)
			std::cout << "\n\t" << x.file_name << "\t" << x.nameSpace_name << "\t" << x.node_type;
		std::cout << "\n ";
	}

}
#endif


