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

enum m_typeIndexTable {
    BEGIN = 1, END, INTEGER, IF, THEN, ELSE, FUNCTION, READ,
    WRITE, IDENTIFIER, CONSTANT, EQUAL, NOT_EQUAL, LESS_EQUAL,
    LESS, MORE_EQUAL, MORE, MINUS, MUTIPLY, ASSIGN, LEFT_BRACKET,
    RIGHT_BRACKET, SEMICOLON, END_OF_LINE, END_OF_FILE
};


class Lex {
public:
	Lex(istream&);
	vector<STableEntry> lexAnalyze();
	void writeToFile();
	int reserve(const string&);

private:
	char getch();
	char getnbch(char&);
	bool isLetter(char);
	bool isDigit(char);
	void retract();
	//int symbol(string&);
	//int constant(string&);
	void error(int);


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


