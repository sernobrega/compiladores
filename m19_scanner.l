%option c++ prefix="m19_scanner_" outfile="m19_scanner.cpp"
%option stack noyywrap yylineno 8bit debug
%{ 
// make relevant includes before including the parser's tab file
#include <string>
#include <vector>
#include <sstream>
#include <cdk/ast/sequence_node.h>
#include <cdk/ast/expression_node.h>
#include <cdk/ast/lvalue_node.h>
#include "m19_parser.tab.h"

// don't change this
#define yyerror LexerError

// output stream for building string literals
static std::ostringstream strlit;

double doubleSciNot(char *yytext, int yyleng) {
    int i = 0;
    double ret;
    std::string * exponent;

    while (i < yyleng) {
        if(isdigit(yytext[i])) {
            strlit << yytext[i];
        } else {
            switch(yytext[i]) {
                case 'a':
                    exponent = new std::string("e-18");
                    break;
                case 'f':
                    exponent = new std::string("e-15");
                    break;
                case 'p':
                    exponent = new std::string("e-12");
                    break;
                case 'n':
                    exponent = new std::string("e-9");
                    break;
                case 'u':
                    exponent = new std::string("e-6");
                    break;
                case 'm':
                    exponent = new std::string("e-3");
                    break;
                case 'k':
                    exponent = new std::string("e+3");
                    break;
                case 'M':
                    exponent = new std::string("e+6");
                    break;
                case 'G':
                    exponent = new std::string("e+9");
                    break;
                case 'T':
                    exponent = new std::string("e+12");
                    break;
                case 'P':
                    exponent = new std::string("e+15");
                    break;
                case 'E':
                    exponent = new std::string("e+18");
            }
            strlit << '.';
        }
        i++;
    }

    strlit << *exponent;
    ret = strtod(strlit.str().c_str(), NULL); 
    if(errno != 0) 
        throw std::string( "overflow strod" );
    strlit.str("");
    return ret;
}

%}
%x X_STRING X_COMMENT X_ISLASH X_HEX
%x X_a X_f X_p X_n X_u X_m X_k X_Me X_G X_T X_P X_E
%%

 /* ---------------------------------------------------------- */
 /* ------------------->    Comentários   <------------------- */
 /* ---------------------------------------------------------- */
"//".*\n               ; /* ignore line comments */

"/*"                   yy_push_state(X_COMMENT);
<X_COMMENT>"/*"        yy_push_state(X_COMMENT);
<X_COMMENT>"*/"        yy_pop_state();
<X_COMMENT>.|\n        ; /* ignore content of contents */  

 /* ---------------------------------------------------------- */
 /* -------------->   Instrução de iteração    <-------------- */
 /* -------------->   Instrução de terminação  <-------------- */
 /* -------------->   Instrução de continuação <-------------- */
 /* -------------->   Instrução de retorno     <-------------- */
 /* --------------->   Instrução condicional  <--------------- */
 /* ---------------------------------------------------------- */
[;:\]\[]               return *yytext;
">>@"                  return tSTOP;
"@>>"                  return tCONTINUE;
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
[+\-*/~=()]            return *yytext;

 /* ---------------------------------------------------------- */
 /* ------------>   Delimitadores e separadores  <------------ */
 /* ------------>   Corpo (de função)            <------------ */
 /* ---------------------------------------------------------- */
"!!"                   return tPRINTNL;
"<<"                   return tBEGINS;
">>"                   return tENDS;
[\{\}\!]               return *yytext;


 /* ---------------------------------------------------------- */
 /* ----------------->   Tipos de dados     <---------------- */
 /* ----------------->   Constantes         <----------------- */
 /* ----------------->   Símbolos Globais   <----------------- */
 /* ---------------------------------------------------------- */
[#%$<>%?!,]            return *yytext;

 /* ------------------------------------------------------------ */
 /* --------------->          Inteiros          <--------------- */
 /* ------------------------------------------------------------ */
"0x"                          yy_push_state(X_HEX);
<X_HEX>[0-9A-Fa-f]+           yylval.i = strtoul(yytext, nullptr, 16); if(errno != 0) yyerror("Overflow strtod"); yy_pop_state(); return tINTEGER;

0|[1-9][0-9]*                 yylval.i = strtoul(yytext, nullptr, 10); if(errno != 0) yyerror("Overflow strtod"); return tINTEGER;

 /* ------------------------------------------------------------ */
 /* --------------->            Reais           <--------------- */
 /* ------------------------------------------------------------ */
[0-9]+[afpnumkMGTPE][0-9]+    yylval.d = doubleSciNot(yytext, yyleng); return tREAL;

[1-9][0-9]*\.?[0-9]+          yylval.d = strtod(yytext, NULL); if(errno != 0) yyerror("Overflow strtod"); return tREAL;

 /* ------------------------------------------------------------ */
 /* --------------->  Identificadores (nomes)  <--------------- */
 /* ------------------------------------------------------------ */
[@]                    yylval.s = new std::string(yytext); return *yytext; 
[A-Za-z][A-Za-z0-9_]*  yylval.s = new std::string(yytext); return tID;

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
<X_STRING>\n                yyerror("Illegal change of line within string");

<X_ISLASH>n                 strlit << '\n'; yy_pop_state();
<X_ISLASH>r                 strlit << '\r'; yy_pop_state();
<X_ISLASH>t                 strlit << '\t'; yy_pop_state();
<X_ISLASH>"\""              strlit << '"'; yy_pop_state();
<X_ISLASH>"\\"              strlit << '\\'; yy_pop_state();
<X_ISLASH>[0-9A-Fa-f]{1,2}  {
                            strlit << (char)(unsigned char)strtoul(yytext, NULL, 16);
                            yy_pop_state();
                            }
<X_ISLASH>.                 strlit << *yytext; yy_pop_state();
<X_ISLASH>\n                 yyerror("Illegal change of line within string");

 /* ------------------------------------------------------------ */
 /* --------------->           Outros           <--------------- */
 /* ------------------------------------------------------------ */
[ \t\n]+                    ; /* ignore whitespace */

.                           yyerror("Unknown character");

%%