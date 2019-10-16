#include "ass6_13CS10060_translator.h"

using namespace std;

/**Static Variables Declaration*/
extern int yyparse();
extern FILE* yyin;
extern int yydebug;
extern QuadArray *quadsArray;
extern SymbolTable *globalST,*currentST;
extern StringTable *strTable;

LabelTable* _labeltable;

void QuadArray::updateLabelTable(){
    for (int i = 0; i < nextinstruction; ++i)
    {
        Quad q = _quadarray[i];
        switch(q.opcode){
            case OP_GOTO:
            case IFGOTO:
            case IFFalseGOTO:
            case IFLessGOTO:
            case IFGrtGOTO:
            case IFLessEqGOTO:
            case IFGrtEqGOTO:
            case IFLogEqGOTO:
            case IFNotEqGOTO: 
                    string s  = _labeltable->lookup(atoi(q.result.getString().c_str()));
                    _quadarray[i].result = Value(s);
        }
    }

}

Pointer* Array::getEquivalentPointer(){
    if(elemType->name != "array")
        return (new Pointer(elemType));
    Array* t = (Array*)elemType;
    return new Pointer(t->getEquivalentPointer());
}

/*Quad Operand Class*/
QuadOperand::QuadOperand():tag(NA),v(Value()),symrow(NULL){}
QuadOperand::QuadOperand(SymbolTableRow* symrow):tag(SYMBOLINK),v(Value()),symrow(symrow){}
QuadOperand::QuadOperand(Value v):tag(VALUE),v(v),symrow(NULL){}
QuadOperand::QuadOperand(int index):tag(VALUE),v(Value(index)),symrow(NULL){}


string QuadOperand::getString(){
    if(tag==SYMBOLINK){
        return symrow->name;
    }
    else if(tag == VALUE){
        return v.getString();
    }
    return "";
}

int QuadOperand::isConstant(){
    if(tag == VALUE)
    {
        return 1;
    }
    return 0;
        
    //if(tag == SYMBOLINK && symrow->categoryOfSymbol == symbolCategory::CONSTANT_TEMP)
        //return 1;
}

int QuadOperand::getIntegerConstValue(){
    Value vv;
    if(tag == VALUE){
        vv = v;
    }
    else if(tag == SYMBOLINK && symrow!=NULL){
        if(symrow->categoryOfSymbol == symbolCategory::CONSTANT_TEMP){
            vv = symrow->initialValue;
        }
    }
    if(vv.tag == Value::INT_TAG){
        return vv.intvalue;
    }
    else if(vv.tag == Value::CHAR_TAG){
        return (int)vv.charvalue;
    }
    return 0;
}
/**End of QuadOperand Class*/

/** Label Table Class */
LabelTable::LabelTable():labelcount(0){}

string LabelTable::lookup(int index){
    std::map<int,int>::iterator it;
    it = table.find(index);
    int r = 0;
    if(it == table.end())
    {
        r = labelcount;
        labelcount = labelcount+1;
        table[index] = r;
    }
    else
    {
        r = it->second;
    }
    return ("L"+ to_string(r));
}

bool LabelTable::find(int index){
    std::map<int,int>::iterator it;
    it = table.find(index);
    return (it != table.end());
}

void LabelTable::print(ostream& out){
    std::map<int,int>::iterator it;
    out<<"Label\t\t|\tQuad Index"<<endl;
    out<<"--------------------------------------------------------------------------------------\n";
    for(it = table.begin(); it != table.end();it++){
        out<<"L"<<it->second<<"\t|\t"<<it->first<<endl;
    }
    out<<endl;
}

int LabelTable::getcount(){
    return labelcount;
}
/**Label Table End*/
/**
void QuadArray::propagateSome(){
    for (int i = 1; i < nextinstruction; ++i)
    {
        Quad q = _quadarray[i];
        Quad p = _quadarray[i-1];
        if(q.opcode == COPY)// && (p.opcode==COPY || p.opcode == ASSIGNVALUE || p.opcode == ASSIGNADDRESS))
        {
            if(q.operand1.getString() == p.result.getString()){
                //_quadarray[i].operand1 = p.operand1;
                //_quadarray[i].opcode = p.opcode;
               _quadarray[i-1].result = q.result;
            }

        }
    }
}
*/

string getGlobalVarString(SymbolTableRow* srow){
    int size = srow->size;
    string s = "";
    if(srow->initialValue.tag == Value::NO_DEF){
        s = "\t.comm\t"+srow->name+","+to_string(size)+"\n";
    }
    else
    {
        s += "\t.globl\t"+srow->name+"\n";
        s += "\t.align\t"+to_string(size)+"\n";
        s += "\t.type\t"+srow->name+", @object\n";
        s += "\t.size\t "+srow->name+","+to_string(size)+"\n";
        s += srow->name+":\n";
        switch(size){
            case 1: s += "\t.byte\t"+to_string((int)srow->initialValue.charvalue)+"\n";
                break;
            case 4: s += "\t.long\t"+srow->initialValue.getString()+"\n";
                break;
            case 8: s += "\t.quad\t"+srow->initialValue.getString()+"\n";
                break;

        }
    }

    return s;

}



string SymbolTable::getDataSegment(){
    string s = "";
    if(name != "global") return s;
    int data = 0;
    for(auto it = insertionOrder.begin(); it!= insertionOrder.end(); ++it){
        SymbolTableRow* symrow = table.find(*it)->second;
        if(symrow->type->name != "function" && symrow->categoryOfSymbol == GLOBAL){
            s += getGlobalVarString(symrow);
            data = 1;
        }
    }
    return s;
}

string StringTable::getStringsASM(){
    std::map<string,int>::iterator it;
    string s = "";
    int data = 0;
    for(it = strtable.begin(); it != strtable.end();it++){
        data = 1;
        s += ".LC"+to_string(it->second)+":\n";
        s += "\t.string "+it->first+"\n";
    }
    if(data) s = "\t.section\t.rodata\n" + s;
    return s;
}

string getIncludeString(){
    string s = "";
    s += "int prints(char * str);\n";
    s += "int printi(int i);\n";
    s += "int readi(int *eP);\n";
    s += "char readchar(int *eP);\n";
    s += "int printchar(char ch);\n";
    return s;
}

