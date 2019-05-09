#include"parser.h"
#include<iostream>
Parser::Parser(const vector<STableEntry> & input)
{
	m_root = new node("program");
    
	if (!input.size())cerr << "input is empty!" << flush;
	m_curWord = input.begin();
    m_curLevel = 1;
    m_curLine = 1;
}

node *
Parser::processInput() {
    program();
    return m_root;
}

void
Parser::program() {
    node * temp = subProgram("subProgram");
    if(temp != NULL)
    m_root->child.push_back(temp);

}

node*
Parser::subProgram(const string& procname) {
    node * self =getNewNode(procname);
    if (m_curWord->type == BEGIN) {
        node *t1= getNewNode("begin", true);
        self->child.push_back(t1);
        advance();

    }
    else {
        error(LACK_BEGIN);
    }

    node* temp = NULL;

    temp = declarationTable(procname);
    if(temp != NULL)
    self->child.push_back(temp);


    temp = executiveTable();
    if(temp != NULL)
    self->child.push_back(temp);

    if (m_curWord->type == END) {
        node * t3 = getNewNode("end", true);
        self->child.push_back(t3);
        advance();
    }
    else
    {
        error(LACK_END);
    }
    return self;
}


node*
Parser::declarationTable(const string& processname, int *firstVariableAdr=NULL) {

    node* self = getNewNode("declarationTable");

    node* temp = declaration(processname);

    if (temp != NULL)
    {
    self->child.push_back(temp);
    *firstVariableAdr = getLastVariableOffset();
    }
    else
    {
        error(LACK_DECLARATION);//必须要有一个变量声明
        delete self;
        return NULL;
    }
    if (m_curWord->type == SEMICOLON) {
        node* t1 = getNewNode(";", true);
        self->child.push_back(t1);
        advance();
    }
    else
    {
        error(LACK_SEMICOLON);
    }

    while (1) {
        node* temp = declaration(processname);
        if (temp == NULL)break;
        else {
            if (m_curWord->type == SEMICOLON) {
                advance();
            }
            else error(LACK_SEMICOLON);
        }
    }


    return self;
}


node*
Parser::declaration(const string& processname) {
    node* self = getNewNode("declaration");

    if (m_curWord->type != INTEGER) {
        delete self;
        return NULL;
    }

        node* t1 = getNewNode("integer", true);
        advance();

        if (m_curWord->type == IDENTIFIER) {// 变量
            node* t2 = getNewNode(m_curWord->name, true);
            node* pvariable = getNewNode("variable");
            pvariable->child.push_back(t1);
            pvariable->child.push_back(t2);

            self->child.push_back(pvariable);

            SvariableNameEntry* entry = new SvariableNameEntry(m_curWord->name, processname, VARIABLE, m_curLevel);
            m_variableNameTable.push_back(entry);
            advance();
        }
        else if (m_curWord->type == FUNCTION) {//函数
            node* t2 = getNewNode(m_curWord->name);
            node* pfunc = getNewNode("function");
            pfunc->child.push_back(t1);
            advance();
            pfunc->child.push_back(t2);
            if (m_curWord->type == IDENTIFIER) {
                node* t3 = getNewNode(m_curWord->name, true);
                pfunc->child.push_back(t3);
                advance();
            }
            else {
                error(LACK_IDENTIFIER);
            }
            if (m_curWord->type == LEFT_BRACKET) {
                node* t4 = getNewNode(m_curWord->name, true);
                pfunc->child.push_back(t4);
                advance();
            }
            else {
                error(LACK_LEFT_BRACKET);
            }

            if (m_curWord->type == IDENTIFIER) {//形参
                node* t5 = getNewNode(m_curWord->name, true);
                pfunc->child.push_back(t5);
                SvariableNameEntry* entry = new SvariableNameEntry(m_curWord->name, processname, PARAMETER, m_curLevel + 1);
                m_variableNameTable.push_back(entry);


                advance();

            }
            else {
                error(LACK_IDENTIFIER);
            }
            if (m_curWord->type == RIGHT_BRACKET) {
                node* t6 = getNewNode(m_curWord->name, true);
                pfunc->child.push_back(t6);
                advance();
            }
            else {
                error(LACK_RIGHT_BRACKET);
            }

            if (m_curWord->type == SEMICOLON) {
                node* t7 = getNewNode(m_curWord->name, true);
                pfunc->child.push_back(t7);
                advance();
            }
            else {
                error(LACK_SEMICOLON);
            }

            if (pfunc->child.size() == 7) {//函数声明正确，注册该函数
                SprocessNameEntry* pentry = new SprocessNameEntry(pfunc->child[1]->word, m_curLevel);
                m_processNameTable.push_back(pentry);
                node* temp = NULL;
                temp = functionBody(pentry);

                //看函数定义是否正确
                if (temp != NULL)
                    pfunc->child.push_back(temp);
                else
                {//函数无定义
                    error(LACK_FUNCTION_DEFINATION);
                }

                self->child.push_back(pfunc);
            }
            else; //函数声明不正确,不会注册该函数
          

        }
        else if (m_curWord->type == INTEGER|| m_curWord->type == BEGIN||m_curWord->type == END||m_curWord->type == IF||m_curWord->type == THEN|| 
            m_curWord->type == ELSE|| m_curWord->type == READ||m_curWord->type==WRITE) {
            error(VOID_KEYWORD_COMBINATION);
        }


    return self;
}

