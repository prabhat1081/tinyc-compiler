%error-verbose
%{
	#include <string>
	#include <cstdlib>
	#include "ass6_13CS10060_translator.h"
	using namespace std;
	extern "C" int yylex(void);
	extern "C" char* yytext;
	void yyerror(const char* s);
    #define YYDEBUG 1
	SymbolTable *globalST,*currentST;
	QuadArray *quadsArray;
    StringTable *strTable;
	Type* currentType;
%}

%union {
	int intVal;
	double doubleVal;
	char charVal;
	char* stringVal;
	char* identifier;
	SymbolTableRow* symrow;
	Expression* expr;
	ListType* ltype;
	ArgList* alist;
	int instruction;
	Type* _type;
	int cpointer;
	ParameterList* plist;


};

%token <intVal> INTEGER_CONST
%token <doubleVal> FLOATING_CONST
%token <charVal> CHARACTER_CONST
%token <stringVal> STRING_LIT

%token AUTO
%token BREAK
%token CASE
%token CHAR
%token CONST
%token CONTINUE
%token DEFAULT
%token DO
%token DOUBLE
%token ELSE
%token ENUM
%token EXTERN
%token FLOAT
%token FOR
%token GOTO
%token IF
%token INLINE
%token INT
%token LONG
%token REGISTER
%token RESTRICT
%token RETURN
%token SHORT
%token SIGNED
%token SIZEOF
%token STATIC
%token STRUCT
%token SWITCH
%token TYPEDEF
%token UNION
%token UNSIGNED
%token VOID
%token VOLATILE
%token WHILE
%token _BOOL
%token _COMPLEX
%token _IMAGINARY
%token ENUMERATION_CONST

%token <identifier> IDENTITY

%token ARROW_OP
%token INCREMENT_OP
%token DECREMENT_OP
%token RIGHT_SHIFT_OP
%token LEFT_SHIFT_OP
%token GREATER_EQUAL_OP
%token LESS_EQUAL_OP
%token EQUALITY_OP
%token NOTEQUAL_OP
%token LOGICAL_OR_OP
%token LOGICAL_AND_OP
%token ELLIPSIS
%token MULT_ASSIGNMENT_OP
%token DIV_ASSIGNMENT_OP
%token MOD_ASSIGNMENT_OP
%token ADD_ASSIGNMENT_OP
%token SUB_ASSIGNMENT_OP
%token LSH_ASSIGNMENT_OP
%token RSH_ASSIGNMENT_OP
%token BIAND_ASSIGNMENT_OP
%token BIXOR_ASSIGNMENT_OP
%token BIOR_ASSIGNMENT_OP
%token NEWLINE


%type <expr> expression
%type <expr> primary_expression
%type <expr> postfix_expression
%type <expr> assignment_expression
%type <expr> unary_expression
%type <expr> cast_expression
%type <expr> multiplicative_expression
%type <expr> additive_expression
%type <expr> shift_expression
%type <expr> relational_expression
%type <expr> equality_expression
%type <expr> AND_expression
%type <expr> exclusive_OR_expression
%type <expr> inclusive_OR_expression
%type <expr> logical_AND_expression
%type <expr> logical_OR_expression
%type <expr> conditional_expression
%type <expr> constant_expression
%type <alist> argument_expression_list
%type <instruction>M
%type <ltype>N
%type <_type>type_specifier
%type <_type>specifier_qualifier_list
%type <_type> type_name
%type <_type>declaration_specifiers
%type <ltype> statement
%type <ltype> labeled_statement
%type <ltype> compound_statement
%type <ltype> expression_statement
%type <ltype> selection_statement
%type <ltype> iteration_statement
%type <ltype> jump_statement
%type <ltype> block_item_list
%type <ltype> block_item
%type <expr> expression_opt
%type <symrow> declaration
%type <symrow> init_declarator
%type <_type> init_declarator_list
%type <symrow> initializer
%type <symrow> declarator
%type <symrow> direct_declarator
%type <plist> parameter_type_list
%type <plist> parameter_list
%type <symrow> parameter_declaration
%type <cpointer> pointer

%start translation_unit

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%

primary_expression:		IDENTITY
						{
							SymbolTableRow* symrow = currentST->lookup($1);
							if(!symrow){
								yyerror("Variable used before declaration");
							}
							$$ = new Expression();
							$$->loc = symrow;
							$$->type = symrow->type;

						}
						| INTEGER_CONST
						{
                            //cout<<"Inte"<<$1<<endl;
                            $$ = new Expression();
                            $$->loc = currentST->gentemp(INT_TYPE,$1,CONSTANT_TEMP);
                            $$->type = INT_TYPE;
                            quadsArray->emit(COPY,($$->loc),Value($1));
						}
						| FLOATING_CONST
						{
                            $$ = new Expression();
                            $$->loc = currentST->gentemp(DOUBLE_TYPE,$1,CONSTANT_TEMP);
                            $$->type = DOUBLE_TYPE;
                            quadsArray->emit(COPY,($$->loc),Value($1));
						}
						| CHARACTER_CONST
						{
                            $$ = new Expression();
                            //cout<<$1<<endl;
                            $$->loc = currentST->gentemp(CHAR_TYPE,$1,CONSTANT_TEMP);
                            $$->type = CHAR_TYPE;
                            quadsArray->emit(COPY,($$->loc),Value($1));
						}
						| ENUMERATION_CONST
						{
                            /* Empty action as enumeration constant not handled */
						}
						| STRING_LIT
						{
                            $$ = new Expression();
                            string lstr = strTable->lookup($1);
                            $$->loc = currentST->gentemp(STRING_TYPE,lstr,CONSTANT_TEMP);
                            $$->type = STRING_TYPE;
                            quadsArray->emit(COPY,$$->loc,Value(lstr));
						}
						|  '(' expression ')'
						{
                            $$ = $2;
						}
						;

