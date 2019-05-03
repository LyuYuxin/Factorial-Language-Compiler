#pragma once
#include<vector>
#include<string>
#include<map>
#include<fstream>
using namespace std;
class Lex;


typedef struct {
	string name;
	int type;
}STableEntry;

class Lex {
public:
	Lex(const ifstream& file);
	
	string analyze_it(char ch);
	void mainProcess();
private:
	vector<STableEntry> m_outputTable;
	map<string, int> m_typeIndexTable;
	ifstream m_file;
};