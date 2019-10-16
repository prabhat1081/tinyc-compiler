#include "ass6_13CS10060_translator.h"
using namespace std;

/**Static Variables Declaration*/
const string SymbolTableRow::RETNAME = "$retvalue";
extern QuadArray *quadsArray;
extern SymbolTable *globalST,*currentST;
extern StringTable *strTable;


/**Value union constructors*/
Value::Value():tag(NO_DEF),intvalue(0),doublevalue(0),charvalue(0),strlabel(""){}
Value::Value(int value):tag(INT_TAG),intvalue(value),doublevalue(0),charvalue(0),strlabel(""){}
Value::Value(double value):tag(DOUBLE_TAG),intvalue(0),doublevalue(value),charvalue(0),strlabel(""){}
Value::Value(char value):tag(CHAR_TAG),intvalue(0),doublevalue(0),charvalue(value),strlabel(""){}
Value::Value(string strlabel):tag(STRING_TAG),intvalue(0),doublevalue(0),charvalue(0),strlabel(strlabel){}
string Value::getString(){
    string res;
    switch(tag){
        case INT_TAG : res = to_string(intvalue);
                        break;
        case DOUBLE_TAG: res = to_string(doublevalue);
                        break;
        case CHAR_TAG : res = to_string(charvalue);
                        break;
        case STRING_TAG : res = strlabel;
                            break;
        case NO_DEF : res = "NA";
                        break;

    }
    return res;


}

/** StringTable Class*/
int StringTable::count = 0;

string StringTable::lookup(string key){
    std::map<string,int>::iterator it;
    it = strtable.find(key);
    int r = 0;
    if(it == strtable.end())
    {
        r = count;
        count = count+1;
        strtable[key] = r;
    }
    else
    {
        r = it->second;
    }
    return (".LC"+ to_string(r));
 }

void StringTable::print(ostream& out){
    std::map<string,int>::iterator it;
    out<<"Label\t\t|\tString"<<endl;
    out<<"--------------------------------------------------------------------------------------\n";
    for(it = strtable.begin(); it != strtable.end();it++){
        out<<"LC"<<it->second<<"\t|\t"<<it->first<<endl;
    }
    out<<endl;

}




/** StringTable class end */




/**Quad Class*/

Quad::Quad():opcode(NOP),result(QuadOperand()),operand1(QuadOperand()),operand2(QuadOperand()){}

Quad::Quad(opcodeType opcode,QuadOperand result,QuadOperand operand1,QuadOperand operand2):
    opcode(opcode),result(result),operand1(operand1),operand2(operand2)
    {

    }
void Quad::updateTarget(string target){
    cout<<"Target:"<<target<<endl;
    result = Value(target);
}