postfix_expression:		primary_expression
                        {
                            $$ = $1;
                        }
						| postfix_expression '[' expression ']'
						{
                            $3->convertBoolToInt();
                            $$ = new Expression();

                            $$->array = $1->array;
                            $$->type = ((Array*)($1->type))->elemType;
                            $$->loc = currentST->gentemp(INT_TYPE);
                            if($1->loc->type->name != "int"){
                                quadsArray->emit(MULT,$$->loc,$3->loc,$$->type->getSize());
                                $$->array = $1->loc;
                            }
                            else
                            {
                                quadsArray->emit(MULT,$$->loc,$3->loc,$$->type->getSize());
                                quadsArray->emit(PLUS,($$->loc),($1->loc),$$->loc);
                            }


						}
						| postfix_expression '(' ')'
						{
                            $$ = new Expression();
                            //cout<<"I was here\n";
                            SymbolTableRow* id = currentST->lookup(($1->loc)->name);
                            if(!isCallable($1->type)){
                                    yyerror("called object is not a function");
                            }
                            else{
                                if($1->array != NULL){
                                    SymbolTableRow *srow = currentST->gentemp($1->type);
                                    /*if($1->array->type->name == "pointer" && 0)
                                    {
                                        SymbolTableRow* tmp = currentST->gentemp($1->array->type);
                                        quadsArray->emit(PLUS,tmp,$1->array,$1->loc);
                                        quadsArray->emit(ASSIGNVALUE,srow,tmp);

                                    }
                                    else
                                    {*/
                                        quadsArray->emit(ASSIGNARR,srow,($1->array),($1->loc));
                                    //}
                                    $$ = callFunction(srow,NULL);
                                }
                                else{
                                    //cout<<"I was here\n";
                                    $$ = callFunction(id,NULL);
                                }
                            }

						}
						| postfix_expression '(' argument_expression_list ')'
						{
                            $$ = new Expression();
                            SymbolTableRow* id = currentST->lookup($1->loc->name);
                            if(!isCallable($1->type)){
                                $1->type->print(cout);
                                yyerror("called object is not a function");
                            }
                            else{
                                if($1->array != NULL){
                                    SymbolTableRow *srow = currentST->gentemp($1->type);
                                    /*if($1->array->type->name == "pointer" && 0)
                                    {
                                        SymbolTableRow* tmp = currentST->gentemp($1->array->type);
                                        quadsArray->emit(PLUS,tmp,$1->array,$1->loc);
                                        quadsArray->emit(ASSIGNVALUE,srow,tmp);

                                    }
                                    else
                                    {*/
                                        quadsArray->emit(ASSIGNARR,srow,($1->array),($1->loc));
                                    //}
                                    $$ = callFunction(srow,$3);
                                }
                                else{
                                    //cout<<"I was here\n";
                                    $$ = callFunction(id,$3);
                                }
                            }
						}
						| postfix_expression '.' IDENTITY
						{
                            /* Empty action as structures  not handled */
						}
						| postfix_expression ARROW_OP IDENTITY
						{
                            /* Empty action as structures  not handled */
						}
						| postfix_expression INCREMENT_OP
						{
                            /**
                            *
                            * As from the specification we have the following constraint over the operand:
                            * Constraint : "The operand of the postfix increment or decrement operator shall
                            * have qualified or unqualified real or pointer type and shall be a modifiable lvalue"
                            * Hence it has to bec checked that the type of $1 is one of int,double,char,pointer and
                            * is indeed a non-const variable
                            * Since the correctness of program is assumed the check is not done
                            * Semantics : "The result of the postfix ++ operator is the value of the operand.
                            * After the result is obtained, the value of the operand is incremented.
                            * (That is, the value 1 of the appropriate type is added to it.)"
                            *
                            */
                            $$ = new Expression();
                            $$->loc = currentST->gentemp($1->type);
                            $$->type = $1->type;
                            int one = $1->type->getIdentity();
                            if($1->array != 0)
                            {
                                quadsArray->emit(ASSIGNARR,$$->loc,$1->array,$1->loc);
                                SymbolTableRow *symrow = currentST->gentemp($1->type);
                                quadsArray->emit(PLUS,symrow,$$->loc,one);
                                quadsArray->emit(ARRASSIGN,$1->array,$1->loc,symrow);
                            }
                            else{
                                quadsArray->emit(COPY,($$->loc),($1->loc));
                                quadsArray->emit(PLUS,($1->loc),($1->loc),one);
                            }


						}
						| postfix_expression DECREMENT_OP
						{
                            /**
                            * Same as post increment operator
                            */
                            $$ = new Expression();
                            $$->loc = currentST->gentemp($1->type);
                            $$->type = $1->type;
                            int one = $1->type->getIdentity();
                            if($1->array != 0)
                            {
                                quadsArray->emit(ASSIGNARR,$$->loc,$1->array,$1->loc);
                                SymbolTableRow *symrow = currentST->gentemp($1->type);
                                quadsArray->emit(MINUS,symrow,$$->loc,one);
                                quadsArray->emit(ARRASSIGN,$1->array,$1->loc,symrow);
                            }
                            else{
                                quadsArray->emit(COPY,($$->loc),($1->loc));
                                quadsArray->emit(MINUS,($1->loc),($1->loc),one);
                            }

						}
						| '(' type_name ')' '{' initializer_list '}'
						{
                            /* Empty action as not handled */
						}
						| '(' type_name ')' '{' initializer_list ',' '}'
						{
                            /* Empty action as not handled */
						}
						;

argument_expression_list:	assignment_expression
                            {
                                /**
                                    Type check and compatibility to be handled in the function call time
                                */
                                $$ = new ArgList();
                                $$->args.push_back($1);

                            }
							| argument_expression_list ',' assignment_expression
							{
                                $1->args.push_back($3);
                                $$ = $1;
							}
							;

