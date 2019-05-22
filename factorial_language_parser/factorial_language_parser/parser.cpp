#include"parser.h"
#include<iostream>
Parser::Parser(const vector<STableEntry> & input)
    :m_outputError("Parser_output.error"), m_outputFunctionTable("functionTable.pro"), m_outputVaribleTable("variableTable.var")
{    
	if (!input.size())cerr << "input is empty!" << flush;
	m_curWord = input.begin();
    m_curLevel = 1;
    m_curLine = 1;
}

void
Parser::processInput() {
    program();
}

void
Parser::program() {
    S();
    if (m_curWord->type == END_OF_FILE)
        return;
    else error(UNDEFINED_IDENTIFIER);
}

void
Parser::S() {
    if (m_curWord->type == BEGIN) {
        advance();
        A();
        string useless;
        B(useless);

        if (m_curWord->type == END) {
            advance();
        }
        else
        {
            error(LACK_END);
            advance();
        }

    }
    else if (m_curWord->type == END_OF_FILE)
    {
        error(LACK_BEGIN);
        return;
    }

    else {
        error(LACK_BEGIN);
        advance();
    }

}


void//for subprogram
Parser::A() {
    switch (m_curWord->type)
    {
    case INTEGER:
    {
        C();
        if (m_curWord->type == SEMICOLON) {
            advance();
        }
        else
        {
            error(LACK_SEMICOLON);
            advance();
        }
        A_();

    }break;
    case READ: case WRITE: case IDENTIFIER: case IF:
    {
        error(LACK_INTEGER);
        return;
    }

    default: 
        error(LACK_INTEGER);
        advance();

        break;
    }


}

void//for subprogram
Parser::A_() {
    switch (m_curWord->type) {
    case INTEGER:
    {
        C();

        if (m_curWord->type == SEMICOLON) {
            advance();
        }
        else {
            error(LACK_SEMICOLON);
            advance();
        }

        A_();


    }
    break;
    case READ:
    case WRITE:
    case IDENTIFIER:
    case IF:
        return;
    default:
        error(LACK_IDENTIFIER);
        advance();
        return;
    }
}

int//for subprogram
Parser::C() {
    if (m_curWord->type == INTEGER) {
        advance();
        C_();

    }
    else if (m_curWord->type == SEMICOLON) {
        error(LACK_INTEGER);
        return 0;
    }
    else {
        error(LACK_DECLARATION);//必须要有一个变量声明
        advance();

    }
    return 0;
  
}

void//for subprogram
Parser::C_() {
    string processname = "subProgram";

        if (m_curWord->type == IDENTIFIER) {// 变量
            SvariableNameEntry* v_entry = new SvariableNameEntry(m_curWord->name, processname, VARIABLE, m_curLevel);
            m_variableNameTable.push_back(v_entry);
            advance();
        }
        else if (m_curWord->type == FUNCTION) {//函数
            int status = 0;
            advance();

            string paraName;
            SprocessNameEntry* pentry = NULL;
            if (m_curWord->type == IDENTIFIER) {
                pentry = new SprocessNameEntry(m_curWord->name, m_curLevel);//创建函数条目
                ++status;
                advance();
            }
            else {
                error(LACK_IDENTIFIER);
                advance();
                return;
            }
            if (m_curWord->type == LEFT_BRACKET) {
                ++status;
                advance();
            }
            else {
                error(LACK_LEFT_BRACKET);
                advance();
                return;


            }

            if (m_curWord->type == IDENTIFIER) {//形参
                paraName = m_curWord->name;
                ++status;

                advance();

            }
            else {
                error(LACK_IDENTIFIER);
                advance();
                return;

            }
            if (m_curWord->type == RIGHT_BRACKET) {
                ++status;
                advance();
            }
            else {
                error(LACK_RIGHT_BRACKET);
                advance();
                return;


            }

            if (m_curWord->type == SEMICOLON) {
                ++status;
                advance();
            }
            else {
                error(LACK_SEMICOLON);
                advance();
                return;

            }

            if (status == 5) {//函数声明正确，注册该函数
                m_processNameTable.push_back(pentry);
               
            }
            else; //函数声明不正确,不会注册该函数
           functionBody(pentry, paraName);

        }
        else if (m_curWord->type == INTEGER|| m_curWord->type == BEGIN||m_curWord->type == END||m_curWord->type == IF||m_curWord->type == THEN|| 
            m_curWord->type == ELSE|| m_curWord->type == READ||m_curWord->type==WRITE) {
            error(VOID_KEYWORD_COMBINATION);
            advance();
            return;

        }
        else if (m_curWord->type == SEMICOLON) {
            error(VOID_CHARACTER);
            return;
        }
        else {
            error(VOID_CHARACTER);
            advance();
        }
}

