#pragma once
#include<vector>
#include<string>
#include"lexical_analyzer.h"
using std::vector;
using std::string;

class Parser;

typedef struct node{
	string word;
	vector<node*> child;
    bool isTerminal;
    node(const string& str, bool status = false) :
    word(str), isTerminal(status){}
}Snode;


enum VariableOrPara {
    VARIABLE, PARAMETER
};


typedef struct variableNameEntry {
    string name;
    string vproc;
    int vkind;
    string vtype;
    int vlev;
    int vadr;
    variableNameEntry(const string& itsname, const string& proc
        , int kind, int lev)
        :name(itsname), vproc(proc), vkind(kind), vtype("integer"), vlev(lev),vadr(0){}
}SvariableNameEntry;

typedef struct processNameEntry {
    string pname;
    string ptype;
    int plev;
    int fadr;
    int ladr;
    processNameEntry(const string &name, int lev)
        :pname(name), ptype("integer"), plev(lev){}
}SprocessNameEntry;

class Parser{
public:
	Parser(const vector<STableEntry>&);
	~Parser();
	node* processInput();

private:
	node* m_root;
	vector<STableEntry>::const_iterator m_curWord;
    int m_curLine;
    int m_curLevel;
    vector<SvariableNameEntry*> m_variableNameTable;
    vector<SprocessNameEntry*> m_processNameTable;

    enum m_errorTypes {
        LACK_BEGIN, LACK_END, LACK_SEMICOLON,LACK_INTEGER, LACK_LEFT_BRACKET, 
        LACK_RIGHT_BRACKET, UNDEFINED_IDENTIFIER, VOID_KEYWORD_COMBINATION,
        LACK_IDENTIFIER, LACK_FUNCTION_DEFINATION, LACK_DECLARATION, LACK_EXPRESSION, 
        LACK_CONDITION, LACK_IF, LACK_THEN, LACK_ELSE, LACK_ITEM
   
    };
    
    void program();
    node * subProgram(const string &procname);
    node * declarationTable(const string &procname, int *);
    node * declarationTable_(const string &processname);
    node * declaration(const string &procname);
    node * executiveTable();
    node * functionBody(SprocessNameEntry *);
    node * executiveStatement();
    node * item();
    node * factor();
    node * conditionStatement();
    node * relationalStatement();
    node * countExpression();

    bool variableExist(const string&);
    bool processExist(const string&);
    void advance();
    void error(int errorkind);
    node *getNewNode(const string& name, bool isTerminal = false);
    node * getErrorNode();
    int getLastVariableOffset();

};