node*
Parser::functionBody(SprocessNameEntry* curfunction) {
    ++m_curLevel;
    if (m_curWord->type != BEGIN)
    {
        error(LACK_BEGIN);//无法识别函数体
        return NULL;

    }

    node * self = getNewNode("functionBody");

        node *t1 = getNewNode("begin", true);
        self->child.push_back(t1);
        advance();

        node* temp = NULL;
        temp = declarationTable(curfunction->pname);
        if(temp != NULL)
            self->child.push_back(temp);
        else self->child.push_back(getErrorNode());

        temp = executiveTable();
        if(temp != NULL)
            self->child.push_back(executiveTable());
        else self->child.push_back(getErrorNode());

    if (m_curWord->type == END) {
        node * t3 = getNewNode("end", true);
        self->child.push_back(t3);
        advance();
    }
    else error(LACK_END);

    return self;


    --m_curLevel;
}

node*
Parser::executiveStatement() {
    node* self = getNewNode("executiveStatement");
    
    if (m_curWord->type == READ){
        node* t1 = getNewNode("read", true);
        self->child.push_back(t1);
        advance();

        if (m_curWord->type == IDENTIFIER) {
            if (variableExist(m_curWord->name)) {//如果变量名表中存在该变量
            node* t2 = getNewNode(m_curWord->name, true);
            self->child.push_back(t2);
            advance();
            }
            else {
                error(UNDEFINED_IDENTIFIER);
            }
            
        }

        return self;

        }
    else if (m_curWord->type == WRITE) {
        node* t1 = getNewNode("write", true);
        self->child.push_back(t1);
        advance();
        if (m_curWord->type == IDENTIFIER) {
            if (variableExist(m_curWord->name)) {//如果变量名表中存在该变量
                node* t2 = getNewNode(m_curWord->name, true);
                self->child.push_back(t2);
                advance();
            }
            else {
                error(UNDEFINED_IDENTIFIER);
            }

        }
        return self;
    }
    else if (m_curWord->type == IDENTIFIER) {
        node* t1 = getNewNode(m_curWord->name, true);
        self->child.push_back(t1);
        advance();

        if (m_curWord->type == ASSIGN) {
            node* t2 = getNewNode(m_curWord->name, true);
            self->child.push_back(t2);
            advance();
        }

        node* temp = NULL;
        temp = countExpression();
        if (temp != NULL)
            self->child.push_back(temp);
        else error(LACK_EXPRESSION);

        return self;
    }
    else if (m_curWord->type == IF) {
        node* t1 = getNewNode(m_curWord->name, true);
        self->child.push_back(t1);

        node* temp = NULL;
        temp = conditionStatement();
        if (temp != NULL) {
            self->child.push_back(temp);
        }
        else error(LACK_CONDITION);

        if (m_curWord->type == THEN) {
            node* t2 = getNewNode(m_curWord->name, true);
            self->child.push_back(t2);
            advance();

        }
        else error(LACK_THEN);

        temp = executiveStatement();
        if (temp != NULL)
            self->child.push_back(temp);
        
        if (m_curWord->type == ELSE) {
            node* t3 = getNewNode(m_curWord->name, true);
            self->child.push_back(t3);
            advance();

        }
        else  error(LACK_ELSE);

        temp = executiveStatement();
        if (temp != NULL)
            self->child.push_back(temp);

        return self;
    }
    else {
        delete self;
        return NULL;
    }
}