int
Parser::functionBody(SprocessNameEntry* curfunction, const string& paraname) {
    ++m_curLevel;
    switch (m_curWord->type) {
    case BEGIN:
    {
        advance();
        if (A(curfunction, paraname))
            curfunction->ladr = getLastVariableOffset();

        B(curfunction->pname);

        if (m_curWord->type == END) {
            advance();
        }
        else
        {
            error(LACK_END);
        }

    }
    break;
    case SEMICOLON:
    {
        error(LACK_DECLARATION);
    }
    break;
    default:
        error(LACK_BEGIN);
        advance();
    }


    --m_curLevel;
    return 1;


}

int //for function
Parser::A(SprocessNameEntry*func, const string&paraname) {
    int status = NULL;
    string str;
    switch (m_curWord->type) {
    case INTEGER:
    {
            status = C(func->pname, &str);

            if (!status || str != paraname)
            {
                error(LACK_PARAMETER_DECLARATION);//必须要有形参声明
                return 0;
            }
            func->fadr = getLastVariableOffset();

            if (m_curWord->type == SEMICOLON) {
                advance();
            }
            else
            {
                error(LACK_SEMICOLON);
                return 0;
            }
            A_(func);
           return 1;

        }
    break;
    case READ: case WRITE: case IDENTIFIER: case IF:
    {
        error(LACK_INTEGER);
        return 0;
    }

    default:
        error(LACK_INTEGER);
        advance();

        break;
    }

    return 0;
}

void
Parser::A_(SprocessNameEntry*func) {
    switch (m_curWord->type) {
    case READ:
    case WRITE:
    case IDENTIFIER:
    case IF:
        return;
    case INTEGER:
    {
        C(func->pname);

        if (m_curWord->type == SEMICOLON) {
            advance();
        }
        else
        {
            error(LACK_SEMICOLON);
            advance();
        }
        A_(func);
    }
    break;
    default:
    {
        error(VOID_CHARACTER);
        advance();
    }
    }
}

int // for function
Parser::C(const string &funcname, string *s) {
    if (m_curWord->type == INTEGER) {
    advance();
    C_(funcname, s);

    }
    else if (m_curWord->type == THEN) {
        error(LACK_DECLARATION);
        return 0;
    }
    else {
        error(VOID_CHARACTER);
        advance();
    }

    return  1;

}

int
Parser::C_(const string &funcname, string *s) {
    if (m_curWord->type == IDENTIFIER) {// 变量
        SvariableNameEntry* v_entry = new SvariableNameEntry(m_curWord->name, funcname, VARIABLE, m_curLevel);
        m_variableNameTable.push_back(v_entry);
        advance();
        if (s != NULL && *s == "")
            *s = v_entry->name;
    }
    else if (m_curWord->type == FUNCTION) {//函数
        int status = 0;
        advance();

        string paraName;
        SprocessNameEntry* pentry = NULL;
        if (m_curWord->type == IDENTIFIER) {
            pentry = new SprocessNameEntry(m_curWord->name, m_curLevel);//创建函数条目
            ++status;
            advance();
        }
        else {
            error(LACK_IDENTIFIER);
            advance();
            return 0;
        }
        if (m_curWord->type == LEFT_BRACKET) {
            ++status;
            advance();
        }
        else {
            error(LACK_LEFT_BRACKET);
            advance();
            return 0;

        }

        if (m_curWord->type == IDENTIFIER) {//形参
            paraName = m_curWord->name;
            ++status;

            advance();

        }
        else {
            error(LACK_IDENTIFIER);
            advance();
            return 0;

        }
        if (m_curWord->type == RIGHT_BRACKET) {
            ++status;
            advance();
        }
        else {
            error(LACK_RIGHT_BRACKET);
            advance();
            return 0;

        }

        if (m_curWord->type == SEMICOLON) {
            ++status;
            advance();
        }
        else {
            error(LACK_SEMICOLON);
            advance();
            return 0;

        }

        if (status == 5) {//函数声明正确，注册该函数
            m_processNameTable.push_back(pentry);

        }
        else; //函数声明不正确,不会注册该函数
        functionBody(pentry, paraName);

    }
    else if (m_curWord->type == INTEGER || m_curWord->type == BEGIN || m_curWord->type == END || m_curWord->type == IF || m_curWord->type == THEN ||
        m_curWord->type == ELSE || m_curWord->type == READ || m_curWord->type == WRITE) {
        error(VOID_KEYWORD_COMBINATION);
        advance();
        return 0;
    }
    else if (m_curWord->type == SEMICOLON) {
        error(VOID_CHARACTER);
        return 0;
    }
    else {
        error(VOID_CHARACTER);
        advance();
        return 0;
    }


}

