#ifndef ASS6_13CS10060_TRANSLATOR_H_INCLUDED
#define ASS6_13CS10060_TRANSLATOR_H_INCLUDED

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <map>
#include <iomanip>
#include <cstdlib>
#include <cstdio>

using namespace std;


typedef enum {
    /* Arithmetic Operators */
    PLUS = 1,
    MINUS,
    MULT,
    DIV,
    MOD,
    /* Shift Operators And Bitwise Operators */
    SLL,
    SRA,  //Signed shift right
    SRL,
    BitwiseAND,
    BitwiseOR,
    BitwiseXOR,

    /* Logical Operators */
	LESS,
    LEQ,
    GRT,
    GEQ,
	AND,
    OR,
    LogicalEQ,
    NotEQ,
    /* Unary operators */
	UMINUS,
    UPLUS,
    NEGATION,
    COMPLEMENT,
    /* Copy operator */
    COPY,
    /* Jump and branch operators */
    OP_GOTO,
	IFGOTO,
    IFFalseGOTO,
	IFLessGOTO,
    IFGrtGOTO,
    IFLessEqGOTO,
    IFGrtEqGOTO,
    IFLogEqGOTO,
    IFNotEqGOTO,
	/* Procedural Operators */
	CALL,
    PARAM,
	OP_RETURN,
	/* Array operators */
	ASSIGNARR,
	ARRASSIGN,
	/* Pointer Access operators */
	ASSIGNADDRESS,
	ASSIGNVALUE,
	VALUEASSIGN,
	/* Type conversion Operators */
 	Int2Dbl,
    Dbl2Int,
    Char2Int,
    Int2Char,
    COND,
    NOP,
    __SLABEL,
    __ELABEL

} opcodeType;

typedef enum{
    TEMPORARY = 1,
    LOCAL,
    RETURNVALUE,
    PARAMETER,
    GLOBAL,
    FUNCTION,
    BLOCK,
    CONSTANT_TEMP,
    UNKNOWN
} symbolCategory;

struct Value{
    enum {INT_TAG,DOUBLE_TAG,CHAR_TAG,STRING_TAG,NO_DEF} tag;
    int intvalue;
    double doublevalue;
    char charvalue;
    string strlabel;

    Value();
    Value(int value);
    Value(double value);
    Value(char value);
    Value(string strlabel);

    string getString();

};
class SymbolTableRow;
class StringTable
{
    map<string,int> strtable;
    static int count;
public:
    string lookup(string key);
    void print(ostream& out);
    string getStringsASM();


};

class LabelTable{
    map<int,int> table;
    int labelcount;
public:
    LabelTable();
    string lookup(int index);
    bool find(int index);
    void print(ostream& out);
    int getcount();
};

class QuadOperand{
public:
    enum {SYMBOLINK=1, VALUE,NA} tag;
    Value v;
    SymbolTableRow* symrow;

    QuadOperand();
    QuadOperand(SymbolTableRow*);
    QuadOperand(Value);
    QuadOperand(int);
    int isConstant();
    int getIntegerConstValue();

    string getString();
};

class SymbolTable;

class Quad;
class Type;
class ParameterList;
class Quad {
  public:
        opcodeType opcode;
        /**
        *    result : The argument on rhs of equal in all instruction where applicable
        *             The target address in case of goto opcode
        *             The parameter in case of param opcode
        *
        *    operand1 : First Operand wherever applicable else NULL
        *    operand2 : Second Operand wherever applicable else NULL
        *
        */

        QuadOperand result,operand1,operand2;



        /**
        *
        */
        Quad();
        Quad(opcodeType opcode, QuadOperand result=QuadOperand(),QuadOperand operand1=QuadOperand(),
            QuadOperand operand2=QuadOperand());

        /**
         *  Function to update the target for a goto instructions(assuming it
         *  is called for the correct quad(ie with opcode of jump types)
         */
        void updateTarget(string result);
        void print(ostream& out);
};

class QuadArray{
    vector<Quad> _quadarray;
    int nextinstruction;

public:
    QuadArray();
    //void emit(opcodeType opcode,SymbolTableRow* result=NULL,SymbolTableRow* arg1=NULL,SymbolTableRow* arg2=NULL);
    //void emit(opcodeType opcode,SymbolTableRow* result,SymbolTableRow* arg1,Value constant);
    //void emit(opcodeType opcode,SymbolTableRow* result,Value constant);
    //void emit(opcodeType opcode,Value constant);

    void emit(opcodeType opcode,QuadOperand result,QuadOperand operand1=QuadOperand(),QuadOperand operand2=QuadOperand());
    void emit(opcodeType);
    int getNextIndex();

    void backpatch(vector<int>& listinstr,int index);
    void updateLabelTable();
    Quad& get(int i);
    void print(ostream&);

    void propagateSome();

};


class Type{

public:
    string name;

    Type();
    Type(string name);
    virtual ~Type();
    virtual int getSize();
    virtual void print(std::ostream &out);
    virtual int getIdentity();
    virtual string getname();
    virtual bool isScalarType();
    virtual bool isIntegerType();
    virtual bool isArithmeticType();

};

class Basic_Type:public Type{
public:
    int size;


    Basic_Type(string name,int size=0);
    int getSize();
    void print(ostream&);
    int getIdentity();
    string getname();
    bool isScalarType();
    bool isIntegerType();
    bool isArithmeticType();

};

class Pointer;