unary_expression:	postfix_expression
                    {
                        $$ = $1;
                    }
					| INCREMENT_OP unary_expression
					{
                        /**
                        *
                        * As from the specification we have the following constraint over the operand:
                        * Constraint : "The operand of the prefix increment or decrement operator shall
                        * have qualified or unqualified real or pointer type and shall be a modifiable lvalue"
                        *
                        * Hence it has to be checked that the type of $1 is one of int,double,char,pointer and
                        * is indeed a non-const variable
                        * Since the correctness of program is assumed the check is not done
                        *
                        * Semantics : "The result of the postfix ++ operator is the value of the operand.
                        * After the result is obtained, the value of the operand is incremented.
                        * (That is, the value 1 of the appropriate type is added to it.)"
                        *
                        */
                        $$ = new Expression();
                        $$->loc = currentST->gentemp($2->type);
                        $$->type = $2->type;
                        int one = $2->type->getIdentity();
                        if($2->array != 0)
                        {
                            quadsArray->emit(ASSIGNARR,$$->loc,$2->array,$2->loc);
                            quadsArray->emit(PLUS,$$->loc,$$->loc,one);
                            quadsArray->emit(ARRASSIGN,$2->array,$2->loc,$$->loc);
                        }
                        else{
                            quadsArray->emit(PLUS,($2->loc),($2->loc),one);
                            quadsArray->emit(COPY,($$->loc),($2->loc));
                        }

					}
					| DECREMENT_OP unary_expression
					{
                        /**
                        * Same as pre increment operator
                        */
                        $$ = new Expression();
                        $$->loc = currentST->gentemp($2->type);
                        $$->type = $2->type;
                        int one = $2->type->getIdentity();
                        if($2->array != 0)
                        {
                            quadsArray->emit(ASSIGNARR,$$->loc,$2->array,$2->loc);
                            quadsArray->emit(MINUS,$$->loc,$$->loc,one);
                            quadsArray->emit(ARRASSIGN,$2->array,$2->loc,$$->loc);
                        }
                        else{
                            quadsArray->emit(MINUS,($2->loc),($2->loc),one);
                            quadsArray->emit(COPY,($$->loc),($2->loc));
                        }

					}
					| '&' cast_expression
					{
                        $$ = new Expression();
                        Type *t = $2->type;
                        t = new Pointer(t);
                        $$->loc = currentST->gentemp(t);
                        $$->type = t;
                        if($2->array == NULL){
                            quadsArray->emit(ASSIGNADDRESS,$$->loc,$2->loc);
                        }
                        else{
                            quadsArray->emit(PLUS,$$->loc,$2->array,$2->loc);
                            //cout<<"ha"<<$2->loc->name<<endl;
                        }

					}
					| '*' cast_expression
					{
                        $$ = new Expression();
                        Type *t = $2->type;
                        Pointer *p = (Pointer*)t;
                        t = p->pointeeType;
                        $$->type = t;
                        t = new Array(1,t);
                        $$->loc = currentST->gentemp(INT_TYPE);
                        
                        $$->array = $2->loc;
                        quadsArray->emit(COPY,$$->loc,0);
					}
					| '+' cast_expression
					{
                        /**
                        * Arithmetic Type : Integer and floating
                        * Constraint : The operand of the unary + or - operator shall have arithmetic type
                        * Semantics : The result of the unary + operator is the value of its (promoted) operand.
                        * The integer promotions are performed on the operand, and the result has the promoted type.
                        */
                        /**
                        * Basically convert bool to int, promote char to int, else do nothing
                        */
                        $2->convertBoolToInt();
                        typecheck($2,UPLUS);
                        $$ = $2;

					}
					| '-' cast_expression
					{
                        /**
                        * Constraint : The operand of the unary + or - operator shall have arithmetic type
                        * Semantics : The result of the unary - operator is the negative of its (promoted) operand.
                        * The integer promotions are performed on the operand, and the result has the promoted type
                        */
                        /**
                        * Basically convert bool to int, promote char to int, else do nothing
                        */
                        $$ = new Expression();
                        if($2->array != NULL){
                            Expression* tmp = new Expression();
                            tmp->loc = currentST->gentemp($2->type);
                            tmp->type = $2->type;
                            quadsArray->emit(ASSIGNARR,tmp->loc,($2->array),($2->loc));
                            $2 = tmp;

                        }
                        $2->convertBoolToInt();
                        typecheck($2,UMINUS);
                        $$->loc = currentST->gentemp($2->type);
                        $$->type = $2->type;
                        quadsArray->emit(UMINUS,($$->loc),($2->loc));

					}
					| '~' cast_expression
					{
                        /**
                        * Constraint : The operand of the ~ operator shall have integer type
                        * Semantics : The result of the ~ operator is the bitwise complement of its (promoted)
                        * operand (that is, each bit in the result is set if and only if the corresponding bit
                        * in the converted operand is not set). The integer promotions are performed on the
                        * operand, and the result has the promoted type.
                        */
                        /**
                        * Basically convert bool to int, promote char to int, else do nothing
                        */
                        $$ = new Expression();
                        if($2->array != NULL){
                            Expression* tmp = new Expression();
                            tmp->loc = currentST->gentemp($2->type);
                            tmp->type = $2->type;
                            quadsArray->emit(ASSIGNARR,tmp->loc,($2->array),($2->loc));
                            $2 = tmp;

                        }
                        $2->convertBoolToInt();
                        typecheck($2,COMPLEMENT);
                        $$->loc = currentST->gentemp($2->type);
                        $$->type = $2->type;
                        quadsArray->emit(COMPLEMENT,($$->loc),($2->loc));
					}
					| '!' cast_expression
					{
                        /**
                        * Scalar Type : Arithmetic and pointer type
                        * Constraint : The operand of the ! operator, scalar type.
                        * Semantics : The result of the logical negation operator ! is 0
                        * if the value of its operand compares unequal to 0, 1 if the value
                        * of its operand compares equal to 0. The result has type int.
                        * The expression !E is equivalent to (0==E).
                        **/
                        /**
                        * Here a implicit bool type expression is generated which will
                        * anyway be converted to int if required
                        */
                        if($2->array != NULL){
                            Expression* tmp = new Expression();
                            tmp->loc = currentST->gentemp($2->type);
                            tmp->type = $2->type;
                            quadsArray->emit(ASSIGNARR,tmp->loc,($2->array),($2->loc));
                            $2 = tmp;

                        }
                        if($2->type->isScalarType() && $2->type->name != "bool"){
                            quadsArray->emit(OP_GOTO,quadsArray->getNextIndex()+1);
                            $2->convertToBool();
                        }
                        $2->swapLists();
                        $$ = $2;

					}
					| SIZEOF unary_expression
					{
                        $$ = new Expression();
                        $$->loc = currentST->gentemp(INT_TYPE,($2->type)->getSize(),CONSTANT_TEMP);
                        $$->type = INT_TYPE;
                        quadsArray->emit(COPY,($$->loc),($2->type)->getSize());

					}
					| SIZEOF '(' type_name ')'
					{
                        $$ = new Expression();
                        $$->loc = currentST->gentemp(INT_TYPE,$3->getSize(),CONSTANT_TEMP);
                        $$->type = INT_TYPE;
                        quadsArray->emit(COPY,($$->loc),$3->getSize());

					}
					;


cast_expression:	unary_expression
                    {
                        $$ =$1;
                    }
					| '(' type_name ')' cast_expression
					{
                        $$ = new Expression();
                        //TODO:: what is typename and how is it handled
					}
					;

