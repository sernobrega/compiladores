%option c++ prefix="m19_scanner_" outfile="m19_scanner.cpp"
%option stack noyywrap yylineno 8bit
%{ 
// make relevant includes before including the parser's tab file
#include <string>
#include <cdk/ast/sequence_node.h>
#include <cdk/ast/expression_node.h>
#include <cdk/ast/lvalue_node.h>
#include "m19_parser.tab.h"

// output stream for building string literals
static std::ostringstream strlit;

// don't change this
#define yyerror LexerError
%}
%x X_STRING X_COMMENT X_ISLASH
%%

 /* ---------------------------------------------------------- */
 /* ------------------->    Comentários   <------------------- */
 /* ---------------------------------------------------------- */
"//".*\n               ; /* ignore line comments */

"/*"                   yy_push_state(X_COMMENT);
<X_COMMENT>"/*"        yy_push_state(X_COMMENT);
<X_COMMENT>"*/"        yy_pop_state(X_COMMENT);
<X_COMMENT>.|\n        ; /* ignore content of contents */  

 /* ---------------------------------------------------------- */
 /* --------------->   Instrução condicional  <--------------- */
 /* ---------------------------------------------------------- */
"if"                   return tIF; 
"else if"              return tELIF;
"else"                 return tELSE; 

 /* ---------------------------------------------------------- */
 /* -------------->   Instrução de iteração    <-------------- */
 /* -------------->   Instrução de terminação  <-------------- */
 /* -------------->   Instrução de continuação <-------------- */
 /* -------------->   Instrução de retorno     <-------------- */
 /* ---------------------------------------------------------- */
[;]                    return *yytext;
">>@"                  return tSTOP;
"@<<"                  return tCONTINUE;
">@<"                  return tRETURN;

 /* ------------------------------------------------------------ */
 /* --------------->   Operadores de expressão  <--------------- */
 /* ------------------------------------------------------------ */
">="                   return tGE; 
"<="                   return tLE; 
"=="                   return tEQ; 
"!="                   return tNE; 
"&&"                   return tAND;
"||"                   return tOR;
[+\-*/~=()\]\[]         return *yytext;

 /* ---------------------------------------------------------- */
 /* ------------>   Delimitadores e separadores  <------------ */
 /* ------------>   Corpo (de função)            <------------ */
 /* ---------------------------------------------------------- */
[{}]                   return *yytext;
"!!"                   return tPRINTNL;
"<<"                   return tBEGINS;
">>"                   return tENDS;

 /* ---------------------------------------------------------- */
 /* ----------------->   Tipos de dados     <---------------- */
 /* ----------------->   Constantes         <----------------- */
 /* ----------------->   Símbolos Globais   <----------------- */
 /* ---------------------------------------------------------- */
"0"                    return tNULL;
[#%$<>%?@!,]           return *yytext;
'm19'                  return tMAIN;

 /* ------------------------------------------------------------ */
 /* --------------->          Inteiros          <--------------- */
 /* ------------------------------------------------------------ */
0x[0-9A-Fa-f]+              yylval.i = strtoul(yyless(yytext-2), nullptr, 16); return tINTEGER;

[1-9][0-9]*                 yylval.i = strtoul(yytext, nullptr, 10); return tINTEGER;

 /* ------------------------------------------------------------ */
 /* --------------->            Reais           <--------------- */
 /* ------------------------------------------------------------ */
[1-9][0-9]*\.?[0-9]+    yylval.d = strtod(yytext, NULL); return tREAL;

 /* ------------------------------------------------------------ */
 /* --------------->   Identificadores (nomes)  <--------------- */
 /* ------------------------------------------------------------ */
[A-Za-z][A-Za-z0-9_]*  yylval.s = new std::string(yytext); return tIDENTIFIER;

 /* ------------------------------------------------------------ */
 /* --------------->    Cadeias de caracteres   <--------------- */
 /* ------------------------------------------------------------ */
"\""                        yy_push_state(X_STRING);
<X_STRING>"\\"              yy_push_state(X_ISLASH);
<X_STRING>"\""              {
                            yylval.s = new std::string(strlit.str());
                            strlit.str("");
                            yy_pop_state();
                            return tSTRING;
                            }
<X_STRING>.                 strlit << *yytext;

<X_ISLASH>n                 strlit << '\n'; yy_pop_state();
<X_ISLASH>r                 strlit << '\r'; yy_pop_state();
<X_ISLASH>t                 strlit << '\t'; yy_pop_state();
<X_ISLASH>"\""              strlit << '"'; yy_pop_state();
<X_ISLASH>"\\"              strlit << '~'; yy_pop_state();
<X_ISLASH>[0-9A-Fa-f]{1,2}  {
                            strlit << (char)(unsigned char)strtoul(yytext, NULL, 16);
                            yy_pop_state();
                            }
<X_ISLASH>.                 strlit << *yytext; yy_pop_state();

 /* ------------------------------------------------------------ */
 /* --------------->           Outros           <--------------- */
 /* ------------------------------------------------------------ */
[ \t\n]+                    ; /* ignore whitespace */

.                           yyerror("Unknown character");

%%