void Quad::print(ostream& out){
    string s;
    string sresult = result.getString();
    string soperand1 = operand1.getString();
    string soperand2 = operand2.getString(); 
    switch(opcode){
        case PLUS : s = sresult + " = " + soperand1 + " + " + soperand2;
                    break;
        case MINUS : s = sresult + " = " + soperand1 + " - " + soperand2;
                    break;
        case MULT : s = sresult + " = " + soperand1 + " * " + soperand2;
                    break;
        case DIV : s = sresult + " = " + soperand1 + " / " + soperand2;
                    break;
        case MOD : s = sresult + " = " + soperand1 + " % " + soperand2;
                    break;
        case SLL : s = sresult + " = " + soperand1 + " << " + soperand2;
                    break;
        case SRL : s = sresult + " = " + soperand1 + " >>> " + soperand2;
                    break;
        case SRA : s = sresult + " = " + soperand1 + " >> " + soperand2;
                    break;
        case BitwiseAND : s = sresult + " = " + soperand1 + " & " + soperand2;
                    break;
        case BitwiseOR : s = sresult + " = " + soperand1 + " | " + soperand2;
                    break;
        case BitwiseXOR : s = sresult + " = " + soperand1 + " ^ " + soperand2;
                    break;
        case LESS : s = sresult + " = " + soperand1 + " < " + soperand2;
                    break;
        case LEQ : s = sresult + " = " + soperand1 + " <= " + soperand2;
                    break;
        case GRT : s = sresult + " = " + soperand1 + " > " + soperand2;
                    break;
        case AND : s = sresult + " = " + soperand1 + " && " + soperand2;
                    break;
        case OR : s = sresult + " = " + soperand1 + " || " + soperand2;
                    break;
        case LogicalEQ : s = sresult + " = " + soperand1 + " == " + soperand2;
                    break;
        case NotEQ : s = sresult + " = " + soperand1 + " != " + soperand2;
                    break;
        case UMINUS : s = sresult + " =  - " + soperand1;
                    break;
        case UPLUS : s = sresult + " =  + " + soperand1;
                    break;
        case NEGATION : s = sresult + " =  ! " + soperand1;
                    break;
        case COMPLEMENT: s = sresult + " =  ~ " + soperand1;
                    break;
        case COPY: s = sresult + " = " + soperand1;
                    break;
        case OP_GOTO: s = "goto " + sresult;
                    break;
        case IFGOTO: s = "if " + soperand1 + " goto " + sresult;
                    break;
        case IFFalseGOTO: s = "ifFalse " + soperand1 + " goto " + sresult;
                    break;
        case IFLessGOTO: s = "if " + soperand1 + " < " + soperand2 +  " goto " + sresult;
                    break;
        case IFGrtGOTO: s = "if " + soperand1 + " > " + soperand2 +  " goto " + sresult;
                    break;
        case IFLessEqGOTO: s = "if " + soperand1 + " <= " + soperand2 +  " goto " + sresult;
                    break;
        case IFGrtEqGOTO: s = "if " + soperand1 + " >= " + soperand2 +  " goto " + sresult;
                    break;
        case IFLogEqGOTO: s = "if " + soperand1 + " == " + soperand2 +  " goto " + sresult;
                    break;
        case IFNotEqGOTO: s = "if " + soperand1 + " != " + soperand2 +  " goto " + sresult;
                    break;
        case CALL : s = sresult + " = call " + soperand1 + " , " + soperand2;
                    break;
        case PARAM : s = "param " + sresult;
                    break;
        case OP_RETURN: s = "return " + sresult;
                    break;
        case ASSIGNARR: s = sresult + " = " + soperand1 + "[ "+soperand2 +" ]";
                    break;
        case ARRASSIGN: s = sresult + " [ " + soperand1 + " ] = " + soperand2;
                    break;
        case ASSIGNADDRESS: 
                    s = sresult +  " = &" + soperand1;
                    break;
        case ASSIGNVALUE: s = sresult + " = *" + soperand1;
                    break;
        case VALUEASSIGN: s = "*"+ sresult + " = " + soperand1;
                    break;
        case Int2Dbl : s = sresult + " = Int2Dbl(" + soperand1 + ")";
                    break;
        case Dbl2Int : s = sresult + " = Dbl2Int(" + soperand1 + ")";
                    break;
        case Char2Int : s = sresult + " = Char2Int(" + soperand1 + ")";
                    break;
        case Int2Char : s = sresult + " = Int2Char(" + soperand1 + ")";
                    break;
        case NOP: s = "some no op error";
                    break;
        case __SLABEL: s = "LFB:" + sresult;
                    break;
        case __ELABEL: s = "LFE:" + sresult;
                    break;
        default: s = "Some Error";
                    break;
    }


    out<<s<<endl;


}

/**End of quad class*/

/**Class QuadArray*/

QuadArray::QuadArray():_quadarray(vector<Quad>()),nextinstruction(0){}

void QuadArray::emit(opcodeType opcode,QuadOperand result,QuadOperand operand1,QuadOperand operand2){
    Quad q(opcode,result,operand1,operand2);
    _quadarray.push_back(q);
    nextinstruction++;

}

void QuadArray::emit(opcodeType opcode){
    Quad q(opcode);
    _quadarray.push_back(q);
    nextinstruction++;
}

/*

void QuadArray::emit(opcodeType opcode,SymbolTableRow* result,SymbolTableRow* arg1,SymbolTableRow* arg2){
    Quad q(opcode,result,arg1,arg2);
    _quadarray.push_back(q);
    nextinstruction++;
}

void QuadArray::emit(opcodeType opcode,SymbolTableRow* result,SymbolTableRow* arg1,Value constant){
    emit(opcode,result,arg1,constant);

}

void QuadArray::emit(opcodeType opcode,SymbolTableRow* result,Value constant)
{
    emit(opcode,result,constant);

}

void QuadArray::emit(opcodeType opcode,Value constant){
    emit(opcode,constant);
}
*/
int QuadArray::getNextIndex(){
    return nextinstruction;
}