multiplicative_expression:  cast_expression
                            {
                                if($1->array != NULL){
                                    $$ = new Expression();
                                    $$->loc = currentST->gentemp($1->type);
                                    $$->type = $1->type;
                                    /*if($1->array->type->name == "pointer" && 0)
                                    {
                                        SymbolTableRow* tmp = currentST->gentemp($1->array->type);
                                        quadsArray->emit(PLUS,tmp,$1->array,$1->loc);
                                        quadsArray->emit(ASSIGNVALUE,$$->loc,tmp);

                                    }
                                    else
                                    {*/
                                        quadsArray->emit(ASSIGNARR,$$->loc,($1->array),($1->loc));
                                    //}
                                    

                                }
                                else
                                {
                                    $$ = $1;
                                }
                            }
							| multiplicative_expression '*' cast_expression
							{

                                /**
                                * Constraint for multiplicatine expression :
                                *           Each of the operands shall have arithmetic type.
                                *           The operands of the % operator shall have integer type.
                                *
                                * Note : Constraints not checked due to assumption of correctness of code
                                *
                                */
                                $$ = new Expression();
                                if($3->array != NULL){
                                    Expression* tmp = new Expression();
                                    tmp->loc = currentST->gentemp($3->type);
                                    tmp->type = $3->type;
                                    quadsArray->emit(ASSIGNARR,tmp->loc,($3->array),($3->loc));
                                    $3 = tmp;

                                }
                                $1->convertBoolToInt();
                                $3->convertBoolToInt();

                                typecheck($1,$3,MULT);

                                $$->loc = currentST->gentemp($1->type);
                                $$->type = $1->type;
                                quadsArray->emit(MULT,($$->loc),($1->loc),($3->loc));


							}
							| multiplicative_expression '/' cast_expression
							{

                                $$ = new Expression();
                                if($3->array != NULL){
                                    Expression* tmp = new Expression();
                                    tmp->loc = currentST->gentemp($3->type);
                                    tmp->type = $3->type;
                                    quadsArray->emit(ASSIGNARR,tmp->loc,($3->array),($3->loc));
                                    $3 = tmp;

                                }
                                $1->convertBoolToInt();
                                $3->convertBoolToInt();

                                typecheck($1,$3,DIV);

                                $$->loc = currentST->gentemp($1->type);
                                $$->type = $1->type;
                                quadsArray->emit(DIV,($$->loc),($1->loc),($3->loc));


							}
							| multiplicative_expression '%' cast_expression
							{
                                $$ = new Expression();
                                if($3->array != NULL){
                                    Expression* tmp = new Expression();
                                    tmp->loc = currentST->gentemp($3->type);
                                    tmp->type = $3->type;
                                    quadsArray->emit(ASSIGNARR,tmp->loc,($3->array),($3->loc));
                                    $3 = tmp;

                                }
                                $1->convertBoolToInt();
                                $3->convertBoolToInt();

                                typecheck($1,$3,MOD);
                                $$->loc = currentST->gentemp($1->type);
                                $$->type = $1->type;
                                quadsArray->emit(MOD,($$->loc),($1->loc),($3->loc));
							}
							;

additive_expression:	multiplicative_expression
                        {
                            $$ = $1;
                        }
						| additive_expression '+' multiplicative_expression
						{
                            /**
                            * Constraint : For addition, either both operands shall have arithmetic type,
                            *                or one operand shall be a pointer to an object type and the other
                            *                shall have integer type. (Incrementing is equivalent to adding 1.)
                            *   TODO : Check for the sematics of pointer and integr addition
                            */
                            $$ = new Expression();
                            $1->convertBoolToInt();
                            $3->convertBoolToInt();

                            typecheck($1,$3,PLUS);
                            $$->loc = currentST->gentemp($1->type);
                            $$->type = $1->type;
                            if($1->type->name == "pointer")
                            {
                                SymbolTableRow *srow = currentST->gentemp($3->type);
                                quadsArray->emit(MULT,srow,srow,$1->type->getIdentity());
                                quadsArray->emit(PLUS,($$->loc),($1->loc),srow);
                            }
                            else if($3->type->name == "pointer")
                            {
                                SymbolTableRow *srow = currentST->gentemp($1->type);
                                quadsArray->emit(MULT,srow,srow,$3->type->getIdentity());
                                quadsArray->emit(PLUS,($$->loc),($3->loc),srow);

                            }
                            else
                            {
                                quadsArray->emit(PLUS,($$->loc),($1->loc),($3->loc));
                            }


                        }
						| additive_expression '-' multiplicative_expression
						{

                            /**
                            *   Constraint : For subtraction, one of the following shall hold:
                            *               -- both operands have arithmetic type;
                            *               -- both operands are pointers to qualified or
                            *                    unqualified versions of compatible object types; or
                            *               -- the left operand is a pointer to an object type and the
                            *                   right operand has integer type.
                            *   TODO : Check for the semantics of subtracting interger from  a pointer
                            *
                            *
                            */
                            $$ = new Expression();

                            $1->convertBoolToInt();
                            $3->convertBoolToInt();;

                            typecheck($1,$3,MINUS);

                            if($1->type->name == "pointer" && $3->type->name == "pointer")
                            {
                                $$->loc = currentST->gentemp(INT_TYPE);
                                $$->type = INT_TYPE;
                                quadsArray->emit(MINUS,$$->loc,$1->loc,$3->loc);
                                quadsArray->emit(DIV,$$->loc,$$->loc,$1->type->getIdentity());
                            }
                            else if($3->type->name == "pointer")
                            {
                                $$->loc = currentST->gentemp($1->type);
                                $$->type = $1->type;
                                SymbolTableRow *srow = currentST->gentemp($1->type);
                                quadsArray->emit(MULT,srow,srow,$3->type->getIdentity());
                                quadsArray->emit(MINUS,($$->loc),($3->loc),srow);

                            }
                            else
                            {
                                $$->loc = currentST->gentemp($1->type);
                                $$->type = $1->type;
                                quadsArray->emit(MINUS,($$->loc),($1->loc),($3->loc));
                            }
                        }
						;

shift_expression: 	additive_expression
                    {
                        $$ = $1;
                    }
					| shift_expression LEFT_SHIFT_OP additive_expression
					{
                        /**
                        * Constraint : Each of the operands shall have integer type.
                        *
                        */
                        $$ = new Expression();
                        $1->convertBoolToInt();
                        $3->convertBoolToInt();

                        typecheck($1,$3,SLL);
                        $$->loc = currentST->gentemp($1->type);
                        $$->type = $1->type;
                        quadsArray->emit(SLL,($$->loc),($1->loc),($3->loc));


					}
					| shift_expression RIGHT_SHIFT_OP additive_expression
					{

                        /**
                        * Semantic : The result of E1 >> E2 is E1 right-shifted E2 bit positions.
                        *            If E1 has an unsigned type or if E1 has a signed type and a
                        *            nonnegative value, the value of the result is the integral
                        *            part of the quotient of E1 / 2 E2 . If E1 has a signed type
                        *            and a negative value, the resulting value is implementation-defined.
                        *
                        */

                        $$ = new Expression();
                        $1->convertBoolToInt();
                        $3->convertBoolToInt();

                        typecheck($1,$3,SRA);
                        $$->loc = currentST->gentemp($1->type);
                        $$->type = $1->type;
                        quadsArray->emit(SRA,($$->loc),($1->loc),($3->loc));

					}
					;


