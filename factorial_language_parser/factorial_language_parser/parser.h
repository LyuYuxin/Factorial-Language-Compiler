#pragma once
#include<vector>
#include<string>
#include"lexical_analyzer.h"
#include<fstream>

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
	void processInput();
    void writeToFile();


private:
	vector<STableEntry>::const_iterator m_curWord;
    int m_curLine;
    int m_curLevel;
    vector<STableEntry>::const_iterator m_lastWord;
    vector<SvariableNameEntry*> m_variableNameTable;
    vector<SprocessNameEntry*> m_processNameTable;
    ofstream m_outputError;
    ofstream m_outputVaribleTable;
    ofstream m_outputFunctionTable;

    enum m_errorTypes {
        //**********************È±ÉÙ¹Ø¼ü×Ö´íÎó**********************//
        LACK_BEGIN, 
        LACK_END, 
        LACK_SEMICOLON,
        LACK_INTEGER, 
        LACK_STATEMENT,
        LACK_LEFT_BRACKET, 
        LACK_RIGHT_BRACKET,
        UNDEFINED_IDENTIFIER, 
        VOID_KEYWORD_COMBINATION,
        VOID_CHARACTER,
        LACK_IDENTIFIER, 
        LACK_FUNCTION_DEFINATION, 
        LACK_DECLARATION, 
        LACK_EXPRESSION, 
        LACK_CONDITION, 
        LACK_IF, 
        LACK_THEN, 
        LACK_ELSE,
        LACK_PARAMETER_DECLARATION,
        REPEAT_DECLARATION,
        LACK_FACTOR
   
    };
    
    void program();
    void  S();
    void A();//for subprogram
    void A_();
    int C();//for subProgram
    void C_();

    int A(SprocessNameEntry*, const string&);//for function
    void A_(SprocessNameEntry*);//for function
    int C(const string &funcname, string *name =NULL);//for function
    int C_(const string &funcname, string*);
    void B(const string&);
    void B_(const string&);

    int functionBody(SprocessNameEntry *, const string &);
    int D(const string&funcname);
    int H();
    int H_();

    int K();
    int K_();
    int F();
    int G();
    int E();
    void E_();
    bool variableExist(const string&);
    bool processExist(const string&);
    void advance();
    void error(int errorkind);
    int getLastVariableOffset();
};
