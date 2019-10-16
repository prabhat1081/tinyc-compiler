%{
	#include <math.h>
	#include <stdio.h>
	#include "ass6_13CS10060_translator.h"
	#include "ass6_13CS10060.tab.h"
	#define YY_DECL extern "C" int yylex()
	static int lineNumber = 1;
%}


IDENTIFIER_NON_DIGIT 		[A-Za-z_]
DIGIT						[0-9]
IDENTIFIER					({IDENTIFIER_NON_DIGIT}({IDENTIFIER_NON_DIGIT}|{DIGIT})*)

NON_ZERO_DIGIT				[1-9]
ZERO 						"0"
INTEGER_CONSTANT 			(({NON_ZERO_DIGIT}({DIGIT}*))|{ZERO}+)

DIGIT_SEQUENCE				{DIGIT}+
SIGN 						[+-]
DOT							"."
EXPONENT_PART 				([eE]{SIGN}?{DIGIT_SEQUENCE})
FRACTIONAL_CONSTANT 		(({DIGIT_SEQUENCE}?{DOT}{DIGIT_SEQUENCE})|({DIGIT_SEQUENCE}{DOT}))
FLOATING_CONSTANT			(({FRACTIONAL_CONSTANT}{EXPONENT_PART}?)|({DIGIT_SEQUENCE}{EXPONENT_PART}))

ENUMERATION_CONSTANT 		({IDENTIFIER})

ESCAPE_SEQUENCE				([\\][\'\"\?\\abfnrtv])
C_CHAR 						[^\'\\\n]|{ESCAPE_SEQUENCE}
C_CHAR_SEQUENCE 			{C_CHAR}+
CHARACTER_CONSTANT 			(\'{C_CHAR_SEQUENCE}\')


S_CHAR 						[^\"\\\n]|{ESCAPE_SEQUENCE}
S_CHAR_SEQ 					{S_CHAR}+
STRING_LITERAL 				(\"{S_CHAR_SEQ}?\")

WHITESPACE					[ \t\n]
NEWLINECHAR						\n

SINGLE_LINE_COMMENT			[/][/].*
NOT_STAR					[^*\n]*
NOT_STAR_SLASH 				"*"+[^*/\n]*
STAR_SLASH 					"*"+"/"

%x comment

%%

{SINGLE_LINE_COMMENT}			{;}

"/*"							BEGIN(comment);
<comment>[^*\n]* 				/* eat anything that’s not a ’*’ */
<comment>"*"+[^*/\n]* 			/* eat up ’*’s not followed by ’/’s */
<comment>\n 					{lineNumber++;}
<comment>"*"+"/"                BEGIN(INITIAL);

"auto" 				{return AUTO;}
"break"				{return BREAK;}
"case"				{return CASE;}
"char" 				{return CHAR;}
"const"				{return CONST;}
"continue"			{return CONTINUE;}
"default"			{return DEFAULT;}
"do"				{return DO;}
"double"			{return DOUBLE;}
"else"				{return ELSE;}
"enum"				{return ENUM;}
"extern"			{return EXTERN;}
"float"				{return FLOAT;}
"for"				{return FOR;}
"goto"				{return GOTO;}
"if"				{return IF;}
"inline"			{return INLINE;}
"int" 				{return INT;}
"long"				{return LONG;}
"register"			{return REGISTER;}
"restrict"			{return RESTRICT;}
"return"			{return RETURN;}
"short"				{return SHORT;}
"signed" 			{return SIGNED;}
"sizeof"			{return SIZEOF;}
"static"			{return STATIC;}
"struct"			{return STRUCT;}
"switch"			{return SWITCH;}
"typedef"			{return TYPEDEF;}
"union"				{return UNION;}
"unsigned"			{return UNSIGNED;}
"void"				{return VOID;}
"volatile"			{return VOLATILE;}
"while"				{return WHILE;}
"_Bool"				{return _BOOL;}
"_Complex"			{return _COMPLEX;}
"_Imaginary"		{return _IMAGINARY;}

{IDENTIFIER} 			{yylval.identifier = strdup(yytext); return IDENTITY;}
{INTEGER_CONSTANT}			{ yylval.intVal = atoi(yytext); return INTEGER_CONST; }
{FLOATING_CONSTANT}			{ yylval.doubleVal = atof(yytext); return FLOATING_CONST; }
{ENUMERATION_CONSTANT} 		{ return ENUMERATION_CONST;}
{CHARACTER_CONSTANT} 			{ yylval.charVal = yytext[1] ;return CHARACTER_CONST; }
{STRING_LITERAL} 				{ yylval.stringVal = strdup(yytext); return STRING_LIT;}

"["  			{ return ('['); }
"]"  			{ return (']'); }
"("  			{ return ('('); }
")"  			{ return (')'); }
"{"  			{ return('{'); }
"}"             { return ('}'); }
"."             { return ('.'); }
"->"            { return (ARROW_OP); }
"++"            { return (INCREMENT_OP); }
"--"            { return  (DECREMENT_OP); }
"&"              { return  ('&'); }
"*"              { return ('*'); }
"+"              { return ('+'); }
"-"              { return ('-'); }
"~"              { return ('~'); }
"!"              { return ('!'); }
"/"              { return ('/'); }
"%"              { return ('%'); }
">>"              { return (RIGHT_SHIFT_OP); }
"<<"              { return (LEFT_SHIFT_OP); }
"<"              { return ('<'); }
">"              { return ('>'); }
">="              { return (GREATER_EQUAL_OP); }
"<="              { return (LESS_EQUAL_OP); }
"=="              { return (EQUALITY_OP); }
"!="              { return (NOTEQUAL_OP); }
"^"              { return ('^'); }
"|"              { return ('|'); }
"||"              { return (LOGICAL_OR_OP); }
"&&"              { return (LOGICAL_AND_OP); }
"?"              { return ('?'); }
":"              { return (':'); }
";"              { return (';'); }
"..."              { return (ELLIPSIS); }
"="              { return ('='); }
"*="              { return (MULT_ASSIGNMENT_OP); }
"/="              { return (DIV_ASSIGNMENT_OP); }
"%="              { return (MOD_ASSIGNMENT_OP); }
"+="              { return (ADD_ASSIGNMENT_OP); }
"-="              { return (SUB_ASSIGNMENT_OP); }
"<<="              { return (LSH_ASSIGNMENT_OP); }
">>="              { return (RSH_ASSIGNMENT_OP); }
"&="              { return (BIAND_ASSIGNMENT_OP); }
"^="              { return (BIXOR_ASSIGNMENT_OP); }
"|="              { return (BIOR_ASSIGNMENT_OP); }
","              { return (','); }
"#"              { return ('#'); }

{NEWLINECHAR} 					{lineNumber++;}
{WHITESPACE} 					{;}
%%