relational_expression:  shift_expression
                        {
                            $$ = $1;
                        }
						| relational_expression '<' shift_expression
						{
                            $$ = new Expression();
                            $1->convertBoolToInt();
                            $3->convertBoolToInt();

                            typecheck($1,$3,IFLessGOTO);
                            $$->loc = NULL;
                            $$->type = BOOL_TYPE;
                            $$->truelist = makelist(quadsArray->getNextIndex());
                            quadsArray->emit(IFLessGOTO,-1,($1->loc),($3->loc));
                            $$->falselist = makelist(quadsArray->getNextIndex());
                            quadsArray->emit(OP_GOTO,-1);

						}
						| relational_expression '>' shift_expression
						{
                            $$ = new Expression();
                            $1->convertBoolToInt();
                            $3->convertBoolToInt();

                            typecheck($1,$3,IFGrtGOTO);
                            $$->loc = NULL;
                            $$->type = BOOL_TYPE;
                            $$->truelist = makelist(quadsArray->getNextIndex());
                            quadsArray->emit(IFGrtGOTO,-1,($1->loc),($3->loc));
                            $$->falselist = makelist(quadsArray->getNextIndex());
                            quadsArray->emit(OP_GOTO,-1);



						}
						| relational_expression LESS_EQUAL_OP shift_expression
						{
                            $$ = new Expression();
                            $1->convertBoolToInt();
                            $3->convertBoolToInt();

                            typecheck($1,$3,IFLessEqGOTO);
                            $$->loc = NULL;
                            $$->type = BOOL_TYPE;
                            $$->truelist = makelist(quadsArray->getNextIndex());
                            quadsArray->emit(IFLessEqGOTO,-1,($1->loc),($3->loc));
                            $$->falselist = makelist(quadsArray->getNextIndex());
                            quadsArray->emit(OP_GOTO,-1);

						}
						| relational_expression GREATER_EQUAL_OP shift_expression
						{
                            $$ = new Expression();
                            $1->convertBoolToInt();
                            $3->convertBoolToInt();

                            typecheck($1,$3,IFGrtEqGOTO);
                            $$->loc = NULL;
                            $$->type = BOOL_TYPE;
                            $$->truelist = makelist(quadsArray->getNextIndex());
                            quadsArray->emit(IFGrtEqGOTO,-1,($1->loc),($3->loc));
                            $$->falselist = makelist(quadsArray->getNextIndex());
                            quadsArray->emit(OP_GOTO,-1);

						}
						;

equality_expression:    relational_expression
                        {
                            $$ = $1;
                        }
						| equality_expression EQUALITY_OP relational_expression
						{
                            $$ = new Expression();
                            $1->convertBoolToInt();
                            $3->convertBoolToInt();

                            typecheck($1,$3,IFLogEqGOTO);
                            $$->loc = NULL;
                            $$->type = BOOL_TYPE;
                            $$->truelist = makelist(quadsArray->getNextIndex());
                            quadsArray->emit(IFLogEqGOTO,-1,($1->loc),($3->loc));
                            $$->falselist = makelist(quadsArray->getNextIndex());
                            quadsArray->emit(OP_GOTO,-1);
						}
						| equality_expression NOTEQUAL_OP relational_expression
						{
                            $$ = new Expression();
                            $1->convertBoolToInt();
                            $3->convertBoolToInt();

                            typecheck($1,$3,IFNotEqGOTO);
                            $$->loc = NULL;
                            $$->type = BOOL_TYPE;
                            $$->truelist = makelist(quadsArray->getNextIndex());
                            quadsArray->emit(IFNotEqGOTO,-1,($1->loc),($3->loc));
                            $$->falselist = makelist(quadsArray->getNextIndex());
                            quadsArray->emit(OP_GOTO,-1);
						}
						;

AND_expression:		equality_expression
                    {
                        $$ = $1;
                    }
					| AND_expression '&' equality_expression
					{
                        $$ = new Expression();
                         $1->convertBoolToInt();
                         $3->convertBoolToInt();

                         typecheck($1,$3,BitwiseAND);
                         $$->type = $1->type;
                         $$->loc = currentST->gentemp($$->type);
                         quadsArray->emit(BitwiseAND,($$->loc),($1->loc),($3->loc));
					}
					;

exclusive_OR_expression:	AND_expression
                            {
                                $$ = $1;
                            }
							| exclusive_OR_expression '^' AND_expression
							{
                                $$ = new Expression();
                                 $1->convertBoolToInt();
                                 $3->convertBoolToInt();

                                 typecheck($1,$3,BitwiseXOR);
                                 $$->type = $1->type;
                                 $$->loc = currentST->gentemp($$->type);
                                 quadsArray->emit(BitwiseXOR,($$->loc),($1->loc),($3->loc));
                            }
							;

inclusive_OR_expression:	exclusive_OR_expression
                            {
                                $$ =$1;
                            }
							| inclusive_OR_expression '|' exclusive_OR_expression
							{
                                $$ = new Expression();
                                 $1->convertBoolToInt();
                                 $3->convertBoolToInt();

                                 typecheck($1,$3,BitwiseOR);
                                 $$->type = $1->type;
                                 $$->loc = currentST->gentemp($$->type);
                                 quadsArray->emit(BitwiseOR,($$->loc),($1->loc),($3->loc));
                            }
							;

logical_AND_expression:		inclusive_OR_expression
                            {
                                $$ = $1;
                            }
							| logical_AND_expression N LOGICAL_AND_OP M inclusive_OR_expression
							{
                                $$ = new Expression();
                                if($5->type->isScalarType() && $5->type->name != "bool")
                                {
                                    quadsArray->emit(OP_GOTO,quadsArray->getNextIndex()+1);
                                    $5->convertToBool();
                                }
                                if($1->type->isScalarType() && $1->type->name != "bool"){
                                    quadsArray->backpatch($2->nextlist,quadsArray->getNextIndex()+1);
                                    $1->convertToBool();
                                }
                                else
                                    quadsArray->backpatch($2->nextlist,quadsArray->getNextIndex());
                                

                                typecheck($1,$5,AND);

                                quadsArray->backpatch($1->truelist,$4);
                                $$->type = BOOL_TYPE;
                                $$->truelist = $5->truelist;
                                $$->falselist = merge($1->falselist,$5->falselist);

							}
							;