int SymbolTable::setARoffsets(int offset=0){
    if(level == SymbolTable::BLOCKLEVEL){
        for (std::vector<string>::iterator i = insertionOrder.begin(); i != insertionOrder.end(); ++i)
        {
            SymbolTableRow* srow = table.find(*i)->second;
            if(srow->nestedTable == NULL){
                offset -= srow->size;
                srow->offset = offset;
            }
            else
            {
                offset = srow->nestedTable->setARoffsets(offset);
                srow->offset = -1;
            }
        }
        return offset;
    }
    else if(level == SymbolTable::FUNCTIONLEVEL){
        std::vector<string>::iterator i;
        int poffset = 2*SIZEOFPOINTER;
        int offset = 0;
        int count = 0;
        for(i=insertionOrder.begin(); i != insertionOrder.end(); i++){
            SymbolTableRow* srow = table.find(*i)->second;
            if(srow->categoryOfSymbol != PARAMETER) break;
            if(srow->type->name == "array")
            {
                Array* t = (Array*)srow->type;
                srow->type = t->getEquivalentPointer();
                srow->size = srow->type->getSize();
            }
            if(count < REGPARAMETERS){
                offset -= srow->size;
                srow->offset = offset;
                count++;
            }
            else
            {
                srow->offset = poffset;
                poffset += srow->size;
            }
        }
        for(;i != insertionOrder.end(); i++){
            SymbolTableRow* srow = table.find(*i)->second;
            if(srow->nestedTable == NULL){
                offset -= srow->size;
                srow->offset = offset;

            }
            else
            {
                offset = srow->nestedTable->setARoffsets(offset);
                srow->offset = -1;
            }
        }
        return offset;
    }
    else if(level == SymbolTable::GLOBALLEVEL){
        std::vector<string>::iterator i;
        for(i=insertionOrder.begin(); i != insertionOrder.end(); i++){
            SymbolTableRow* srow = table.find(*i)->second;
            if(srow->nestedTable != NULL){
                int off = srow->nestedTable->setARoffsets();
                srow->offset = -off;

            }
        }
        return 0;
    }
    return 0;
}

string getRegisterApt(string type,int no){
    string s;
    if(type=="char"){
        switch(no){
            case 0: s = "dil";
                    break;
            case 1: s = "sil";
                    break;
            case 2: s = "dl";
                    break;
            case 3: s = "cl";
                    break;
            case 4: s = "r8b";
                    break;
            case 5: s = "r9b";
                    break;
        }
    }
    else if(type == "int"){
        switch(no){
            case 0: s = "edi";
                    break;
            case 1: s = "esi";
                    break;
            case 2: s = "edx";
                    break;
            case 3: s = "ecx";
                    break;
            case 4: s = "r8d";
                    break;
            case 5: s = "r9d";
                    break;
        }
    }
    else
    {
        switch(no){
            case 0: s = "rdi";
                    break;
            case 1: s = "rsi";
                    break;
            case 2: s = "rdx";
                    break;
            case 3: s = "rcx";
                    break;
            case 4: s = "r8";
                    break;
            case 5: s = "r9";
                    break;
        }

    }
    return s;
}


string transferParameters(SymbolTableRow* symrow){
    Function *f= (Function*)symrow->type;
    int n = f->n;
    string regname,s;
    SymbolTable* ftable = symrow->nestedTable;
    for(int i=0;i<REGPARAMETERS && i <n;i++){
        SymbolTableRow* srow = ftable->table.find(ftable->insertionOrder[i])->second;
        if(srow->type->name == "char"){
            regname = getRegisterApt("char",i);
            s += "\tmovb\t%"+regname+", "+to_string(srow->offset)+"(%rbp)\n";
        }
        else if(srow->type->name == "int"){
            regname = getRegisterApt("int",i);
            s += "\tmovl\t%"+regname+", "+to_string(srow->offset)+"(%rbp)\n";
        }
        else {
            regname = getRegisterApt("",i);
            s += "\tmovq\t%"+regname+", "+to_string(srow->offset)+"(%rbp)\n";
        }        
    }
    return s;
}

string makeFunctionPrologue(SymbolTableRow* srow){
    static int c = 0;
    string s = "";
    s += "\t.text\n";
    s += "\t.globl "+srow->name+"\n";
    s += "\t.type "+srow->name+", @function\n";
    s += srow->name + ":\n";
    s += ".LFB"+to_string(c++)+":\n";
    s += "\tpushq\t%rbp\n";
    s += "\tmovq\t%rsp, %rbp\n";
    s += "\tsubq\t$"+to_string(srow->offset)+", %rsp\n";
    s += transferParameters(srow);
    return s; 
}

string makeFunctionEpilogue(SymbolTableRow* srow){
    static int c = 0;
    string s = "";
    s += "\taddq\t$"+to_string(srow->offset)+", %rsp\n";
    s += "\tpopq\t%rbp\n";
    s += "\tret\n";
    s += ".LFE"+to_string(c++)+":\n";
    s += "\t.size\t"+srow->name+" ,.-"+srow->name+"\n";
    return s;
}


string getAddressExpression(SymbolTableRow* srow){
    string s = "";
    if(srow->categoryOfSymbol == symbolCategory::GLOBAL){
        //Not sure of the array thing

        if(srow->type->name == "array"){
            s = "$"+srow->name;
        }
        else
            s = srow->name+"(%rip)";
    }
    else{
        s = to_string(srow->offset)+("(%rbp)");
    }
    //cout<<"NAme:"<<srow->name<<"::"<<s<<endl;
    return s;

}