void
Parser::B(const string&funcname) {
    switch (m_curWord->type) {
    case READ: case WRITE: case IDENTIFIER: case IF:
    {
    D(funcname);
    if (m_curWord->type == SEMICOLON) {
            advance();
    }
       else {
            error(LACK_SEMICOLON);
            advance();
        }

    B_(funcname);

    }
    break;
    case END:
    {
        error(VOID_CHARACTER);
        return;
    }
    default:
        error(VOID_CHARACTER);
        advance();
    }
}

void
Parser::B_(const string&funcname) {

    switch (m_curWord->type) {
    case END:
        return;
    case READ: case WRITE: case IDENTIFIER: case IF: {
        D(funcname);

    if (m_curWord->type == SEMICOLON) {
        advance();
    }
    else {
        error(LACK_SEMICOLON);
        advance();
        return;
    }

    B_(funcname);

    }
    break;
    default:
        error(VOID_CHARACTER);
        advance();
    }

}


int
Parser::D(const string& funcname) {
    
    if (m_curWord->type == READ) {
        advance();

        if (m_curWord->type == LEFT_BRACKET) {
            advance();
        }
        else
        {
            error(LACK_LEFT_BRACKET);
            advance();
        }

        if (m_curWord->type == IDENTIFIER) {
            if (variableExist(m_curWord->name)) {//如果变量名表中存在同一层次的该变量
                advance();
            }
            else {
                error(UNDEFINED_IDENTIFIER);
                advance();
            }

        }
        if (m_curWord->type == RIGHT_BRACKET) {
            advance();
        }
        else {
            error(LACK_RIGHT_BRACKET);
            advance();
        }
    }
        
    else if (m_curWord->type == WRITE) {
        advance();
        if (m_curWord->type == LEFT_BRACKET) {
            advance();
        }
        else {
            error(LACK_LEFT_BRACKET);
            advance();
        }

        if (m_curWord->type == IDENTIFIER) {
            if (variableExist(m_curWord->name)) {//如果变量名表中存在同一层次的该变量
                advance();
            }
            else {
                error(UNDEFINED_IDENTIFIER);
                advance();

            }


        }

        if (m_curWord->type == RIGHT_BRACKET) {
            advance();
        }
        else {
            error(LACK_RIGHT_BRACKET);
            advance();
        }

    }
    else if (m_curWord->type == IDENTIFIER) {
        if (variableExist(m_curWord->name) || funcname == m_curWord->name) {//如果变量名表中存在同一层次的该变量,或者该变量为此子过程所在的函数名
            advance();
        }
        else {
            error(UNDEFINED_IDENTIFIER);
            advance();

        }

        if (m_curWord->type == ASSIGN) {
            advance();
        }
        else {
            error(VOID_CHARACTER);
            advance();
        }
        E();
    }
    else if (m_curWord->type == IF) {
        advance();
        F();

        if (m_curWord->type == THEN) {
            advance();
        }
        else {
            error(LACK_THEN);
            advance();
        }
        D(funcname);

        if (m_curWord->type == ELSE) {
            advance();

        }
        else {
            error(LACK_ELSE);
            advance();
        }
        D(funcname);
    }
    else if (m_curWord->type == SEMICOLON|| m_curWord->type == ELSE) {
    error(VOID_CHARACTER);
    return 0;
}
    else {
    error(VOID_CHARACTER);
    advance();
    return 0;
    }

    return 1;
}


int
Parser::G(){
    switch (m_curWord->type) {
    case LESS: case LESS_EQUAL: case MORE: case MORE_EQUAL: case NOT_EQUAL: case EQUAL:
        advance();
        break;
    case IDENTIFIER: case CONSTANT:
    {
        error(VOID_CHARACTER);
        return 0;
    }
    break;
    default:
        error(LACK_CONDITION);
        advance();
    }
    return 1;

}

