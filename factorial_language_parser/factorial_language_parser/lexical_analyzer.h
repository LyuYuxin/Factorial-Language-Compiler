#pragma once
#include<vector>
#include<string>
#include<map>
#include<set>
#include<fstream>
using namespace std;
class Lex;

typedef struct STableEntry{
	string name;
	int type;
	STableEntry(string n, int t) {
		name = n;
		type = t;
	}
}STableEntry;

class Lex {
public:
	Lex(istream&);
	vector<STableEntry> lexAnalyze();
	void writeToFile();
private:
	char getch();
	char getnbch(char&);
	bool isLetter(char);
	bool isDigit(char);
	void retract();
	int reserve(const string&);
	//int symbol(string&);
	//int constant(string&);
	void error(int);

	enum m_typeIndexTable {
		begin = 1, end, integer, If, then, Else, function, read,
		write, identifier, constant, equal, not_equal, less_equal,
		less, more_equal, more, minus, mutiply, assign, left_bracket,
		right_bracket, semicolon, end_of_line, end_of_file
	};

	enum {
		colonerror, invalidch
	};

	vector<STableEntry> m_output;
	istream &m_file;
	ofstream m_errorInformation;
	int m_curline;

	map<string, int> m_reservedWordTable;
	set<string> m_identifierTable;
	map<int, int> m_constantTable;
};