string generateCallStack(Quad q,int i,int *size){
    int n = q.operand2.v.intvalue;
    int si = 0;
    string s="";
    SymbolTable *ftable = q.operand1.symrow->nestedTable;
    while(n > 6 && i >= 0){
        i = i - 1;
        Quad p = quadsArray->get(i);
        if(p.opcode == PARAM){
            SymbolTableRow* srow = p.result.symrow;
            if(srow->type->name == "char"){
                si = si + 4;
                s += "\tsubq $"+to_string(4)+", %rsp\n";
                s += "\tmovsbl\t"+getAddressExpression(srow)+", %eax\n";
                s += "\tmovl\t %eax, (%rsp)\n";
                
            }
            else if(srow->type->name == "int"){
                si = si + 4;
                s += "\tsubq $"+to_string(4)+", %rsp\n";
                s += "\tmovl\t"+getAddressExpression(srow)+", %eax\n";
                s += "\tmovl\t %eax, (%rsp)\n";
                
            }
            else if(srow->type->name == "array"){
                si = si + 8;
                s += "\tsubq $"+to_string(8)+", %rsp\n";
                s += "\tleaq\t"+getAddressExpression(srow)+", %rax\n";
                s += "\tmovq\t %rax, (%rsp)\n";
                
            }
            else
            {
                si = si + 8; 
                s += "\tsubq $"+to_string(8)+", %rsp\n";
                s += "\tmovq\t"+getAddressExpression(srow)+" %rax\n";
                s += "\tmovq\t %rax, (%rsp)\n";
                
            }
            n--;
        }
    }
    while(n > 0 && i >= 0)
    {
        i = i - 1;
        Quad p = quadsArray->get(i);
        string regname;
        if(p.opcode == PARAM){
            SymbolTableRow* srow = p.result.symrow;
            if(srow->type->name == "char"){
                regname = getRegisterApt("char",n-1);
                s += "\tmovb\t"+getAddressExpression(srow)+", %"+regname+"\n";
            }
            else if(srow->type->name == "int"){
                regname = getRegisterApt("int",n-1);
                s += "\tmovl\t"+getAddressExpression(srow)+", %"+regname+"\n";
            }
            else if(srow->type->name == "array"){
                regname = getRegisterApt("",n-1);
                s += "\tleaq\t"+getAddressExpression(srow)+", %"+regname+"\n"; 
            }
            else
            {
                regname = getRegisterApt("",n-1);
                s += "\tmovq\t"+getAddressExpression(srow)+", %"+regname+"\n"; 
            }
            n--;
        } 
    }
    *size = si;
    return s;
}