void QuadArray::backpatch(vector<int>& listinstr,int index){
    Value target = Value(index);
    int size = listinstr.size();
    for(int i=0;i<size;i++){
        int id = listinstr[i];
        //Quad tmp = _quadarray[id];
        //Quad q(tmp.opcode,target,tmp.operand1,tmp.operand2);

        _quadarray[id].result = target;
        //cout<<"i : Target:"<<id<<" : "<<target<<endl;
    }
    listinstr.clear();

}

void QuadArray::print(ostream& out){
    for(int i=0;i<nextinstruction;i++){
        out<<i<<":\t";
        _quadarray[i].print(out);
    }

}

Quad& QuadArray::get(int index){
    return _quadarray[index];
}


/**End of Quad Array*/


/**Type class*/

Type::Type():name("unknown"){}
Type::Type(string name):name(name){}
Type::~Type(){}
int Type::getSize(){
    return 0;
}
void Type::print(std::ostream &out){
    string s = "Unknown Type";
    out <<name;

}
int Type::getIdentity(){
    return -1;
}
string Type::getname(){
    return name;
}
bool Type::isScalarType(){
    return false;
}
bool Type::isIntegerType(){
    return false;
}
bool Type::isArithmeticType(){
    return false;
}

/**class Basic_Type*/

Basic_Type::Basic_Type(string name,int size):Type(name),size(size){}

int Basic_Type::getSize(){return size;}

void Basic_Type::print(ostream &out){
        out<<name;
}

int Basic_Type::getIdentity(){
    return 1;
}

string Basic_Type::getname(){
    return name;
}

bool Basic_Type::isScalarType(){
    return true;
}

bool Basic_Type::isIntegerType(){
    return true;
}

bool Basic_Type::isArithmeticType(){
    return true;
}

/**End of Basic_Type*/

/**class Array*/

Array::Array(int size,Type *elemType):Type("array"),size(size),elemType(elemType){}

int Array::getSize(){
    return size*elemType->getSize();
}

void Array::print(ostream& out){
    string s;
    out<<"array( "<<size<<" , ";
    elemType->print(out);
    out<<" ) ";
}

string Array::getname(){
    return name;
}

Type* Array::makeArray(Type* type,int size){
    if(type->getname()=="array"){
        Array *old = dynamic_cast<Array*>(type);
        Array *ar = new Array(size,old->elemType);
        old->elemType = ar;
        return old;
    }
    else{
        return (new Array(size,type));
    }
    return type;

    /*TODO : decide how to make others array*/

}

/**End of class Array*/

/** Class Function*/

Function::Function(Type* retType,vector<Type*> paramlist,int n):Type("function"),retType(retType),paramList(paramlist),n(n){}

int Function::getSize(){
    return -1;
}

void Function::print(ostream& out){
    string s;
    s = " <-- ";
    retType->print(out);
    out<< s;
    int i;
    for(i=0;i<n-1;i++){
        paramList[i]->print(out);
        s = " X ";
        out<< s;
    }
    if(n>0)
        paramList[i] -> print(out);
    else{
        out<<"void";
    }
}

int Function::getNoOfParameters(){
    return n;
}

Type* Function::getReturnType(){
    return retType;
}

string Function::getname(){
    return name;
}

void Function::updateParameterList(Type* t,ParameterList* plist){
    if(t->getname() == "function"){
        Function *func = dynamic_cast<Function*>(t);
        if(plist!=NULL){
            func->n = plist->plist.size();
            func->paramList = plist->plist;
        }
    }
    else if(t->getname() == "pointer"){
        Pointer* p = (Pointer*)(t);
        updateParameterList(p->pointeeType,plist);
        

    }
}
Type* Function::makeFunction(Type* t){
    if(t->name != "array" && t->name != "pointer" && t->name != "function"){
        Function *f = new Function(t,vector<Type*>(),0);
        return f;
    }
    else if(t->name == "pointer"){
        Pointer* p = dynamic_cast<Pointer*>(t);
        Type* tt = makeFunction(p->getPointeeType());
        p->pointeeType = tt;
        return p;
    }
    else if(t->name == "array"){
        Array *ar = dynamic_cast<Array*>(t);
        Type *tt = makeFunction(ar->elemType);
        ar->elemType = tt;
        return ar;
    }

    return t;

}