class Array:public Type{
public:
    int size;
    Type *elemType;


    Array(int size,Type *elemType);
    int getSize();
    void print(ostream&);
    string getname();
    static Type* makeArray(Type*,int size=1);
    Pointer* getEquivalentPointer();

};
class Function:public Type{
public:
    Type* retType;
    vector<Type*> paramList;
    int n;

    Function(Type* retType,vector<Type*> paramlist,int n);
    int getSize();
    void print(ostream&);
    int getNoOfParameters();
    Type* getReturnType();
    string getname();
    bool isScalarType();
    bool isIntegerType();
    bool isArithmeticType();
    static void updateParameterList(Type *t,ParameterList* plist=NULL);
    static Type* makeFunction(Type* t);

};

class Pointer:public Type{
public:
    int size;
    Type* pointeeType;

    Pointer(Type* pointee);
    int getSize();
    void print(ostream&);
    int getIdentity();
    string getname();
    Type* getPointeeType();
    static Type* makePointer(Type *);
};

class SymbolTableRow{

public:
    string name;
    Type* type;
    symbolCategory categoryOfSymbol;
    Value initialValue;
    int size;
    int offset;
    SymbolTable* nestedTable;
    static const string RETNAME;

    SymbolTableRow(string name,Type *type=NULL,symbolCategory categoryOfSymbol=UNKNOWN,
            Value initialValue=Value(),int size=-1,int offset=-1,SymbolTable* nestedTable=NULL);

    void print(ostream&);


};

class SymbolTable{

    unordered_map<string,SymbolTableRow*> table;
    vector<string> insertionOrder;
    typedef pair<string,SymbolTableRow*> tablepair;
    static int tempNo;
    static int blockNo;
    static int paramNo;


public:
    static const int GLOBALLEVEL = 0;
    static const int FUNCTIONLEVEL = 1;
    static const int BLOCKLEVEL = 2;
    string name;
    SymbolTable* parent;
    int level;

    SymbolTable(string name,SymbolTable* parent,int level);
    SymbolTableRow* gentemp(Type* type,Value initialValue=Value(),symbolCategory categoryOfSymbol=UNKNOWN);
    SymbolTableRow* lookup(string name);
    static SymbolTable* getNewBlockTable();
    void insert(SymbolTableRow* symrow);
    SymbolTableRow* insertDummyParam(Type * t);
    void updateOffset();
    string getDataSegment();
    void print(ostream&);

    int setARoffsets(int);
    friend string transferParameters(SymbolTableRow* srow);


};


class Expression{

public:
    Expression();
    SymbolTableRow* loc;
    vector<int> truelist;
    vector<int> falselist;
    Type* type;
    SymbolTableRow* array;
    SymbolTableRow* offset;

    void swapLists();
    /**
    *
    * Promotes the value to the corresponding int value
    * Converts bool to int and promotes char to int
    * Leaves other types unchanged
    */
    Expression convertToInt();
    /**
    * Converts expression to bool if it is of type convertible to bool
    */
    void convertToBool();
    /**
    * Converts expression from bool to int if it is of type int
    */
    void convertBoolToInt();


};
class ListType{
public:
    vector<int> nextlist;
    ListType();

};

class ArgList{
public:
    vector<Expression*> args;
    ArgList();
};

class ParameterList{
public:
    vector<Type*> plist;
};
vector<int> merge(const vector<int> list1,const vector<int> list2);
vector<Type*> merge(const vector<Type*> list1,const vector<Type*> list2);
vector<int> makelist(int addr);
vector<Type*> makelist(Type* type);

int isCallable(Type *t);
Expression* callFunction(SymbolTableRow* f,ArgList *args);
/**
* This function checks if the two types are same or can be promoted to same
* using implicit promotion(and promotes if possible) and returns 1 else return 0
*/
int typecheck(Expression* e1, opcodeType opcode);
int typecheck(Expression* e1, Expression* e2,opcodeType opcode);
int typecheck(Expression* e1, Expression* e2,Expression* e3,opcodeType opcode);
int typecheck(Type* t, Expression *e );



/**Global Functions*/
string getGlobalVarString(SymbolTableRow* srow);
string getIncludeString();
string getRegisterApt(string type,int no);
string makeFunctionPrologue(SymbolTableRow* srow);
string makeFunctionEpilogue(SymbolTableRow* srow);
string getAddressExpression(SymbolTableRow* srow);
string generateCallStack(Quad q,int i,int *size);
string getASMforQuad(Quad& q,int *factive,int i);
void printASMCode(ostream& out,string filename);



const int SIZEOFINT = 4;
const int SIZEOFCHAR = 1;
const int SIZEOFDOUBLE = 8;
const int SIZEOFPOINTER = 8;
const int REGPARAMETERS = 6;

//extern const Basic_Type *INT_TYPE;
static Basic_Type* INT_TYPE = new Basic_Type("int",SIZEOFINT);
static Basic_Type *DOUBLE_TYPE = new Basic_Type("double",SIZEOFDOUBLE);
static Basic_Type *CHAR_TYPE = new Basic_Type("char",SIZEOFCHAR);
static Basic_Type *VOID_TYPE = new Basic_Type("void");
static Basic_Type *BOOL_TYPE = new Basic_Type("bool");
static Pointer *STRING_TYPE = new Pointer(CHAR_TYPE);



#endif // ASS6_13CS10060_TRANSLATOR_H_INCLUDED