string getASMforQuad(Quad& q,int *factive,int i){
    string s = "";
    int si;
    string instrbase = "";
    string suffix = "";
    static int lcount = _labeltable->getcount();
    SymbolTableRow* result,*operand1,*operand2;
    Type* t;
    switch(q.opcode)
    {
        case PLUS : operand1 = q.operand1.symrow;
                    operand2 = q.operand2.symrow;
                    result = q.result.symrow;
                    t = operand1->type;
                    if(t->name == "int"){
                        s += "\tmovl\t"+getAddressExpression(operand1)+", %eax\t # eax = "+operand1->name+"\n";
                        if(q.operand2.isConstant())
                            s+= "\taddl\t$"+to_string(q.operand2.getIntegerConstValue())+", %eax\t # eax = eax + "
                                +to_string(q.operand2.getIntegerConstValue())+"\n";
                        else
                        {
                            operand2 = q.operand2.symrow;
                            s += "\taddl\t"+getAddressExpression(operand2)+", %eax\t # eax = eax + "+operand2->name+"\n";
                        }
                        s += "\tmovl\t%eax, "+getAddressExpression(result)+"\t # "+result->name+" = eax\n";
                    }
                    else if(t->name == "char"){
                        s += "\tmovzbl\t"+getAddressExpression(operand1)+", %eax\n";
                        if(q.operand2.isConstant())
                        {
                            int v = q.operand2.getIntegerConstValue();
                            s += "\taddl\t$"+to_string(v)+", %eax";
                        }
                        else
                        {
                             operand2 = q.operand2.symrow;
                            s += "\tmovzbl\t"+getAddressExpression(operand2)+", %edx\n";
                            s += "\taddl\t%edx, %eax";
                        }
                        
                        s += "\tmovb\t%al, "+getAddressExpression(result)+"\n";
                    }
                    else if(t->name == "pointer"){
                        s += "\tmovq\t"+getAddressExpression(operand1)+", %rax\n";
                        if(q.operand2.isConstant()){
                            s += "addq\t$"+to_string(q.operand2.getIntegerConstValue())+
                                ", %rax\n";
                        }
                        else
                        {
                            s += "\tmovl\t"+getAddressExpression(operand2)+",%edx\n";
                            //s += "\tcltd\n";
                            s += "\taddq\t %rdx, %rax\n";
                        }
                        s += "\tmovq\t%rax, "+getAddressExpression(result)+"\n";
                    }
                    else if(t->name == "array"){
                        s += "\tleaq\t"+getAddressExpression(operand1)+", %rax\n";
                        if(q.operand2.isConstant()){
                            s += "\taddq\t$"+to_string(q.operand2.getIntegerConstValue())+
                                ", %rax\n";
                        }
                        else
                        {
                            s += "\tmovl\t"+getAddressExpression(operand2)+",%edx\n";
                            //s += "\tcltd\n";
                            s += "\taddq\t %rdx, %rax\n";
                            //cout<<"Here:"<<i<<"\n"<<operand2->name<<endl;
                        }
                        s += "\tmovq\t%rax, "+getAddressExpression(result)+"\n";
                    }
                    break;
                    
        case MINUS : operand1 = q.operand1.symrow;
                    result = q.result.symrow;
                    t = operand1->type;
                    if(t->name == "int"){
                        s += "\tmovl\t"+getAddressExpression(operand1)+", %eax\t # eax = "+operand1->name+"\n";
                        if(q.operand2.isConstant())
                        {
                            //cout<<"Here:"<<result->name<<endl;
                            //cout<<"Here:"<<q.operand2.symrow->name<<endl;
                            s+= "\tsubl\t$"+to_string(q.operand2.getIntegerConstValue())+", %eax\t # eax = eax + "
                                +to_string(q.operand2.getIntegerConstValue())+"\n";
                        }
                        else
                        {
                            operand2 = q.operand2.symrow;
                            s += "\tsubl\t"+getAddressExpression(operand2)+", %eax\t # eax = eax + "+operand2->name+"\n";
                        }
                        s += "\tmovl\t%eax, "+getAddressExpression(result)+"\t # "+operand1->name+" = eax\n";
                    }
                    else if(t->name == "char"){
                        s += "\tmovzbl\t"+getAddressExpression(operand1)+", %eax\n";
                        if(q.operand2.isConstant())
                        {
                            int v = q.operand2.getIntegerConstValue();
                            s += "\tsubl\t$"+to_string(v)+", %eax\n";
                        }
                        else
                        {
                             operand2 = q.operand2.symrow;
                            s += "\tmovzbl\t"+getAddressExpression(operand2)+", %edx\n";
                            s += "\tsubl\t%edx, %eax\n";
                        }
                        
                        s += "\tmovb\t%al, "+getAddressExpression(result)+"\n";
                    }
                    else if(t->name == "pointer"){
                        s += "\tmovq\t"+getAddressExpression(operand1)+", %rax\n";
                        if(q.operand2.isConstant()){
                            s += "subq\t$"+to_string(q.operand2.getIntegerConstValue())+
                                ", %rax\n";
                        }
                        else
                        {
                            s += "\tsubq\t"+getAddressExpression(operand2)+", %rax\n";
                        }
                        s += "\tmovq\t%rax, "+getAddressExpression(result)+"\n";
                    }
                    break;
        
        case MULT : operand1 = q.operand1.symrow;
                    result = q.result.symrow;
                    t = operand1->type;
                    if(t->name == "int"){
                        s += "\tmovl\t"+getAddressExpression(operand1)+", %eax\t # eax = "+operand1->name+"\n";
                        if(q.operand2.isConstant())
                            s+= "\timull\t$"+to_string(q.operand2.getIntegerConstValue())+", %eax\t # eax = eax + "
                                +to_string(q.operand2.getIntegerConstValue())+"\n";
                        else
                        {
                             operand2 = q.operand2.symrow;
                            s += "\timull\t"+getAddressExpression(operand2)+", %eax\t # eax = eax + "+operand2->name+"\n";
                        }
                        s += "\tmovl\t%eax, "+getAddressExpression(result)+"\t # "+operand1->name+" = eax\n";
                    }
                    else if(t->name == "char"){
                        s += "\tmovzbl\t"+getAddressExpression(operand1)+", %eax\n";
                        if(q.operand2.isConstant())
                        {
                            int v = q.operand2.getIntegerConstValue();
                            s += "\timull\t$"+to_string(v)+", %eax\n";
                        }
                        else
                        {
                             operand2 = q.operand2.symrow;
                            s += "\tmovzbl\t"+getAddressExpression(operand2)+", %edx\n";
                            s += "\timull\t%edx, %eax";
                        }
                        
                        s += "\tmovb\t%al, "+getAddressExpression(result)+"\n";
                    }
                    break;
        
        case DIV : operand1 = q.operand1.symrow;
                    result = q.result.symrow;
                    t = operand1->type;
                    if(t->name == "int"){
                        s += "\tmovl\t"+getAddressExpression(operand1)+", %eax\t # eax = "+operand1->name+"\n";
                        s += "\tcltd\n";
                        if(q.operand2.isConstant())
                            s+= "\tidivl\t$"+to_string(q.operand2.getIntegerConstValue())+"\t # eax = eax / "
                                +to_string(q.operand2.getIntegerConstValue())+"\n";
                        else
                        {
                             operand2 = q.operand2.symrow;
                            s += "\tidivl\t"+getAddressExpression(operand2)+"\t # eax = eax / "+operand2->name+"\n";
                        }
                        s += "\tmovl\t%eax, "+getAddressExpression(result)+"\t # "+result->name+" = eax\n";
                    }
                    else if(t->name == "char"){
                        s += "\tmovsbl\t"+getAddressExpression(operand1)+", %eax\n";
                        if(q.operand2.isConstant())
                        {
                            int v = q.operand2.getIntegerConstValue();
                             s += "\tcltd\n";
                            s += "\tidivl\t$"+to_string(v)+"\n";
                        }
                        else
                        {
                             operand2 = q.operand2.symrow;
                            s += "\tmovsbl\t"+getAddressExpression(operand2)+", %edx\n";
                            s += "\tcltd\n";
                            s += "\tidivl\t%edx";
                        }
                        
                        s += "\tmovb\t%al, "+getAddressExpression(result)+"\n";
                    }
                    break;
        
        case MOD :  operand1 = q.operand1.symrow;
                    result = q.result.symrow;
                    t = operand1->type;
                    if(t->name == "int"){
                        s += "\tmovl\t"+getAddressExpression(operand1)+", %eax\t # eax = "+operand1->name+"\n";
                        s += "\tcltd\n";
                        if(q.operand2.isConstant())
                            s+= "\tidivl\t$"+to_string(q.operand2.getIntegerConstValue())+"\t # eax = eax / "
                                +to_string(q.operand2.getIntegerConstValue())+"\n";
                        else
                        {
                             operand2 = q.operand2.symrow;
                            s += "\tidivl\t"+getAddressExpression(operand2)+"\t # eax = eax / "+operand2->name+"\n";
                        }
                        s += "\tmovl\t%edx, "+getAddressExpression(result)+"\t # "+operand1->name+" = eax\n";
                    }
                    else if(t->name == "char"){
                        s += "\tmovsbl\t"+getAddressExpression(operand1)+", %eax\n";
                        if(q.operand2.isConstant())
                        {
                            int v = q.operand2.getIntegerConstValue();
                             s += "\tcltd\n";
                            s += "\tidivl\t$"+to_string(v)+"\n";
                        }
                        else
                        {
                             operand2 = q.operand2.symrow;
                            s += "\tmovsbl\t"+getAddressExpression(operand2)+", %ecx\n";
                            s += "\tcltd\n";
                            s += "\tidivl\t%ecx\n";
                        }
                        s += "\tmovl\t%edx, %eax\n";
                        s += "\tmovb\t%al, "+getAddressExpression(result)+"\n";
                    }
                    break;

        case UMINUS : operand1 = q.operand1.symrow;
                      result = q.result.symrow;
                        if(operand1->type->name == "char"){
                             s += "\tmovzbl\t"+getAddressExpression(operand1)+", %eax\n";
                        }
                        else
                            s += "\tmovl\t"+getAddressExpression(operand1)+", %eax\t # eax = "+operand1->name+"\n"; 
                        s += "\tnegl\t %eax\n";
                        if(operand1->type->name == "char"){
                            s += "\tmovb\t%al, "+getAddressExpression(result)+"\n";
                        }
                        else
                            s += "\tmovl\t%eax, "+getAddressExpression(result)+"\t #"+result->name+" = eax\n";
                        break;
        case COPY:  result = q.result.symrow;
                    if(q.operand1.tag == QuadOperand::SYMBOLINK){
                        operand1 = q.operand1.symrow;
                        
                        if(operand1->type->name == "char"){
                            s += "\tmovzbl\t"+getAddressExpression(operand1)+", %eax\n";
                            s += "\tmovb\t%al, "+getAddressExpression(result)+"\n";
                        }
                        else if(operand1->type->name == "int"){
                            s += "\tmovl\t"+getAddressExpression(operand1)+", %eax\t # eax = "+operand1->name+"\n"; 
                            s += "\tmovl\t%eax, "+getAddressExpression(result)+"\t # eax = "+result->name+"\n";
                        }
                        else if(operand1->type->name == "array"){
                            s += "\tleaq\t"+getAddressExpression(operand1)+", %rax\t # eax = "+operand1->name+"\n"; 
                            s += "\tmovq\t%rax, "+getAddressExpression(result)+"\t # eax = "+result->name+"\n";
                        }
                        else if(operand1->type->name == "pointer"){
                            s += "\tmovq\t"+getAddressExpression(operand1)+", %rax\t # eax = "+operand1->name+"\n"; 
                            s += "\tmovq\t%rax, "+getAddressExpression(result)+"\t # eax = "+result->name+"\n";
                        }
                    }
                    else if(q.operand1.tag == QuadOperand::VALUE)
                    {
                        Value vv = q.operand1.v;
                        if(vv.tag == Value::CHAR_TAG){
                            s += "\tmovb\t$"+to_string((int)(vv.charvalue))+", "+getAddressExpression(result)+"\n";
                        }
                        else if(vv.tag == Value::STRING_TAG)
                        {
                            s += "\tmovq\t$"+vv.getString()+", "+getAddressExpression(result)+"\n";
                        }
                        else
                        {
                            s += "\tmovl\t$"+vv.getString()+", "+getAddressExpression(result)+"\n";
                        }
                    }
                    break;
        case OP_GOTO: s += "\tjmp\t." + q.result.v.strlabel+"\n";
                    break;
        case IFGOTO:  operand1 = q.operand1.symrow;
                    if(operand1->type->name == "char"){
                        suffix = "b";
                    }
                    else if(operand1->type->name == "int"){
                        suffix = "l";
                    }
                    else {
                       suffix = "q";
                    }
                    s += "\tcmp"+suffix+"\t$0, "+getAddressExpression(operand1)+"\n";
                    s += "\tjne\t."+q.result.v.strlabel+"\n";
                    break;
        case IFFalseGOTO:  operand1 = q.operand1.symrow;
                    if(operand1->type->name == "char"){
                        suffix = "b";
                    }
                    else if(operand1->type->name == "int"){
                        suffix = "l";
                    }
                    else {
                       suffix = "q";
                    }
                    s += "\tcmp"+suffix+"\t$0, "+getAddressExpression(operand1)+"\n";
                    s += "\tje\t."+q.result.v.strlabel+"\n";
                    break;
        
        case IFLessGOTO:  operand1 = q.operand1.symrow;
                          operand2 = q.operand2.symrow;
                    if(operand1->type->name == "char"){
                        s += "\tmovzbl\t"+getAddressExpression(operand1)+", %eax\n";
                        s += "\tcmpb\t"+getAddressExpression(operand2)+", %al\n";
                    }
                    else if(operand1->type->name == "int"){
                        s += "\tmovl\t"+getAddressExpression(operand1)+", %eax\n";
                        s += "\tcmpl\t"+getAddressExpression(operand2)+", %eax\n";
                    }
                    else {
                        s += "\tmovq\t"+getAddressExpression(operand1)+", %rax\n";
                        s += "\tcmpq\t"+getAddressExpression(operand2)+", %rax\n";
                    }
                    s += "\tjl\t."+q.result.v.strlabel+"\n";
                    break;
        
        case IFGrtGOTO:  operand1 = q.operand1.symrow;
                          operand2 = q.operand2.symrow;
                    if(operand1->type->name == "char"){
                        s += "\tmovzbl\t"+getAddressExpression(operand1)+", %eax\n";
                        s += "\tcmpb\t"+getAddressExpression(operand2)+", %al\n";
                    }
                    else if(operand1->type->name == "int"){
                        s += "\tmovl\t"+getAddressExpression(operand1)+", %eax\n";
                        s += "\tcmpl\t"+getAddressExpression(operand2)+", %eax\n";
                    }
                    else {
                        s += "\tmovq\t"+getAddressExpression(operand1)+", %rax\n";
                        s += "\tcmpq\t"+getAddressExpression(operand2)+", %rax\n";
                    }
                    s += "\tjg\t."+q.result.v.strlabel+"\n";
                    break;

        case IFLessEqGOTO:  operand1 = q.operand1.symrow;
                          operand2 = q.operand2.symrow;
                    if(operand1->type->name == "char"){
                        s += "\tmovzbl\t"+getAddressExpression(operand1)+", %eax\n";
                        s += "\tcmpb\t"+getAddressExpression(operand2)+", %al\n";
                    }
                    else if(operand1->type->name == "int"){
                        s += "\tmovl\t"+getAddressExpression(operand1)+", %eax\n";
                        s += "\tcmpl\t"+getAddressExpression(operand2)+", %eax\n";
                    }
                    else {
                        s += "\tmovq\t"+getAddressExpression(operand1)+", %rax\n";
                        s += "\tcmpq\t"+getAddressExpression(operand2)+", %rax\n";
                    }
                    s += "\tjle\t."+q.result.v.strlabel+"\n";
                    break;

        case IFGrtEqGOTO:  operand1 = q.operand1.symrow;
                          operand2 = q.operand2.symrow;
                    if(operand1->type->name == "char"){
                        s += "\tmovzbl\t"+getAddressExpression(operand1)+", %eax\n";
                        s += "\tcmpb\t"+getAddressExpression(operand2)+", %al\n";
                    }
                    else if(operand1->type->name == "int"){
                        s += "\tmovl\t"+getAddressExpression(operand1)+", %eax\n";
                        s += "\tcmpl\t"+getAddressExpression(operand2)+", %eax\n";
                    }
                    else {
                        s += "\tmovq\t"+getAddressExpression(operand1)+", %rax\n";
                        s += "\tcmpq\t"+getAddressExpression(operand2)+", %rax\n";
                    }
                    s += "\tjge\t."+q.result.v.strlabel+"\n";
                    break;

        case IFLogEqGOTO:  operand1 = q.operand1.symrow;
                          operand2 = q.operand2.symrow;
                    if(operand1->type->name == "char"){
                        s += "\tmovzbl\t"+getAddressExpression(operand1)+", %eax\n";
                        s += "\tcmpb\t"+getAddressExpression(operand2)+", %al\n";
                    }
                    else if(operand1->type->name == "int"){
                        s += "\tmovl\t"+getAddressExpression(operand1)+", %eax\n";
                        s += "\tcmpl\t"+getAddressExpression(operand2)+", %eax\n";
                    }
                    else {
                        s += "\tmovq\t"+getAddressExpression(operand1)+", %rax\n";
                        s += "\tcmpq\t"+getAddressExpression(operand2)+", %rax\n";
                    }
                    s += "\tje\t."+q.result.v.strlabel+"\n";
                    break;

        case IFNotEqGOTO:  operand1 = q.operand1.symrow;
                          operand2 = q.operand2.symrow;
                    if(operand1->type->name == "char"){
                        s += "\tmovzbl\t"+getAddressExpression(operand1)+", %eax\n";
                        s += "\tcmpb\t"+getAddressExpression(operand2)+", %al\n";
                    }
                    else if(operand1->type->name == "int"){
                        s += "\tmovl\t"+getAddressExpression(operand1)+", %eax\n";
                        s += "\tcmpl\t"+getAddressExpression(operand2)+", %eax\n";
                    }
                    else {
                        s += "\tmovq\t"+getAddressExpression(operand1)+", %rax\n";
                        s += "\tcmpq\t"+getAddressExpression(operand2)+", %rax\n";
                    }
                    s += "\tjne\t."+q.result.v.strlabel+"\n";
                    break;
        case CALL : si = 0;
                    s = generateCallStack(q,i,&si);
                    result = q.result.symrow;
                    if(q.operand1.symrow->type->name == "pointer")
                    {
                        s += "\tmovq\t"+getAddressExpression(q.operand1.symrow)+", %rax\n";
                        s += "\tcall\t*%rax\n";

                    }
                    else
                        s += "\tcall\t"+q.operand1.symrow->name+"\n";
                    if(result->type->name == "void"){
                    }
                    else if(result->type->name == "char"){
                        s += "\tmovb\t%al, "+getAddressExpression(result)+"\n";
                    }
                    else if(result->type->name == "int"){
                        s += "\tmovl\t%eax, "+getAddressExpression(result)+"\n";
                    }
                    else{
                        s += "\tmovq\t%rax, "+getAddressExpression(result)+"\n";
                    }
                    if(si > 0)
                        s += "\taddq $"+to_string(si)+", %rsp\n";
                    break;
        case PARAM : s = "";
                    break;
        case OP_RETURN: if(q.result.tag != QuadOperand::NA)
                        {
                            SymbolTableRow *result = q.result.symrow;
                            if(result->type->name == "char"){
                                s += "\tmovzbl\t"+getAddressExpression(result)+", %eax\n";
                            }
                            else if(result->type->name == "int"){
                                s += "\tmovl\t"+getAddressExpression(result)+", %eax\n";
                            }
                            else {
                                s += "\tmovq\t"+getAddressExpression(result)+", %rax\n";
                            }
                        }
                        s += "\tjmp .RET"+to_string(lcount)+"\n";
                    break;
        case ASSIGNARR: 
                    operand1 = q.operand1.symrow;
                    operand2 = q.operand2.symrow;
                    result = q.result.symrow;
                    if(operand1->type->name == "pointer"){
                        if(result->type->name == "char"){
                            s += "\tmovl\t"+getAddressExpression(operand2)+",%edx\n";
                            s += "\tcltq\n";
                            s += "\tmovq\t"+getAddressExpression(operand1)+", %rax\n";
                            s += "\taddq\t%rdx,%rax\n";
                            s += "\tmovzbl\t0(%rax), %eax\n";
                            s += "\tmovb\t%al, "+getAddressExpression(result)+"\n";
                        }
                        else if(result->type->name == "int"){
                            s += "\tmovl\t"+getAddressExpression(operand2)+",%edx\n";
                            s += "\tcltq\n";
                            s += "\tmovq\t"+getAddressExpression(operand1)+", %rax\n";
                            s += "\taddq\t%rdx,%rax\n";
                            s += "\tmovl\t0(%rax), %eax\n";
                            s += "\tmovl\t%eax, "+getAddressExpression(result)+"\n";
                        }
                        else {
                            s += "\tmovl\t"+getAddressExpression(operand2)+",%edx\n";
                            s += "\tcltq\n";
                            s += "\tmovq\t"+getAddressExpression(operand1)+", %rax\n";
                            s += "\taddq\t%rdx,%rax\n";
                            s += "\tmovq\t0(%rax), %eax\n";
                            s += "\tmovq\t%rax, "+getAddressExpression(result)+"\n";
                        }
                    }
                    else{
                        if(result->type->name == "char"){
                            s += "\tmovl\t"+getAddressExpression(operand2)+",%eax\n";
                            s += "\tcltq\n";
                            if(operand1->categoryOfSymbol == symbolCategory::GLOBAL){
                                s += "\tmovzbl\t"+operand1->name+"(%rax), %eax\n";
                            }
                            else
                            {
                                s += "\tmovzbl\t"+to_string(operand1->offset)+"(%rbp, %rax), %eax\n";
                            }
                            s += "\tmovb\t%al, "+getAddressExpression(result)+"\n";
                        }
                        else if(result->type->name == "int"){
                            s += "\tmovl\t"+getAddressExpression(operand2)+",%eax\n";
                            s += "\tcltq\n";
                            if(operand1->categoryOfSymbol == symbolCategory::GLOBAL){
                                s += "\tmovl\t"+operand1->name+"(%rax), %eax\n";
                            }
                            else
                            {
                                s += "\tmovl\t"+to_string(operand1->offset)+"(%rbp, %rax), %eax\n";
                            }
                            s += "\tmovl\t%eax, "+getAddressExpression(result)+"\n";
                        }
                        else {
                            s += "\tmovl\t"+getAddressExpression(operand2)+",%eax\n";
                            s += "\tcltq\n";
                            if(operand1->categoryOfSymbol == symbolCategory::GLOBAL){
                                s += "\tmovq\t"+operand1->name+"(%rax), %rax\n";
                            }
                            else
                            {
                                s += "\tmovq\t"+to_string(operand1->offset)+"(%rbp, %rax), %rax\n";
                            }
                            s += "\tmovq\t%rax, "+getAddressExpression(result)+"\n";
                        }
                    }
                    break;
        case ARRASSIGN: 
                    operand1 = q.operand1.symrow;
                    operand2 = q.operand2.symrow;
                    result = q.result.symrow;
                    if(result->type->name == "pointer"){
                        if(operand2->type->name == "char"){
                            s += "\tmovl\t"+getAddressExpression(operand1)+",%edx\n";
                            s += "\tcltq\n";
                            s += "\tmovq\t"+getAddressExpression(result)+", %rax\n";
                            s += "\taddq\t%rdx,%rax\n";
                            s += "\tmovzbl\t"+getAddressExpression(operand2)+", %edx\n";
                            s += "\tmovb\t %dl, 0(%rax)\n";
                            
                        }
                        else if(operand2->type->name == "int"){
                            s += "\tmovl\t"+getAddressExpression(operand1)+",%edx\n";
                            s += "\tcltq\n";
                            s += "\tmovq\t"+getAddressExpression(result)+", %rax\n";
                            s += "\taddq\t%rdx,%rax\n";
                            s += "\tmovl\t"+getAddressExpression(operand2)+", %edx\n";
                            s += "\tmovl\t %edx, 0(%rax)\n";
                        }
                        else {
                            s += "\tmovl\t"+getAddressExpression(operand1)+",%edx\n";
                            s += "\tcltq\n";
                            s += "\tmovq\t"+getAddressExpression(result)+", %rax\n";
                            s += "\taddq\t%rdx,%rax\n";
                            s += "\tmovq\t"+getAddressExpression(operand2)+", %rdx\n";
                            s += "\tmovq\t %rdx, 0(%rax)\n";
                        }
                    }
                    else
                    {
                        if(operand2->type->name == "char"){
                            s += "\tmovl\t"+getAddressExpression(operand1)+",%eax\n";
                            s += "\tcltq\n";
                            s += "\tmovzbl\t"+getAddressExpression(operand2)+", %edx\n";
                            if(operand1->categoryOfSymbol == symbolCategory::GLOBAL){
                                s += "\tmovb\t%dl, "+result->name+"(%rax)\n";
                            }
                            else
                            {
                                s += "\tmovb\t%dl, "+to_string(result->offset)+"(%rbp, %rax)\n";
                            }
                            
                        }
                        else if(operand2->type->name == "int"){
                            s += "\tmovl\t"+getAddressExpression(operand1)+",%eax\n";
                            s += "\tcltq\n";
                            s += "\tmovl\t"+getAddressExpression(operand2)+", %edx\n";
                            if(operand1->categoryOfSymbol == symbolCategory::GLOBAL){
                                s += "\tmovl\t%edx, "+result->name+"(%rax)\n";
                            }
                            else
                            {
                                s += "\tmovl\t%edx, "+to_string(result->offset)+"(%rbp, %rax)\n";
                            }
                        }
                        else {
                            s += "\tmovl\t"+getAddressExpression(operand1)+",%eax\n";
                            s += "\tcltq\n";
                            s += "\tmovq\t"+getAddressExpression(operand2)+", %rdx\n";
                            if(operand1->categoryOfSymbol == symbolCategory::GLOBAL){
                                s += "\tmovq\t%rdx, "+result->name+"(%rax)\n";
                            }
                            else
                            {
                                s += "\tmovq\t%rdx, "+to_string(result->offset)+"(%rbp, %rax)\n";
                            }
                        }
                    }
                    break;
                    
        case ASSIGNADDRESS:
                    operand1 = q.operand1.symrow;
                    result = q.result.symrow;
                    if(operand1->categoryOfSymbol == symbolCategory:: GLOBAL){
                        s += "\tmovq\t$"+operand1->name+", %rax\n";
                    }
                    else
                        s += "\tleaq\t"+getAddressExpression(operand1)+", %rax\n";
                    s += "\tmovq\t%rax, "+getAddressExpression(result)+"\n";
                    break;
        case ASSIGNVALUE:
                    operand1 = q.operand1.symrow;
                    result = q.result.symrow;
                    s += "\tmovq\t"+getAddressExpression(operand1)+", %rax\n";
                    if(result->type->name == "char"){
                        s += "\tmovzbl\t(%rax), %eax\n";
                        s += "\tmovb\t%al, "+getAddressExpression(result)+"\n";
                    }
                    else if(result->type->name == "int"){
                        s += "\tmovl\t(%rax), %eax\n";
                        s += "\tmovl\t%eax, "+getAddressExpression(result)+"\n";
                    }
                    else {
                        s += "\tmovq\t(%rax), %rax\n";
                        s += "\tmovq\t%rax, "+getAddressExpression(result)+"\n";
                    }
                    s += "#"+to_string(i)+"\n";
                    break;
        case VALUEASSIGN: 
                    operand1 = q.operand1.symrow;
                    result = q.result.symrow;
                    if(operand1->type->name == "char"){
                        s += "\tmovzbl\t"+getAddressExpression(operand1)+", %edx\n";
                        s += "\tmovq\t"+getAddressExpression(result)+", %rax\n";
                        s += "\tmovb\t%dl, (%rax)\n";
                    }
                    else if(operand1->type->name == "int"){
                        s += "\tmovl\t"+getAddressExpression(operand1)+", %edx\n";
                        s += "\tmovq\t"+getAddressExpression(result)+", %rax\n";
                        s += "\tmovl\t%edx, (%rax)\n";
                    }
                    else {
                        s += "\tmovq\t"+getAddressExpression(operand1)+", %rdx\n";
                        s += "\tmovq\t"+getAddressExpression(result)+", %rax\n";
                        s += "\tmovq\t%rdx, (%rax)\n";
                    }
                    break;
        case __SLABEL: 
                    s += makeFunctionPrologue(q.result.symrow);
                    *factive = 1;
                    lcount++;
                    break;
        case __ELABEL: s += ".RET"+to_string(lcount)+":\n";
                       s += makeFunctionEpilogue(q.result.symrow);
                       *factive = 0;
                        break;
        default: cout<<"Some error occurred\n";
                 exit(1);
    }
    return s;
}