logical_OR_expression:	logical_AND_expression
                        {
                            $$ = $1;
                        }
						| logical_OR_expression N LOGICAL_OR_OP M logical_AND_expression
						{
                            $$ = new Expression();
                            if($5->type->isScalarType() && $5->type->name != "bool")
                            {
                                quadsArray->emit(OP_GOTO,quadsArray->getNextIndex()+1);
                                $5->convertToBool();
                            }
                            if($1->type->isScalarType() && $1->type->name != "bool"){
                                quadsArray->backpatch($2->nextlist,quadsArray->getNextIndex()+1);
                                $1->convertToBool();
                            }
                            else
                                    quadsArray->backpatch($2->nextlist,quadsArray->getNextIndex());

                            typecheck($1,$5,OR);

                            quadsArray->backpatch($1->falselist,$4);
                            $$->type = BOOL_TYPE;
                            $$->falselist = $5->falselist;
                            $$->truelist = merge($1->truelist,$5->truelist);

                        }
						;

conditional_expression: logical_OR_expression
                        {
                            $$ = $1;
                        }
						| logical_OR_expression N '?' M expression N ':' M conditional_expression
						{
                            $$ = new Expression();

                            ///TODO: Check the code once
                            $5->convertBoolToInt();
                            $9->convertBoolToInt();

                            typecheck($1,$5,$9,COND);
                            if($1->type->isScalarType() && $1->type->name != "bool")
                            {
                                quadsArray->backpatch($2->nextlist,quadsArray->getNextIndex()+1);
                                $1->convertToBool();
                            }
                            else
                                    quadsArray->backpatch($2->nextlist,quadsArray->getNextIndex());

                            quadsArray->backpatch($1->truelist,$4);
                            quadsArray->backpatch($1->falselist,$8);

                            $$->loc = currentST->gentemp($5->type);
                            $$->type = $5->type;
                            quadsArray->emit(COPY,($$->loc),($5->loc));
                            quadsArray->backpatch($6->nextlist,quadsArray->getNextIndex());
                            quadsArray->emit(COPY,($$->loc),($9->loc));

						}

assignment_expression:  conditional_expression
                        {
                            $$ = $1;
                        }
						| unary_expression '=' assignment_expression
						{
                            $$ = new Expression();
                            //TODO : Check for array assignment
                            $1->convertBoolToInt();
                            $3->convertBoolToInt();

                            typecheck($1,$3,COPY);
                            $$->loc = currentST->gentemp($1->type);
                            $$->type = $1->type;
                            if($1->array != NULL)
                            {
                                /*if($1->array->type->name == "pointer" && 0)
                                {
                                    SymbolTableRow* tmp = currentST->gentemp($1->array->type);
                                    quadsArray->emit(PLUS,tmp,$1->array,$1->loc);
                                    quadsArray->emit(VALUEASSIGN,tmp,$3->loc);
                                    quadsArray->emit(ASSIGNVALUE,$$->loc,tmp);


                                }
                                else
                                {*/
                                    quadsArray->emit(ARRASSIGN,$1->array, $1->loc, $3->loc);
                                    quadsArray->emit(ASSIGNARR,($$->loc),($1->array),$1->loc);
                                //}
                                
                            }
                            else
                            {
                                
                                quadsArray->emit(COPY,($1->loc),($3->loc));
                                quadsArray->emit(COPY,($$->loc),($1->loc));
                            }
						}
						;


expression:		assignment_expression
                {
                    $$ = $1;
                }
				| expression ',' assignment_expression
				{
                    /* Comma operator not handled*/
				}
				;

constant_expression:	conditional_expression
                        {
                           /**
                            Not handled
                           */
                        }
						;
M   :   {
            $$ = quadsArray->getNextIndex();
        }

N :     {
            $$ = new ListType();
            $$->nextlist = makelist(quadsArray->getNextIndex());
            quadsArray->emit(OP_GOTO,-1);
        }

/**
Unused rules are deleted in the declaration phase because it amounts to lot of unused rules
**/

declaration:	declaration_specifiers ';'
                {
                    /* Wierd Rule */
                }
				| declaration_specifiers init_declarator_list ';'
				{
                    currentType = NULL;

				}
				;

declaration_specifiers:		type_specifier
                            {
                                $$ = $1;
                                currentType = $1;
                            }
							| type_specifier declaration_specifiers
							{
                                /**
                                * Unused rule
                                */
							}
							;

init_declarator_list:	init_declarator_list  {
                                                        currentType = $1;
                                                        if($1->name == "function"){
                                                            /*SymbolTableRow symrow = new SymbolTableRow(SymbolTableRow::RETNAME,
                                                                $2->type->getReturnType,RETURNVALUE);
                                                            currentST->insert(symrow);*/
                                                            //cout<<"I was here\n";
                                                            currentST->updateOffset();
                                                            currentST  = currentST->parent;

                                                        }
                                                    }
                        ',' init_declarator
						{
                            $$ = currentType;
                            if($4->type->name == "function"){
                                currentST->updateOffset();
                                currentST = currentST->parent;

                            }
                            /*Nothing to do I guess*/
						}
						|init_declarator
                        {
                            $$ = currentType;
                            //cout<<"I am always here   ";
                            //cout<<$1->type->name<<endl;

                            if($1->type->name == "function"){
                                currentST->updateOffset();
                                currentST = currentST->parent;

                            }
                        }
						;

init_declarator:	declarator
                    {
                        $$ = $1;
                    }
					| declarator '=' initializer
					{
                        quadsArray->emit(COPY,$1,$3);
                        $1->initialValue = $3->initialValue;
                        /*TODO:Initilaize using value constan*/
					}
					;


type_specifier:	VOID
                {
                    $$ = VOID_TYPE;
                }
				| CHAR
				{
                    $$ = CHAR_TYPE;
				}
				| INT
				{
                    $$ = INT_TYPE;
				}
				| DOUBLE
				{
                    $$ = DOUBLE_TYPE;
				}
				;

specifier_qualifier_list:   type_specifier
                            {
                                $$ = $1;
                            }
							| type_specifier specifier_qualifier_list
							;

