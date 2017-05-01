/////////////////////////////////////////////////////////////////////
//  SCC.cpp - Creates a Strongly Connected graph		           //
//																   //
//  Language:      Visual C++ 2015                                 //
//  Platform:      Dell Inspiron, Windows 8.1			           //
//  Application:   Dependency Analysis - CIS 687 Project 2         //
//  Author:        Abhijit Srikanth SUID:864888072			       //
/////////////////////////////////////////////////////////////////////

#include "../StrongComponents/SCC.h"

//---------------------------- Graph Class ------------------------------//

//<---------------------- Default Constructor ------------------->
Directed_Graph::Directed_Graph()
{
}

Directed_Graph::~Directed_Graph()
{
	delete[] adjacency_matrix;
}

//<---------------------- Main Graph function to create Adjacency Matrix and Map to tag int to paths ------------------->
std::unordered_map<std::string, int> Directed_Graph::createGraph(DependencyTable &dptemp)
{
	try {
		dpx = dptemp;
		arrayOfFiles = dpx.getAllFiles();
		verticesCount = makeMap();
		adjacency_matrix = new std::list<int>[verticesCount];
		makeAdjMatrix();

		return file2IntMap;
	}
	catch (const std::exception& e)
	{
		std::cout << e.what();
	}
	return file2IntMap;
}

//<---------------------- Constructing an unordered Map to map Each file path to int ------------------->
int Directed_Graph::makeMap()
{
	int i = 0;
	for (File x : arrayOfFiles)
	{
		file2IntMap[x] = i;
		i++;
	}
	return i;
}

//<---------------------- Constructing an Adjacency Matrix ------------------->
void Directed_Graph::makeAdjMatrix()
{
	//adjacency_matrix = new std::list<int>[verticesCount];
	depTable = dpx.getDb();
	
	Files files = depTable.keys();
	for (File file : files)
	{
		Files listFiles = depTable.value(file).dependencies.getValue();
		int index = file2IntMap[file];
		for (File x : listFiles)
		{
			adjacency_matrix[index].push_back(file2IntMap[x]);
		}
	}
}

//<---------------------- Returning the private Adjacency matrix ------------------->
std::list<int>* Directed_Graph::returnAdjMat()
{
	return adjacency_matrix;
}




//---------------------------- StronglyConnectedComps Class ------------------------------//

//<---------------------- Default Constructor ------------------->
StronglyConnectedComps::StronglyConnectedComps()
{
}

//<---------------------- Main Function to create SCC ------------------->
void StronglyConnectedComps::makeGraph(DependencyTable dp)
{
	dpTable = dp;
	file2 = graph.createGraph(dpTable);
	vertices_no = (int)file2.size();
	adj_matrix = graph.returnAdjMat();
	SCCMain();
	//SccDisplay();
}

//<---------------------- Strongly Connected Graph construction ------------------->
void StronglyConnectedComps::SCCMain()
{
	try {
		int *vss = new int[vertices_no];
		int *scc_low = new int[vertices_no];
		bool *sBool = new bool[vertices_no];
		std::stack<int> *scc_stack = new std::stack<int>();

		for (int i = 0; i < vertices_no; i++)
		{
			vss[i] = -1;
			scc_low[i] = -1;
			sBool[i] = false;
		}

		for (int i = 0; i < vertices_no; i++)
			if (vss[i] == -1)
				SCC_Recursive(i, vss, scc_low, scc_stack, sBool);

		delete[] vss;
		delete[] scc_low;
		delete[] sBool;
		delete scc_stack;
	}
	catch (const std::exception& e)
	{
		std::cout << e.what();
	}

}

//<---------------------- SCC Recursive function ------------------->
void StronglyConnectedComps::SCC_Recursive(int u, int vss[], int scc_low[], std::stack<int>* scc_stack, bool sBool[])
{
	try {
		static int time = 0;

		vss[u] = scc_low[u] = ++time;
		scc_stack->push(u);
		sBool[u] = true;

		std::list<int>::iterator i;
		for (i = adj_matrix[u].begin(); i != adj_matrix[u].end(); ++i)
		{
			int v = *i;
			if (vss[v] == -1)
			{
				SCC_Recursive(v, vss, scc_low, scc_stack, sBool);
				scc_low[u] = min(scc_low[u], scc_low[v]);
			}

			else if (sBool[v] == true)
				scc_low[u] = min(scc_low[u], vss[v]);
		}

		int w = 0;
		if (scc_low[u] == vss[u])
		{
			while (scc_stack->top() != u)
			{
				w = (int)scc_stack->top();
				temp.push_back(valueInMap(w));
				//std::cout << w << " ";
				sBool[w] = false;
				scc_stack->pop();
			}
			w = (int)scc_stack->top();
			temp.push_back(valueInMap(w));
			scc_matrix[ijk] = temp;
			ijk++;
			temp.clear();
			//std::cout << w << "\n";
			sBool[w] = false;
			scc_stack->pop();
		}
	}
	catch (const std::exception& e)
	{
		std::cout << e.what();
	}
}

//<---------------------- Mapping back from int back to file path ------------------->
std::string StronglyConnectedComps::valueInMap(int w)
{
	for (auto i : file2)
	{
		if (i.second == w)
			return i.first;
	}
	return "Null";
}

//<---------------------- Returns the Strongly connected matrix ------------------->
std::unordered_map<int, std::vector<std::string>>& StronglyConnectedComps::returnMatrix()
{
	return scc_matrix;
}

//<---------------------- Persisting Scc graph into XML ------------------->
//std::string StronglyConnectedComps::persistXml(std::string path)
//{
//	std::string xml;
//	XmlDocument document;
//	sptr pRoot = makeTaggedElement("Strongly Connected Components");
//	document.docElement() = pRoot;
//	int i = 1;
//	for (auto x : scc_matrix)
//	{
//		sptr pKey = makeTaggedElement("SCC");
//		pRoot->addChild(pKey);
//		for (auto xx : x.second)
//		{
//			sptr px = makeTaggedElement("File");
//			pKey->addChild(px);
//			sptr pxkey = makeTextElement(xx);
//			px->addChild(pxkey);
//		}
//	}
//	xml = document.toString();
//	//std::string path = "../XML_SCC.xml";
//	try
//	{
//		std::ofstream sample(path);
//		sample << xml;
//		sample.close();
//	}
//	catch (const std::exception& e)
//	{
//		std::cout << e.what();
//	}
//	return xml;
//}

//void StronglyConnectedComps::SccDisplay()
//{
//	for (auto toD : scc_matrix)
//	{
//		for (auto xx : toD.second)
//		{
//			std::cout << xx << "\t";
//		}
//		std::cout << "\n";
//	}
//}

#ifdef TEST_SCC

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

	StronglyConnectedComps scc_temp;
	scc_temp.makeGraph(dp_temp);

	for (auto toD : scc_temp.returnMatrix())
	{
		for (auto xx : toD.second)
		{
			std::cout << xx << "\t";
		}
		std::cout << "\n";
	}
	
}

#endif