void printASMCode(ostream& out,string filename){
    out<<"\t.file\t\""<<filename<<".c\""<<endl;
    string s = globalST->getDataSegment();
    s = s + strTable->getStringsASM();
    if(!s.empty()){
        out<<"\t.data\n";
        out<<s;

    }
    int len = quadsArray->getNextIndex();
    int isFunctionActive = 0;
    for(int i=0;i<len;i++){
        if(_labeltable->find(i)){
            out<<"."<<_labeltable->lookup(i)<<":"<<endl;
        }
        int old = isFunctionActive;
        s = getASMforQuad(quadsArray->get(i),&isFunctionActive,i);

        if(isFunctionActive || old) {
            out<<"#";
            quadsArray->get(i).print(out);
            out<<"\n";
            out<<s;
        }
    }
    s = "\t.ident\t\"GCC: (Ubuntu 7.4.0-1ubuntu1~18.04.1) 7.4.0\"\n";
    s += "\t.section\t.note.GNU-stack,\"\",@progbits\n";
    out<<s;
}

/** Label Table End */

int main(int argc, char *argv[])
{
    if(argc < 2){
        cout<<"Usage: tinyc <filename> -d<y/n>(for debug) -s<number>(to specify names of output files)"<<endl;
        exit(1);
    }
    int flag = 0, formatflag = 0, testn = 0;
    string filename(argv[1]);
    if(argc > 2){
        char c = argv[2][1];
        if(c != 'd')
        {
            cout<<"Usage: tinyc <filename> -d<y/n>(for debug) -s<number>(to specify names of output files)"<<endl;
            exit(1);
        }
        else
        {
            if(argv[2][2] == 'y')
                flag = 1;
        }
    }
    if(argc > 3){
        char c = argv[3][1];
        if(c != 's')
        {
            cout<<"Usage: tinyc <filename> -d<y/n>(for debug) -s<number>(to specify names of output files)"<<endl;
            exit(1);
        }
        else{
            formatflag = 1;
            testn = argv[3][2] - '0';
            if(argv[3][2] != '\0'){
                testn = testn*10 + argv[3][3] - '0';
            }
        }
    }
    filename = filename.substr(0,filename.find_last_of('.'));

    /*Generating temp file with headers added:printLib is default included*/
    ofstream workingfile(filename+"temp.c");
    ifstream inputfile(filename+".c");
    string hinclude = getIncludeString();
    workingfile<<hinclude;
    workingfile<<inputfile.rdbuf();
    inputfile.close();
    workingfile.close();
    string s = (filename+"temp.c");
    FILE* fp = fopen(s.c_str(),"r");
    yyin = fp;

    /*Setting output file streams*/
    ofstream asmout(filename+".s");
    ofstream tacout(filename+"_tac.out");
    ofstream symout(filename+".sym");
        ofstream asmouts("ass6_13CS10060"+to_string(testn)+".asm");
        ofstream  quadout("ass6_13CS10060_quads"+to_string(testn)+".out");
    

    // Debug paramater for bison
    yydebug = 0;
    yyparse();
    fclose(fp);

    _labeltable = new LabelTable();
    quadsArray->updateLabelTable();
    //quadsArray->propagateSome();
    /*Printing quads in respective file*/
    if(formatflag){
        quadsArray->print(quadout);
        quadout<<"\n\n\n\nString Table:::\n"<<endl;
        strTable->print(quadout);
        quadout<<"\n\n\n\nLabel Table::::\n"<<endl;
        _labeltable->print(quadout);
        quadout<<"\n\n\n\nSymbol Table(Without AR Offsets):::\n"<<endl;
        globalST->print(quadout);

    }
    quadsArray->print(tacout);
    tacout.close();
    /*Printing symbol tables */
    symout<<"\n\n\n\nString Table:::\n"<<endl;
    strTable->print(symout);
    symout<<"\n\n\n\nLabel Table::::\n"<<endl;
    _labeltable->print(symout);
    symout<<"\n\n\n\nSymbol Table(Without AR Offsets):::\n"<<endl;
    globalST->print(symout);
    globalST->setARoffsets();
    symout<<"\n\n\n\nSymbol Table(With AR Offsets):::\n"<<endl;
    globalST->print(symout);
    symout.close();
    if(formatflag){
        quadout<<"\n\n\n\nSymbol Table(With AR Offsets):::\n"<<endl;
        globalST->print(quadout); 
        printASMCode(asmouts,filename);
        asmouts.close();
        quadout.close();
    }
    /**/
    printASMCode(asmout,filename);
    asmout.close();
    /**System commands to generate a.out*/

    string command = "gcc -c "+filename+".s";
    system(command.c_str());
    command = "gcc -no-pie "+filename+".o -L. -lprintl";
    system(command.c_str());

    /*Remove files if not in debug mode*/
    if(!flag)
    {
        string filen = filename+".s";
        remove(filen.c_str());
        filen = filename+"_tac.out";
        remove(filen.c_str());
        filen = filename+".sym";
        remove(filen.c_str());
        filen = filename+"temp.c";
        remove(filen.c_str());
        filen = filename+".o";
        remove(filen.c_str());
    }
    if(!formatflag){
        string filen = "ass6_13CS10060"+to_string(testn)+".asm";
        remove(filen.c_str());
        filen = "ass6_13CS10060_quads"+to_string(testn)+".out";
        remove(filen.c_str());
    }
    return 0;



}