declarator: direct_declarator
            {
                $$ = $1;
            }
			| pointer direct_declarator
			{
                Type * t = $2->type;
                for(int i = 0; i < $1; i++){
                    t = Pointer::makePointer(t);
                }
                $2->type = t;
                $$  = $2;
			}
			;

direct_declarator:  IDENTITY
                    {
                        SymbolTableRow* symrow;
                        symrow = currentST->lookup($1);
                        if(currentST->level == SymbolTable::GLOBALLEVEL){
                            if(symrow!=NULL && symrow->nestedTable != NULL){
                                    /*
                                    * Decide what to do when function defintion exists
                                    *
                                    */

                                    currentST = symrow->nestedTable;
                            }
                            else
                            {
                                //currentType->print(cout);
                                symrow = new SymbolTableRow($1,currentType,GLOBAL);
                                currentST->insert(symrow);
                            }
                        }
                        else
                        {
                            if(symrow!=NULL && symrow->categoryOfSymbol == PARAMETER)
                            {
                                symrow->type = currentType;
                            }
                            else
                            {
                                symrow = new SymbolTableRow($1,currentType,LOCAL);
                                currentST->insert(symrow);
                            }
                        }
                        $$ = symrow;
                    }
					| '(' declarator ')'
					{
                        $$ = $2;
					}
					| direct_declarator '[' assignment_expression ']'
					{
                        
                        $1->type = Array::makeArray($1->type,($3->loc)->initialValue.intvalue);
                        $$ = $1;


					}
					| direct_declarator '[' ']'
					{
                        //cout<<"here"<<endl;
                        //cout<<$1->name<<endl;
                        //($1->type)->print(cout);
                        //cout<<endl;
                        $1->type = Array::makeArray($1->type,1);
                        $$ = $1;

					}
					| direct_declarator '[' '*' ']'
					{
                        /**
                        *   Not handled
                        */
					}
					| direct_declarator   '('       {
                                                        $1->type = Function::makeFunction($1->type);

                                                        if($1->type->getname() == "function"){
                                                            if($1->nestedTable == NULL){
                                                                SymbolTable* st = new SymbolTable($1->name,currentST,SymbolTable::FUNCTIONLEVEL);
                                                                st->name = $1->name;
                                                                st->level = SymbolTable::FUNCTIONLEVEL;
                                                                st->parent = currentST;
                                                                $1->nestedTable = st;
                                                                currentST = st;
                                                                //cout<<"hhhhhhhhhhhhhhhhhhhhh"<<currentST->name<<endl;
                                                            }
                                                            else
                                                            {
                                                                //cout<<"heeeeeeeeeeeeeeeeeeeeeeeeee\n";
                                                                currentST = $1->nestedTable;
                                                            }

                                                        }
                                                    }
                     parameter_type_list ')'
                    {
                        Function::updateParameterList($1->type,$4);
                        $$ = $1;

                    }
					| direct_declarator '(' identifier_list ')'
					{
                        /**
                        * Not supported
                        */
					}
					| direct_declarator   '('      {   $1->type = Function::makeFunction($1->type);

                                                        if($1->type->name == "function"){
                                                            if($1->nestedTable == NULL){

                                                                SymbolTable* st = new SymbolTable($1->name,currentST,SymbolTable::FUNCTIONLEVEL);
                                                                st->name = $1->name;
                                                                st->level = SymbolTable::FUNCTIONLEVEL;
                                                                st->parent = currentST;
                                                                $1->nestedTable = st;
                                                                currentST = st;
                                                                //cout<<"hhhhhhhhhhhhhhhhhhhhh"<<currentST->name<<endl;
                                                            }
                                                            else
                                                            {
                                                                //cout<<"Came Hereeeeeeeeeeeeeeeeeee";
                                                                currentST = $1->nestedTable;
                                                            }


                                                        }
                                                    }

                         ')'
					{

                        Function::updateParameterList($1->type);
                        $$ = $1;

					}
					;


pointer:	'*' pointer
            {
                $$ = 1 + $2;
                //currentType = new Pointer(currentType);
            }
            | '*'
            {
                $$ = 1;
                //currentType = new Pointer(currentType);
            }
			;


parameter_type_list:    parameter_list
                        {
                            $$ = $1;
                        }
						| parameter_list ',' ELLIPSIS
						{
                            /**
                            * Not supported in implementation
                            */
						}
						;

parameter_list: parameter_declaration
                {
                    $1->categoryOfSymbol = PARAMETER;
                    if($1->type->name == "array"){
                        $1->type = ((Array*)$1->type)->getEquivalentPointer();
                    }
                    $$ = new ParameterList();
                    $$->plist = makelist($1->type);
                }
				| parameter_list ',' parameter_declaration
				{
                    $3->categoryOfSymbol = PARAMETER;
                    if($3->type->name == "array"){
                        $3->type = ((Array*)$3->type)->getEquivalentPointer();
                    }
                    $$ = new ParameterList();
                    $$->plist = merge($1->plist,makelist($3->type));
				}
				;

parameter_declaration: 	declaration_specifiers declarator
                        {
                            $$ = $2;
                        }
						|	declaration_specifiers
						{
                            $$ = currentST->insertDummyParam($1);

						}
						;

identifier_list:	IDENTITY
					|	identifier_list ',' IDENTITY
					;

type_name:	specifier_qualifier_list
            {
                $$ = $1;
            }
			;

initializer:	assignment_expression
                {
                    //cout<<currentST->name<<endl;
                    $1->convertBoolToInt();
                    $$ = $1->loc;
                }
				| '{'initializer_list '}'
				{
                    /**
                    * Not supported
                    */
				}
				| '{' initializer_list ',' '}'
				{
                    /**
                    * Not supported
                    */
				}
				;

initializer_list:	designation initializer
                    {
                        /**
                        * Not supported
                        */
                    }
					| initializer
					{
                        /**
                        * Not supported
                        */
					}
					| initializer_list ',' designation initializer
					{
                        /**
                        * Not supported
                        */
					}
					| initializer_list ',' initializer
					{
                        /**
                        * Not supported
                        */
					}
					;

designation:	designator_list '='
                {
                    /**
                    * Not supported
                    */
                }
				;

designator_list:	designator
                    {
                        /**
                        * Not supported
                        */
                    }
					| designator_list designator
					{
                        /**
                        * Not supported
                        */
					}
					;

designator: 	'[' constant_expression ']'
                {
                    /**
                    * Not supported
                    */
                }
				;

/*End of declaration phase grammar*/

