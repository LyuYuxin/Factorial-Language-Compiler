#include<fstream>
#include<iostream>
#include"lexical_analyzer.h"
using namespace std;

int main() {
	ifstream in;
	in.open("input.pas", ios::in);
	Lex lex(in);
	lex.mainProcess();

	return 0;
}