bool Function::isArithmeticType(){
    return false;
}

bool Function::isIntegerType(){
    return false;
}

bool Function::isScalarType(){
    return false;
}

/**End of function class*/

/**Class Pointer*/

Pointer::Pointer(Type* pointee):Type("pointer"),size(SIZEOFPOINTER),pointeeType(pointee){}
int Pointer::getSize(){
    return size;
}
void Pointer::print(ostream& out){
    string s;
    s = "pointer( ";
    out<<s;
    pointeeType->print(out);
    s = " ) ";
    out<<s;
}

int Pointer::getIdentity(){
    return pointeeType->getSize();

}

string Pointer::getname(){
    return name;
}

Type* Pointer::getPointeeType(){
    return pointeeType;
}

Type* Pointer::makePointer(Type *t)
{
    if(t->getname() == "function"){
        Function* f = (Function*)t;
        f->retType = new Pointer(f->retType);
        return f;
    }
    else if(t->getname() == "array"){
        Array *ar = (Array*)t;
        ar->elemType = new Pointer(ar->elemType);
        return ar;
    }

    return (new Pointer(t));
}

/**End of pointer class*/

/**Class Symbol Table Row*/

SymbolTableRow::SymbolTableRow(string name,Type *type,symbolCategory categoryOfSymbol,
            Value initialValue,int size,int offset,SymbolTable* nestedTable):
            name(name),type(type),categoryOfSymbol(categoryOfSymbol),initialValue(initialValue),
            size(size),offset(offset),nestedTable(nestedTable){}

void SymbolTableRow::print(ostream& out){
    out<<name<<"\t|\t";
    type->print(out);
    out<<"\t|\t";
    switch(categoryOfSymbol){
        case TEMPORARY : out<<"temporary";
                            break;
        case LOCAL : out<<"local";
                        break;
        case RETURNVALUE : out<<"retVal";
                        break;
        case PARAMETER: out<<"parameter";
                        break;
        case GLOBAL: out<<"global";
                        break;
        case FUNCTION: out<<"function";
                        break;
        case BLOCK : out<<"block";
                        break;
        case CONSTANT_TEMP: out<<"constant";
                        break;
        case UNKNOWN: out<<"unknown";
                        break;
    }
    out<<"\t|\t"<<initialValue.getString()<<"\t|\t"<<size<<"\t|\t"<<offset<<"\t|\t";
    if(nestedTable != NULL){
        out<<nestedTable->name;
    }
    else out<<"NONE";
    out<<"\n";

}

/**End of symbol table row*/

/**class Symbol Table*/

int SymbolTable::blockNo = 0;
int SymbolTable::paramNo = 0;
int SymbolTable::tempNo = 0;

SymbolTable::SymbolTable(string name,SymbolTable* parent,int level):
    table(unordered_map<string,SymbolTableRow*>()),insertionOrder(vector<string>()),name(name),parent(parent),level(level){}

SymbolTableRow* SymbolTable::gentemp(Type* type,Value initialValue,symbolCategory categoryOfSymbol)
{
    string name = "$t"+to_string(tempNo);
    if(categoryOfSymbol != CONSTANT_TEMP){
        categoryOfSymbol = TEMPORARY;
    }
    tempNo++;
    SymbolTableRow *symrow = new SymbolTableRow(name,type,categoryOfSymbol,initialValue,type->getSize());
    table.insert(tablepair(name,symrow));
    insertionOrder.push_back(name);
    return symrow;
}

SymbolTableRow* SymbolTable::lookup(string name){
    std::unordered_map<std::string,SymbolTableRow*>::const_iterator it = table.find (name);
    if(it == table.end()){
        if(level == GLOBALLEVEL){
            return NULL;
        }
        else
        {
            return (parent->lookup(name));
        }
    }
    return it->second;
}

SymbolTable* SymbolTable::getNewBlockTable(){
    string name = "$block"+to_string(blockNo);
    blockNo++;
    SymbolTable *symtab = new SymbolTable(name,NULL,BLOCKLEVEL);
    SymbolTableRow* symrow = new SymbolTableRow(name,VOID_TYPE,BLOCK);
    symrow->nestedTable = symtab;
    currentST->insert(symrow);
    return symtab;

}