statement:	labeled_statement
            {
                /**
                *  Not Used
                */
            }
			| B compound_statement
			{
                $$ = $2;
			}
			| expression_statement
			| selection_statement
			| iteration_statement
			| jump_statement
			;

labeled_statement:	IDENTITY ':' statement
                    {}
					| CASE constant_expression ':' statement{}
					| DEFAULT ':' statement{}
					;

compound_statement:	'{'  '}'
                    {
                        $$ = new ListType();
                    }
					| '{' block_item_list '}'
					{
                        quadsArray->backpatch($2->nextlist,quadsArray->getNextIndex());
                        currentST->updateOffset();
                        currentST = currentST->parent;
                        $$ = new ListType();
					}
					;

B : {
        SymbolTable* symtab = SymbolTable::getNewBlockTable();
        symtab->parent = currentST;
        currentST = symtab;
    }

block_item_list:	block_item
                    {
                        $$ = $1;
                    }
					| block_item_list M block_item
					{
                        //cout<<"Declaration Over\n";
                        quadsArray->backpatch($1->nextlist,$2);
                        $$ = $3;
					}
					;

block_item: declaration
            {
                $$ = new ListType();
            }
			| statement
			{
                $$ = $1;
			}
			;

expression_statement: 	expression ';'
                        {

                            $1->convertBoolToInt();

                            $$ = new ListType();
                        }
                        | ';'
                        {
                            $$ = new ListType;
                        }
						;

expression_opt: 	expression
                    {
                        $$ = $1;
                    }
					|
					{
                        $$ = new Expression();
					}
					;

selection_statement:	IF '(' expression N')'M statement %prec LOWER_THAN_ELSE
                        {
                            if($3->type->isScalarType() && $3->type->name != "bool")
                            {
                                quadsArray->backpatch($4->nextlist,quadsArray->getNextIndex()+1);
                                $3->convertToBool();
                            }
                            else
                                    quadsArray->backpatch($4->nextlist,quadsArray->getNextIndex());
                            quadsArray->backpatch($3->truelist,$6);
                            $$ = new ListType();
                            $$->nextlist = merge($3->falselist,$7->nextlist);

                        }
						| IF '(' expression N ')' M statement ELSE N M statement
						{
                            if($3->type->isScalarType() && $3->type->name != "bool")
                            {
                                quadsArray->backpatch($4->nextlist,quadsArray->getNextIndex()+1);
                                $3->convertToBool();
                            }
                            else
                                    quadsArray->backpatch($4->nextlist,quadsArray->getNextIndex());
                            quadsArray->backpatch($3->truelist,$6);
                            quadsArray->backpatch($3->falselist,$10);
                            $$ = new ListType();
                            $$->nextlist = merge(merge($9->nextlist,$7->nextlist),$11->nextlist);
						}
						| SWITCH '(' expression ')' statement
						{
                            /**
                            * Not supported
                            */
						}
						;

iteration_statement:    WHILE '(' M expression N ')' M statement
                        {
                            quadsArray->emit(OP_GOTO,$3);
                            quadsArray->backpatch($8->nextlist,$3);
                            if($4->type->isScalarType() && $4->type->name != "bool")
                            {
                                quadsArray->backpatch($5->nextlist,quadsArray->getNextIndex()+1);
                                $4->convertToBool();
                            }
                            else
                                    quadsArray->backpatch($5->nextlist,quadsArray->getNextIndex());
                            quadsArray->backpatch($4->truelist,$7);
                            $$ = new ListType();
                            $$->nextlist = $4->falselist;

                        }
						| DO M statement WHILE '(' expression N ')' ';'
						{
                            if($6->type->isScalarType() && $6->type->name != "bool")
                            {
                                quadsArray->backpatch($7->nextlist,quadsArray->getNextIndex()+1);
                                $6->convertToBool();
                            }
                            quadsArray->backpatch($6->truelist,$2);
                            $$ = new ListType();
                            $$->nextlist = $6->falselist;

						}
						| FOR '(' expression_opt ';' M expression N ';' M expression_opt N ')' M statement
						{
                            quadsArray->emit(OP_GOTO,$9);
                            quadsArray->backpatch($14->nextlist,$9);
                            if($6->type->isScalarType() && $6->type->name != "bool")
                            {
                                quadsArray->backpatch($7->nextlist,quadsArray->getNextIndex()+1);
                                $6->convertToBool();
                            }
                            else
                                    quadsArray->backpatch($7->nextlist,quadsArray->getNextIndex());
                            quadsArray->backpatch($6->truelist,$13);
                            quadsArray->backpatch($11->nextlist,$5);
                            $$ = new ListType();
                            $$->nextlist = $6->falselist;

						}
						| FOR '(' expression_opt ';' ';' M expression_opt ')'  statement
						{

                            quadsArray->emit(OP_GOTO,$6);
                            quadsArray->backpatch($9->nextlist,$6);
                            $$ = new ListType();

						}
						| FOR '(' declaration expression_opt ';'expression_opt ')' statement
						{}
						;

jump_statement: GOTO IDENTITY ';'
                {}
				| CONTINUE ';'
				{}
				| BREAK ';'
				{}
				| RETURN expression';'
				{
                    $2->convertBoolToInt();
                    //cout<<currentST->name;
                    quadsArray->emit(OP_RETURN,$2->loc);
                    $$ = new ListType();
				}
				| RETURN ';'
				{
                    quadsArray->emit(OP_RETURN);
                    $$ = new ListType();
				}
				;

translation_unit: 	{
                         quadsArray = new QuadArray();
                         globalST = new SymbolTable("global",NULL,SymbolTable::GLOBALLEVEL);
                         strTable = new StringTable();
                         currentST = globalST;
                    }
                    external_declaration
                    {
                        currentST  = globalST;
                    }
					| translation_unit external_declaration
					{
                        globalST->updateOffset();

					}
					;

external_declaration: 	function_definition
						| declaration
						;

function_definition:	declaration_specifiers declarator declaration_list compound_statement
                        {
                            /**
                            * Not supported
                            */

                        }
						| declaration_specifiers declarator { quadsArray->emit(__SLABEL,$2);}
                         compound_statement
						{
                            quadsArray->backpatch($4->nextlist,quadsArray->getNextIndex());
                            quadsArray->emit(__ELABEL,$2);
						}
						;



declaration_list:	declaration
                    {
                        /*Unused*/
                    }
					| declaration_list declaration
					{
                        /*unused*/
					}
					;


%%

void yyerror(const char *s ) {
    //cout<<s<<endl;
    printf("Error parsing - : %s at %s\n", s, yytext);
    exit(1);

}