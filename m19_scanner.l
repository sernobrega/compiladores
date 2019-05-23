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

double doubleSciNot(char *yytext) {
    char c;
    while(std::yytext.get(c)) {
        printf("%c\n", c);
    }
        
    return 2.1;
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
"0x"                        yy_push_state(X_HEX);
<X_HEX>[0-9A-Fa-f]+         yylval.i = strtoul(yytext, nullptr, 16); if(errno != 0) yyerror("Overflow strtod"); yy_pop_state(); return tINTEGER;

0|[1-9][0-9]*                 yylval.i = strtoul(yytext, nullptr, 10); if(errno != 0) yyerror("Overflow strtod"); return tINTEGER;

 /* ------------------------------------------------------------ */
 /* --------------->            Reais           <--------------- */
 /* ------------------------------------------------------------ */
[0-9]+[afpnumkMGTPE][0-9]+ yyval.d = doubleSciNot(std::string *yytext); return tREAL;
 /* [0-9]+a             yy_push_state(X_a); yyless(yyleng - 1); strlit << *yytext; 
<X_a>[0-9]+             {
                            strlit << '.' + *yytext + "e-18";
                            yylval.d = strtod(strlit.str().c_str(), NULL); 
                            strlit.str("");
                            yy_pop_state();
                            return tREAL;
                        }

[0-9]+f             yy_push_state(X_f); yyless(yyleng - 1); strlit << *yytext; 
<X_f>[0-9]+             {
                            strlit << '.' + *yytext + "e-15";
                            yylval.d = strtod(strlit.str().c_str(), NULL);
                            strlit.str("");
                            yy_pop_state();
                            return tREAL;
                        }
                    
[0-9]+p             yy_push_state(X_p); yyless(yyleng - 1); strlit << *yytext; 
<X_p>[0-9]+             {
                            strlit << '.' + *yytext + "e-12";
                            yylval.d = strtod(strlit.str().c_str(), NULL);
                            strlit.str("");
                            yy_pop_state();
                            return tREAL;
                        }

[0-9]+n             yy_push_state(X_n); yyless(yyleng - 1); strlit << *yytext; 
<X_n>[0-9]+             {
                            strlit << '.' + *yytext + "e-9";
                            yylval.d = strtod(strlit.str().c_str(), NULL);
                            strlit.str("");
                            yy_pop_state();
                            return tREAL;
                        }

[0-9]+u             yy_push_state(X_u); yyless(yyleng - 1); strlit << *yytext; 
<X_u>[0-9]+             {
                            strlit << '.' + *yytext + "e-6";
                            yylval.d = strtod(strlit.str().c_str(), NULL);
                            strlit.str("");
                            yy_pop_state();
                            return tREAL;
                        }

[0-9]+m             yy_push_state(X_m); yyless(yyleng - 1); strlit << *yytext; 
<X_m>[0-9]+             {
                            strlit << '.' + *yytext + "e-3";
                            yylval.d = strtod(strlit.str().c_str(), NULL);
                            strlit.str("");
                            yy_pop_state();
                            return tREAL;
                        }

[0-9]+k             yy_push_state(X_k); yyless(yyleng - 1); strlit << *yytext; 
<X_k>[0-9]+             {
                            strlit << '.' + *yytext + "e+3";
                            yylval.d = strtod(strlit.str().c_str(), NULL);
                            strlit.str("");
                            yy_pop_state();
                            return tREAL;
                        }

[0-9]+M             yy_push_state(X_Me); yyless(yyleng - 1); strlit << *yytext;
<X_Me>[0-9]+             {
                            strlit << '.' + *yytext + "e+6";
                            yylval.d = strtod(strlit.str().c_str(), NULL);
                            strlit.str("");
                            yy_pop_state();
                            return tREAL;
                        }

[0-9]+G             yy_push_state(X_G); yyless(yyleng - 1); strlit << *yytext; 
<X_G>[0-9]+             {
                            strlit << '.' + *yytext + "e+9";
                            yylval.d = strtod(strlit.str().c_str(), NULL);
                            strlit.str("");
                            yy_pop_state();
                            return tREAL;
                        }

[0-9]+T             yy_push_state(X_T); yyless(yyleng - 1); strlit << *yytext; 
<X_T>[0-9]+             {
                            strlit << '.' + *yytext + "e+12";
                            yylval.d = strtod(strlit.str().c_str(), NULL);
                            strlit.str("");
                            yy_pop_state();
                            return tREAL;
                        }

[0-9]+P             yy_push_state(X_P); yyless(yyleng - 1); strlit << *yytext; 
<X_P>[0-9]+             {
                            strlit << '.' + *yytext + "e+15";
                            yylval.d = strtod(strlit.str().c_str(), NULL);
                            strlit.str("");
                            yy_pop_state();
                            return tREAL;
                        }

[0-9]+E             yy_push_state(X_E); yyless(yyleng - 1); strlit << *yytext; 
<X_E>[0-9]+             {
                            strlit << '.' + *yytext + "e+18";
                            yylval.d = strtod(strlit.str().c_str(), NULL);
                            strlit.str("");
                            yy_pop_state();
                            return tREAL;
                        }*/




[1-9][0-9]*\.?[0-9]+    yylval.d = strtod(yytext, NULL); if(errno != 0) yyerror("Overflow strtod"); return tREAL;

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