void SymbolTable::insert(SymbolTableRow* symrow){
    table.insert(tablepair(symrow->name,symrow));
    insertionOrder.push_back(symrow->name);

}

SymbolTableRow* SymbolTable::insertDummyParam(Type * t){
    string name = "$param"+to_string(paramNo);
    paramNo++;

    SymbolTableRow *symrow = new SymbolTableRow(name,t,PARAMETER);
    table.insert(tablepair(name,symrow));
    insertionOrder.push_back(name);
    return symrow;


}

void SymbolTable::updateOffset(){
    int offset = 0;
    for(auto it = insertionOrder.begin(); it!= insertionOrder.end(); ++it){
        SymbolTableRow* symrow = table.find(*it)->second;
        int size = symrow->type->getSize();
        symrow->size = size;
        symrow->offset = offset;
        offset = offset + (size>0?size:0);

    }

}

void SymbolTable::print(ostream& out){
    out<<"-------------------------------------------------------------------------------------------------"<<endl;
    out<<"Name : "<<name<<"\t\t"<<"Parent : ";
    if(parent==NULL){
        out<<"None"<<endl;
    }
    else
        out<<parent->name<<endl;
    out<<"Level : ";
    switch(level){
        case GLOBALLEVEL: out<<"GLOBAL";
                            break;
        case BLOCKLEVEL: out<<"BLOCK";
                            break;
        case FUNCTIONLEVEL: out<<"FUNCTION";
                            break;
    }
    out<<endl;
    out<<"-----------------------------------------------------------------------------------------------"<<endl;
    out<<"Name\t|\tType\t\t|\tCategory\t\t|\tInitialValue\t|\tSize\t|\tOffset\t|\tNestedTable\n";
    out<<"------------------------------------------------------------------------------------------------"<<endl;
    vector<SymbolTable*> queuer;
    for(auto it = insertionOrder.begin(); it!= insertionOrder.end(); ++it){
        SymbolTableRow* symrow = table.find(*it)->second;
        symrow->print(out);
        if(symrow->nestedTable != NULL){
            queuer.push_back(symrow->nestedTable);
        }
    }
    out<<"------------------------------------------------------------------------------------------------\n\n"<<endl;
    int sizeo = queuer.size();
    for(int  i=0;i<sizeo;i++){
        queuer[i]->print(out);
    }

}

/**End of SymbolTable class*/

/**Class Expression*/

Expression::Expression():loc(NULL),truelist(vector<int>()),falselist(vector<int>()),type(NULL),array(NULL),offset(NULL){}

void Expression::swapLists(){
    vector<int> tmp = truelist;
    truelist = falselist;
    falselist = tmp;
}

/**
* Converts expression to bool if it is of type convertible to bool
*/
void Expression::convertToBool(){
    if(type->isScalarType() && type->name != "bool"){
        quadsArray->emit(OP_GOTO,quadsArray->getNextIndex()+3);
        falselist = makelist(quadsArray->getNextIndex());
        quadsArray->emit(IFFalseGOTO,-1,loc);
        truelist = makelist(quadsArray->getNextIndex());
        quadsArray->emit(OP_GOTO,-1);
        type = BOOL_TYPE;
    }
}
/**
* Converts expression from bool to int if it is of type bool
*/
void Expression::convertBoolToInt(){
    if(type->name == "bool"){
        loc = currentST->gentemp(INT_TYPE);
        quadsArray->backpatch(truelist,quadsArray->getNextIndex());
        quadsArray->emit(COPY,loc,1);
        quadsArray->emit(OP_GOTO,quadsArray->getNextIndex()+2);
        quadsArray->backpatch(falselist,quadsArray->getNextIndex());
        quadsArray->emit(COPY,loc,0);
        type = INT_TYPE;
        truelist.clear();
        falselist.clear();
    }

}


/**End of class Expression*/

ListType::ListType():nextlist(vector<int>()){}

ArgList::ArgList():args(vector<Expression*>()){}

/**Global Functions*/

