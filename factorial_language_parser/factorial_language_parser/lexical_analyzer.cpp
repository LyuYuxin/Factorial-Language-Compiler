#include"lexical_analyzer.h"
#include<istream>
#include<iostream>
#include<iomanip>

Lex::Lex(istream & file):
	m_file(file),m_errorInformation("file.error"), m_curline(1)
{
	m_reservedWordTable["begin"] = begin;
	m_reservedWordTable["end"] = end;
	m_reservedWordTable["integer"] = integer;
	m_reservedWordTable["if"] = If;
	m_reservedWordTable["then"] = then;
	m_reservedWordTable["else"] = Else;
	m_reservedWordTable["function"] = function;
	m_reservedWordTable["read"] = read;
	m_reservedWordTable["write"] = write;
	if (!m_errorInformation) {
		cerr << "open error file failed !" << endl;
	}

}

void
Lex::writeToFile() {
	vector<STableEntry>::iterator it;
	ofstream outputFile("output.dyd");

	for (it = m_output.begin(); it != m_output.end(); ++it)
	{
		const char * str = it->name.c_str();
		outputFile << setw(16) <<setfill(' ')<< it->name << "  ";
		outputFile << setw(2) <<setfill('0')<< it->type << endl;
	}
}

vector<STableEntry> 
Lex::lexAnalyze() {
	char ch;

	while (m_file>>noskipws>>ch) {
	string token;
	ch = getnbch(ch);
	cout << ch;
	switch (ch) {
	case 'a':case'b':case'c':case'd':case'e':
	case'f':case'g':case'h':case'i':case'j':
	case'k':case'l':case'm':case'n':case'o':
	case'p':case'q':case'r':case's':case't':
	case'u':case'v':case'w':case'x':case'y':
	case'z':case'A':case'B':case'C':case'D':
	case'E':case'F':case'G':case'H':case'I':
	case'J':case'K':case'L':case'M':case'N':
	case'O':case'P':case'Q':case'R':case'S':
	case'T':case'U':case'V':case'W':case'X':
	case'Y':case'Z':
	{
		while (isLetter(ch) || isDigit(ch)) {
			token += ch;
			ch = getch();
		}
		retract();		

		int num = reserve(token);
		if (num)m_output.push_back(STableEntry(token, num));
		else {
			m_output.push_back(STableEntry(token, identifier));
			m_identifierTable.insert(token);
		}
		break;
	}
	case '0':case'1':case'2':case'3':case'4':case'5':case'6':case'7':
	case'8':case'9':
		while (isDigit(ch)) {
			token += ch;
			ch = getch();
		}
		m_output.push_back(STableEntry(token, constant));
		retract();
		break;
	case '=':
		token = ch;
		m_output.push_back(STableEntry(token, equal));
		break;
	case '-':
		token += ch;
		m_output.push_back(STableEntry(token, minus));
		break;
	case '*':
		token += ch;
		m_output.push_back(STableEntry(token, mutiply));
		break;
	case '(':
		token += ch;
		m_output.push_back(STableEntry(token, left_bracket));
		break;
	case')':
		token += ch;
		m_output.push_back(STableEntry(token, right_bracket));
		break;
	case '<':
		token += ch;
		ch = getch();
		if (ch == '=') {
			token += ch;
			m_output.push_back(STableEntry(token, less_equal));
		}
		else if(ch == '>'){
			token += ch;
			m_output.push_back(STableEntry(token, not_equal));
		}
		else {
			m_output.push_back(STableEntry(token, equal));
			retract();
		}
		break;
	case '>':
		token += ch;
		ch = getch();
		if (ch == '=') {
			token += ch;
			m_output.push_back(STableEntry(token, more_equal));
		}
		else {
			retract();
			m_output.push_back(STableEntry(token, more));
		}
		break;

	case ':':
		token += ch;
		ch = getch();
		if (ch == '=') {
			token += ch;
			m_output.push_back(STableEntry(token, assign));
		}
		else {
			error(colonerror);
		}
		break;
	case ';':
		token += ch;
		m_output.push_back(STableEntry(token, semicolon));
		break;
	case '\n':
		token ="EOLN";
		m_output.push_back(STableEntry(token, end_of_line));
		++m_curline;
		break;
	default:
		error(invalidch);
		break;
	}
	}
	m_output.push_back(STableEntry("EOF", end_of_file));

	return m_output;

}

char
Lex::getch() {
	char ch;
	m_file >> noskipws >> ch;
	m_file >> skipws;
	return ch;
}

char
Lex::getnbch(char &ch) {
	while (ch == ' ')
		m_file >>noskipws>> ch;
	m_file >> skipws;
	return ch;
}

bool
Lex::isDigit(char ch) {
	if (ch >= '0'&&ch <= '9')
		return true;
	return false;
}

bool
Lex::isLetter(char ch) {
	if (ch >= 'a'&&ch <= 'z' || ch >= 'A' && ch <= 'Z')
		return true;
	return false;
}

void
Lex::retract(){
	m_file.unget();
}

int
Lex::reserve(const string& str) {
	if (m_reservedWordTable.count(str) > 0) {
		return m_reservedWordTable[str];
	}
	else
		return 0;
}

void
Lex::error(int status) {
	m_errorInformation << "***LINE:" << m_curline << "  ";
	switch (status)
	{
	case colonerror:
		m_errorInformation << "Ã°ºÅ²»Æ¥Åä" << endl;
		break;
	case invalidch:
		m_errorInformation << "·Ç·¨×Ö·û" << endl;
		break;
	default:
		break;
	}
}