int
Parser::K() {
    switch (m_curWord->type) {
    case IDENTIFIER: {
        if (processExist(m_curWord->name)|| variableExist(m_curWord->name)) {//存在该标识符，说明是过程名
            advance();
            K_();
            return 0;
        }
        else {
            error(UNDEFINED_IDENTIFIER);
            advance();
        }
    }
    break;
    case CONSTANT:
        advance();
        return 0;
        break;
    case SEMICOLON: case LESS: case LESS_EQUAL: case NOT_EQUAL: case MORE: case MORE_EQUAL:
    case MINUS: case THEN: case ELSE: case EQUAL: case MUTIPLY: case RIGHT_BRACKET:
        error(VOID_CHARACTER);
        return 0;

    default:
        error(VOID_CHARACTER);
        advance();
    }

}

int 
Parser::K_() {
    switch (m_curWord->type) {
    case SEMICOLON: case LESS: case LESS_EQUAL: case NOT_EQUAL: case MORE: case MORE_EQUAL:
    case MINUS: case THEN: case ELSE: case EQUAL: case MUTIPLY: case RIGHT_BRACKET:
        return 0;
    case LEFT_BRACKET:
    {
            advance();

            E();

        if (m_curWord->type == RIGHT_BRACKET) {
            advance();
        }
        else {
            error(LACK_RIGHT_BRACKET);
            advance();
        }

    }
    break;
    default:
    error(LACK_LEFT_BRACKET);
    advance();

    }
    return 0;



}

int
Parser::F() {
    if (m_curWord->type == IDENTIFIER||m_curWord->type == CONSTANT) {
        E();
        G();
        E();
    }
    else if(m_curWord->type == THEN){
        error(LACK_CONDITION);
        return 0;
    }
    else {
        error(LACK_CONDITION);
        advance();
    }
    return 0;
}

int
Parser::E() {
    switch (m_curWord->type) {
    case IDENTIFIER: case CONSTANT:
    {
    H();
    E_();

    }
    break;
    case SEMICOLON: case LESS: case LESS_EQUAL: case NOT_EQUAL: case MORE: case MORE_EQUAL:
    case THEN: case ELSE: case EQUAL: case RIGHT_BRACKET:
        error(VOID_CHARACTER);
        return 0;
   
    default: 
        error(VOID_CHARACTER);
        advance();
 }
    return 1;
}

void
Parser::E_() {
    switch (m_curWord->type)
    {
    case SEMICOLON:case LESS: case LESS_EQUAL: case NOT_EQUAL: case MORE: case MORE_EQUAL:
    case ELSE: case EQUAL:case THEN:case RIGHT_BRACKET:
        return;
    case MINUS:
    {
        advance();
        H();
        E_();
    }
    break;
    default:
        error(VOID_CHARACTER);
        advance();
    }
}

void
Parser::advance() {

    if (m_curWord->type == END_OF_FILE)
        return;
        ++m_curWord;
        if (m_curWord->type == END_OF_LINE) {
            ++m_curLine;
            advance();

        }

 
}

int
Parser::H() {
    switch (m_curWord->type) {
    case SEMICOLON: case LESS: case LESS_EQUAL: case NOT_EQUAL: case MORE: case MORE_EQUAL:
    case MINUS: case THEN: case ELSE: case EQUAL: case RIGHT_BRACKET:
        error(VOID_CHARACTER);
        return 0;

    case IDENTIFIER: case CONSTANT:
    K();
    H_();
    break;
    default:
        error(VOID_CHARACTER);
        advance();
    }
    return 1;
}

int
Parser::H_() {
    switch (m_curWord->type) {
    case SEMICOLON: case LESS: case LESS_EQUAL: case NOT_EQUAL: case MORE: case MORE_EQUAL:
    case MINUS: case THEN: case ELSE: case EQUAL: case RIGHT_BRACKET:
        return 0;
    case MUTIPLY:
    {
        advance();

        K();
        H_();
    }
    break;
    default:
        error(LACK_EXPRESSION);
        advance();
    } 
    return 0;

}


