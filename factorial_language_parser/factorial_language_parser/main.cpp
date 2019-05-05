#include<fstream>
#include<iostream>
#include"lexical_analyzer.h"
using namespace std;

int main() {
	ifstream in;
	in.open("input.pas");
	if (!in.is_open())
		cerr << "open input file error!" << endl;
	Lex lex(in);
	lex.lexAnalyze();
	lex.writeToFile();
	return 0;
}