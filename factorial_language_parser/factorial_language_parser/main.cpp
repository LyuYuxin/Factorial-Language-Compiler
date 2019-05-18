#include<fstream>
#include<iostream>
#include"lexical_analyzer.h"
#include"parser.h"
using namespace std;

int main() {
	ifstream in;
	in.open("input.pas");
	if (!in.is_open())
		cerr << "open input file error!" << endl;
	Lex lex(in);

    vector<STableEntry> temp = lex.lexAnalyze();
    Parser parser(temp);
    parser.processInput();
	lex.writeToFile();
	return 0;
}