bool 
Parser::variableExist(const string& str) {
    vector<SvariableNameEntry*>::const_iterator it = m_variableNameTable.begin();
    for (;it != m_variableNameTable.end(); ++it) {
        if ((*it)->name == str&&((*it)->vlev == m_curLevel)) {
            return true;
        }
    } 
    return false;

}

bool
Parser::processExist(const string& str) {
    vector<SprocessNameEntry*>::const_iterator it = m_processNameTable.begin();
    for (;it != m_processNameTable.end(); ++it) {
        if ((*it)->pname == str && (*it)->plev <= m_curLevel) {
            return true;
        }
    return false;
    }

}

int 
Parser::getLastVariableOffset() {
    return m_variableNameTable.size() - 1;
}

Parser::~Parser() {
    int iv = 0;
    for (;iv != m_variableNameTable.size(); ++iv) {
        delete m_variableNameTable[iv];
    }

    int ip = 0;
    for (; ip != m_processNameTable.size(); ++ip) {
        delete m_processNameTable[ip];
    }
}

inline void
Parser :: error(int errorkind) {


    m_outputError << "***LINE:"<<m_curLine<<"  ";
    switch (errorkind) {
    case (LACK_BEGIN):
        m_outputError << "缺少begin关键字" << endl;
        break;
    case(LACK_CONDITION):
        m_outputError << "缺少条件" << endl;
        break;
    case(LACK_DECLARATION):
        m_outputError << "缺少声明" << endl;
        break;
    case(LACK_ELSE):
        m_outputError << "缺少else关键字" << endl;
        break;
    case(LACK_STATEMENT):
        m_outputError << "缺少执行语句" << endl;
        break;
    case(LACK_END):
        m_outputError << "end关键字不匹配" << endl;
        break;
    case(LACK_EXPRESSION):
        m_outputError << "缺少表达式" << endl;
        break;
    case(LACK_FUNCTION_DEFINATION):
        m_outputError << "缺少函数定义" << endl;
        break;
    case(LACK_IDENTIFIER):
        m_outputError << "缺少标识符" << endl;
        break;
    case(LACK_IF):
        m_outputError << "缺少if关键字" << endl;
        break;
    case(LACK_THEN):
        m_outputError << "缺少then关键字" << endl;
        break;
    case(LACK_INTEGER):
        m_outputError << "缺少integer关键字" << endl;
        break;
    case(LACK_LEFT_BRACKET):
        m_outputError << "缺少（" << endl;
        break;
    case(LACK_RIGHT_BRACKET):
        m_outputError << "括号不匹配" << endl;
        break;
    case(LACK_PARAMETER_DECLARATION):
        m_outputError << "缺少形参声明" << endl;
        break;
    case(LACK_SEMICOLON):
        m_outputError << "缺少分号" << endl;
        break;
    case(LACK_FACTOR):
        m_outputError << "缺少因子" << endl;
        break;
    case(UNDEFINED_IDENTIFIER):
        m_outputError << "未定义的标识符" << endl;
        break;
    case(VOID_KEYWORD_COMBINATION):
        m_outputError << "无效的关键字组合" << endl;
        break;
    case(REPEAT_DECLARATION):
        m_outputError << "重复声明" << endl;
        break;
    case(VOID_CHARACTER):
        m_outputError << "非法字符" << endl;
        break;
    }
}

void
Parser::writeToFile() {
    if (m_variableNameTable.size()) {
        vector<variableNameEntry*>::const_iterator it;
        m_outputVaribleTable << "name" << "    " << "adr" << "    " << "kind" << "    " << "level" << "    "
            << "processname" << "    " << "type" << endl;

        for ( it = m_variableNameTable.begin(); it != m_variableNameTable.end(); ++it) {
            m_outputVaribleTable << (*it)->name << "    " << (*it)->vadr << "    " << (*it)->vkind << "    " << (*it)->vlev << "    "
                << (*it)->vproc << "    " << (*it)->vtype << endl;
        }
    }

    if (m_processNameTable.size()) {
        vector<processNameEntry*>::const_iterator it;

        m_outputFunctionTable << "name" << "    " << "type" << "    " << "level" << "    " << "fadr" << "    "
            << "ladr" << endl;

        for (it = m_processNameTable.begin(); it != m_processNameTable.end(); ++it) {
            m_outputFunctionTable << (*it)->pname << "    " << (*it)->ptype << "    " << (*it)->plev << "    " << (*it)->fadr << "    "
                << (*it)->ladr << endl;
        }

    }
}