node*
Parser::relationalStatement(){
    node* self = getNewNode("relationalStatement");
    if(m_curWord->type == LESS|| m_curWord->type == LESS_EQUAL||m_curWord->type == MORE||m_curWord->type == MORE_EQUAL||m_curWord->type==NOT_EQUAL){
        node* t = getNewNode(m_curWord->name, true);
        self->child.push_back(t);
        advance();
        return self;
    }
    else {
        error(LACK_EXPRESSION);
        delete self;
        return NULL;
    }

}

node*
Parser::factor() {
    node* self = getNewNode("functionCall");

    if (m_curWord->type == IDENTIFIER){
        if (processExist(m_curWord->name)) {//过程名表中存在该标识符，说明是过程名
            node* t1 = getNewNode(m_curWord->name, true);
            self->child.push_back(t1);
            advance();
            if (m_curWord->type == LEFT_BRACKET) {
                node* t2 = getNewNode(m_curWord->name, true);
                self->child.push_back(t2);
                advance();
            }
            else error(LACK_LEFT_BRACKET);

            if (m_curWord->type == IDENTIFIER) {
                node* t2 = getNewNode(m_curWord->name, true);
                self->child.push_back(t2);
                advance();
            }
            else error(LACK_IDENTIFIER);

            if (m_curWord->type == RIGHT_BRACKET) {
                node* t3 = getNewNode(m_curWord->name, true);
                self->child.push_back(t3);
                advance();
            }
            else error(LACK_RIGHT_BRACKET);

            if (m_curWord->type == SEMICOLON) {
                node* t4 = getNewNode(m_curWord->name, true);
                self->child.push_back(t4);
                advance();
            }
            else error(LACK_SEMICOLON);
                
            return self;
        }
        else {
            node* t1 = getNewNode(m_curWord->name, true);
            self->child.push_back(t1);
            advance();
            
            return self;

        }
    }
    else if (m_curWord->type == CONSTANT) {
        node* t1 = getNewNode(m_curWord->name, true);
        self->child.push_back(t1);
        advance();
        return self;
    }
}

node*
Parser::conditionStatement() {
    node* self = getNewNode("conditionStatement");
    
    node* temp = NULL;
    temp = countExpression();
    if (temp != NULL)
        self->child.push_back(temp);
    else {
        error(LACK_EXPRESSION);
        delete self;
        return NULL;
    }
    temp = relationalStatement();
    if (temp != NULL)
        self->child.push_back(temp);
    else error(LACK_EXPRESSION);

    temp = countExpression();
    if (temp != NULL)
        self->child.push_back(temp);
    else error(LACK_EXPRESSION);
    
    return self;

}

node*
Parser::countExpression() {
    node* self = getNewNode("countExpression");

    node* temp = NULL;
    temp = item();
    if (temp != NULL)
        self->child.push_back(temp);
    else {
        delete self;
        return NULL;
    }
    while (m_curWord->type == MINUS) {
        node* t1 = getNewNode(m_curWord->name, true);

        temp = item();
        if (temp != NULL)
            self->child.push_back(temp);
        else error(LACK_ITEM);
    }

    return self;
}


void
Parser::advance() {
    ++m_curWord;
    if (m_curWord->type == END_OF_LINE)
    {
        ++m_curLine;
        advance();
    }
}

node*
Parser::getErrorNode() {
    node* error = new node("error", true);
 
    return error;
}

node*
Parser::getNewNode(const string & str, bool isterminnal) {
    node* n = new node(str, isterminnal);

    return n;
}

node*
Parser::item() {
    node* self = getNewNode("item");
    
    node*temp = NULL;
    temp = factor();
    if (temp != NULL)
        self->child.push_back(temp);
    else {
        delete(self);
        return NULL;
    }

    while (m_curWord->type == MUTIPLY) {
        node *t1 = getNewNode(m_curWord->name, true);
        self->child.push_back(t1);
        advance();

        temp = factor();
        if (temp != NULL)
            self->child.push_back(temp);
        else error(LACK_EXPRESSION);
    }

    return self;
}