vector<int> merge(const vector<int> list1,const vector<int> list2){
    vector<int> result;
    result.insert(result.end(),list1.begin(),list1.end());
    result.insert(result.end(),list2.begin(),list2.end());
    return result;
}
vector<Type*> merge(const vector<Type*> list1,const vector<Type*> list2)
{
    vector<Type*> result;
    result.insert(result.end(),list1.begin(),list1.end());
    result.insert(result.end(),list2.begin(),list2.end());
    return result;
}
vector<int> makelist(int addr)
{
    vector<int> result;
    result.push_back(addr);
    return result;
}
vector<Type*> makelist(Type* type)
{
    vector<Type*> result;
    result.push_back(type);
    return result;

}

int isCallable(Type *t){
    if(t->getname() == "function"){
        return 1;
    }
    else if(t->getname() == "pointer"){
        Pointer *p = (Pointer*)t;
        Type *t = p->pointeeType;
        if(t->getname() == "function")
            return 1; 
    }
    return 0;
}

Expression* callFunction(SymbolTableRow* f,ArgList *args){
    /**Call Function here*/
    Function *type;
    if(f->type->getname() == "pointer"){
        Pointer *p = (Pointer*)(f->type);
        type = (Function*)p->pointeeType;
    }
    else
        type = (Function*)f->type;
    for(int i=0;i<type->n;i++){
        typecheck(type->paramList[i],args->args[i]);
        quadsArray->emit(PARAM,args->args[i]->loc);
    }
    Expression *e = new Expression();
    e->loc = currentST->gentemp(type->retType);
    e->type = type->retType;
    quadsArray->emit(CALL,e->loc,f,type->n);
    return e;

}

int typecheck(Expression* e1, opcodeType opcode){


    /*Not requires sicne correctness is assumed*/
    return 1;
}
int typecheck(Expression* e1, Expression* e2,opcodeType opcode){
    return 1;
    if(e1->type->name == e2->type->name)
		return 1;

	SymbolTableRow *temp;

	if(opcode == COPY)
	{
		temp = currentST->gentemp(e1->type);
		if(e1->type->name == "int" && e2->type->name == "double")
			quadsArray->emit(Dbl2Int, temp,e2->loc);
		else if(e1->type->name == "double" && e2->type->name == "int")
			quadsArray->emit(Int2Dbl, temp, e2->loc);
		else if(e1->type->name == "char" && e2->type->name == "int")
			quadsArray->emit(Int2Char, temp, e2->loc);
		else if(e1->type->name == "int" && e2->type->name == "char")
			quadsArray->emit(Char2Int, temp, e2->loc);
		e2->loc = temp;
		e2->type = temp->type;
	}
	else
	{

        if(e1->type->name == "double" && e2->type->name == "int")
        {
            temp = currentST->gentemp(e1->type);
            quadsArray->emit(Int2Dbl,temp,e2->loc);
            e2->loc = temp;
			e2->type = temp->type;

        }
        else if(e1->type->name == "int" && e2->type->name == "char")
        {
            temp = currentST->gentemp(e1->type);
            quadsArray->emit(Char2Int,temp,e2->loc);
            e2->loc = temp;
			e2->type = temp->type;

        }
        else if(e2->type->name == "int" && e1->type->name == "char")
        {
            temp = currentST->gentemp(e2->type);
            quadsArray->emit(Char2Int,temp,e1->loc);
            e1->loc = temp;
			e1->type = temp->type;

        }
        else if(e2->type->name == "double" && e1->type->name == "int")
        {
            temp = currentST->gentemp(e2->type);
            quadsArray->emit(Int2Dbl,temp,e1->loc);
            e1->loc = temp;
			e1->type = temp->type;

        }
    }

    return 1;
}

int typecheck(Type* t, Expression *e ){
    return 1;
    if(t->name == e->type->name)
        return 1;
    SymbolTableRow* temp = currentST->gentemp(t);
    if(t->name == "double" && e->type->name == "int")
        quadsArray->emit(Int2Dbl, temp, e->loc);
    else if(t->name == "int" && e->type->name == "char")
        quadsArray->emit(Char2Int, temp, e->loc);
    e->loc = temp;
    e->type = temp->type;
    return 1;
}

int typecheck(Expression* e1, Expression* e2,Expression* e3,opcodeType opcode){
    return 1;
    /*Not req*/
}


