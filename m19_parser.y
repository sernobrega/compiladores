%{
//-- don't change *any* of these: if you do, you'll break the compiler.
#include <algorithm>
#include <cdk/compiler.h>
#include "ast/all.h"
#define LINE               compiler->scanner()->lineno()
#define yylex()            compiler->scanner()->scan()
#define yyerror(s)         compiler->scanner()->error(s)
#define YYPARSE_PARAM_TYPE std::shared_ptr<cdk::compiler>
#define YYPARSE_PARAM      compiler
//-- don't change *any* of these --- END!

%}

%union {
  int                     i;	/* integer value */
  double                  d;  /* real value*/
  std::string             *s;	/* symbol name or string literal */
  cdk::basic_node         *node;	/* node pointer */
  cdk::sequence_node      *sequence;
  cdk::expression_node    *expression; /* expression nodes */
  cdk::lvalue_node        *lvalue;
  basic_type              *type;
  m19::block_node         *block; /* block node*/
  m19::section_init_node  *init;
  m19::section_end_node   *end;
  m19::section_node       *sec;
};

%token <i> tINTEGER
%token <d> tREAL
%token <s> tID tSTRING '@'
%token tREAD tPUBLIC tEXTERN tPRIVATE tPRINTNL tINCLUSIVE tEXCLUSIVE
%token tSTOP tCONTINUE tRETURN tAND tOR tIF tBEGINS tENDS

%right '='
%left tOR
%left tAND
%nonassoc '~'
%left tNE tEQ
%left tGE tLE '>' '<'
%left '+' '-'
%left '*' '/' '%'
%nonassoc '?'
%nonassoc tUNARY
%right '(' '['
%nonassoc ':'

%type <node> declaration vardecl func fundecl
%type <node> cond_i iter_i instruction fundef
%type <sequence> args secs declarations innerdecls vardecls exprs_in
%type <sequence> opt_instructions instructions exprs file secm
%type <expression> expr literal integer real
%type <lvalue> lval
%type <type> data_type pure_type
%type <block> block
%type <init> ini_sem
%type <end> end_sec end_sem
%type <sec> sec
%type <s> string

%{
//-- The rules below will be included in yyparse, the main parsing function.
%}
%%

file    	      : /* empty */                                               { compiler->ast($$ = new cdk::sequence_node(LINE)); }
                | declarations                                              { compiler->ast($$ = $1); }
                ;

declarations    : declaration                                               { $$ = new cdk::sequence_node(LINE, $1); }
                | declarations declaration                                  { $$ = new cdk::sequence_node(LINE, $2, $1); }
                ;

declaration     : vardecl ';'                                               { $$ = $1; }
                | func                                                      { $$ = $1; }
                ;

vardecl         : data_type tID                                             { $$ = new m19::variable_declaration_node(LINE, tPRIVATE, $1, *$2, nullptr); delete $2; }
                | data_type tID     '=' expr                                { $$ = new m19::variable_declaration_node(LINE, tPRIVATE, $1, *$2, $4); delete $2; }
                | data_type tID '!'                                         { $$ = new m19::variable_declaration_node(LINE, tPUBLIC, $1, *$2, nullptr); delete $2; }
                | data_type tID '!' '=' expr                                { $$ = new m19::variable_declaration_node(LINE, tPUBLIC, $1, *$2, $5); delete $2; }
                | data_type tID '?'                                         { $$ = new m19::variable_declaration_node(LINE, tEXTERN, $1, *$2, nullptr); delete $2; }
                ;

data_type       : pure_type                                                 { $$ = $1; }
                | '<'data_type'>'                                           { $$ = new basic_type(4, basic_type::TYPE_POINTER); $$->_subtype = $2; }
                ;

pure_type       : '#'                                                       { $$ = new basic_type(4, basic_type::TYPE_INT); }      
                | '%'                                                       { $$ = new basic_type(8, basic_type::TYPE_DOUBLE); }
                | '$'                                                       { $$ = new basic_type(4, basic_type::TYPE_STRING); }
                ;

func            : fundecl                                                   { $$ = $1; }
                | fundef                                                    { $$ = $1; }
                ;

fundecl         : data_type tID     args                                    { $$ = new m19::function_declaration_node(LINE, tPRIVATE, $1, *$2, $3);      delete $2; }
                | '!'       tID     args                                    { $$ = new m19::function_declaration_node(LINE, tPRIVATE,     *$2, $3);      delete $2; }
                | data_type tID '?' args                                    { $$ = new m19::function_declaration_node(LINE, tEXTERN , $1, *$2, $4);      delete $2; }
                | '!'       tID '?' args                                    { $$ = new m19::function_declaration_node(LINE, tEXTERN ,     *$2, $4);      delete $2; }
                | data_type tID '!' args                                    { $$ = new m19::function_declaration_node(LINE, tPRIVATE, $1, *$2, $4);      delete $2; }
                | '!'       tID '!' args                                    { $$ = new m19::function_declaration_node(LINE, tPRIVATE,     *$2, $4);      delete $2; }
                ;

vardecls			  : vardecl								                                    { $$ = new cdk::sequence_node(LINE, $1); }
                | vardecls ',' vardecl						                          { $$ = new cdk::sequence_node(LINE, $3, $1); }
                ;

fundef			    : data_type tID 	  args             ini_sem secs end_sec	  { $$ = new m19::function_definition_node(LINE, tPRIVATE, $1, *$2, $3, $4, $5, $6); delete $2;}
                | '!'       tID			args             ini_sem secs end_sec	  { $$ = new m19::function_definition_node(LINE, tPRIVATE,     *$2, $3, $4, $5, $6); delete $2; }
                | data_type tID '!' args             ini_sem secs end_sec	  { $$ = new m19::function_definition_node(LINE, tPUBLIC , $1, *$2, $4, $5, $6, $7); delete $2; }
                | '!'       tID '!' args             ini_sem secs end_sec	  { $$ = new m19::function_definition_node(LINE, tPUBLIC ,     *$2, $4, $5, $6, $7); delete $2; }
                | data_type tID 	  args '=' literal ini_sem secs end_sec	  { $$ = new m19::function_definition_node(LINE, tPRIVATE, $1, *$2, $3, $5, $6, $7, $8); delete $2; }
                | data_type tID '!' args '=' literal ini_sem secs end_sec	  { $$ = new m19::function_definition_node(LINE, tPUBLIC , $1, *$2, $4, $6, $7, $8, $9); delete $2; }
                | data_type tID 	  args                     secm end_sec	  { $$ = new m19::function_definition_node(LINE, tPRIVATE, $1, *$2, $3, nullptr, $4, $5); delete $2;}
                | '!'       tID			args                     secm end_sec	  { $$ = new m19::function_definition_node(LINE, tPRIVATE,     *$2, $3, nullptr, $4, $5); delete $2; }
                | data_type tID '!' args                     secm end_sec	  { $$ = new m19::function_definition_node(LINE, tPUBLIC , $1, *$2, $4, nullptr, $5, $6); delete $2; }
                | '!'       tID '!' args                     secm end_sec	  { $$ = new m19::function_definition_node(LINE, tPUBLIC ,     *$2, $4, nullptr, $5, $6); delete $2; }
                | data_type tID 	  args '=' literal         secm end_sec	  { $$ = new m19::function_definition_node(LINE, tPRIVATE, $1, *$2, $3, $5, nullptr, $6, $7); delete $2; }
                | data_type tID '!' args '=' literal         secm end_sec	  { $$ = new m19::function_definition_node(LINE, tPUBLIC , $1, *$2, $4, $6, nullptr, $7, $8); delete $2; }
                | data_type tID 	  args                          end_sem	  { $$ = new m19::function_definition_node(LINE, tPRIVATE, $1, *$2, $3, nullptr, nullptr, $4); delete $2;}
                | '!'       tID			args                          end_sem	  { $$ = new m19::function_definition_node(LINE, tPRIVATE,     *$2, $3, nullptr, nullptr, $4); delete $2; }
                | data_type tID '!' args                          end_sem	  { $$ = new m19::function_definition_node(LINE, tPUBLIC , $1, *$2, $4, nullptr, nullptr, $5); delete $2; }
                | '!'       tID '!' args                          end_sem	  { $$ = new m19::function_definition_node(LINE, tPUBLIC ,     *$2, $4, nullptr, nullptr, $5); delete $2; }
                | data_type tID 	  args '=' literal              end_sem	  { $$ = new m19::function_definition_node(LINE, tPRIVATE, $1, *$2, $3, $5, nullptr, nullptr, $6); delete $2; }
                | data_type tID '!' args '=' literal              end_sem	  { $$ = new m19::function_definition_node(LINE, tPUBLIC , $1, *$2, $4, $6, nullptr, nullptr, $7); delete $2; }
                ;

args            : '('          ')'                                          { $$ = new cdk::sequence_node(LINE); }
                | '(' vardecls ')'                                          { $$ = new cdk::sequence_node(LINE, $2); }
                ;

literal			    : integer                                                   { $$ = $1; }
                | real                                                      { $$ = $1; }
                | string                                                    { $$ = new cdk::string_node(LINE, $1); }
                ;

ini_sem         : tBEGINS block                                             { $$ = new m19::section_init_node(LINE, $2); };

secm            : sec                                                       { $$ = new cdk::sequence_node(LINE, $1); }
                | secm sec                                                  { $$ = new cdk::sequence_node(LINE, $1, new cdk::sequence_node(LINE, $2)); }
                ;
                
secs            : /* empty */                                               { $$ = nullptr; }
                | secm                                                      { $$ = $1; }
                ;

sec             : '[' expr ']' block                                        { $$ = new m19::section_node(LINE, tEXCLUSIVE, $2, $4); }
                | '(' expr ')' block                                        { $$ = new m19::section_node(LINE, tINCLUSIVE, $2, $4); }
                | '['      ']' block                                        { $$ = new m19::section_node(LINE, tEXCLUSIVE, nullptr, $3); }
                | '('      ')' block                                        { $$ = new m19::section_node(LINE, tINCLUSIVE, nullptr, $3); }
                |              block                                        { $$ = new m19::section_node(LINE, tINCLUSIVE, $1    ); }
                ;

end_sem         : tENDS block                                               { $$ = new m19::section_end_node(LINE, $2); };

end_sec         : /* empty */                                               { $$ = nullptr; }
                | end_sem                                                   { $$ = $1; }
                ;

block           : '{' innerdecls opt_instructions '}'                       { $$ = new m19::block_node(LINE, $2, $3); }
                | '{'            opt_instructions '}'                       { $$ = new m19::block_node(LINE, nullptr, $2); }
                ;

innerdecls      :            vardecl ';'                                    { $$ = new cdk::sequence_node(LINE, $1); }
                | innerdecls vardecl ';'                                    { $$ = new cdk::sequence_node(LINE, $2, $1); }
                ;

opt_instructions: /* empty */                                               { $$ = new cdk::sequence_node(LINE); }
                | instructions                                              { $$ = $1; }
                ;

instructions    : instruction                                               { $$ = new cdk::sequence_node(LINE, $1); }
                | instruction instructions                                  { std::reverse($2->nodes().begin(), $2->nodes().end()); $$ = new cdk::sequence_node(LINE, $1, $2); std::reverse($$->nodes().begin(), $$->nodes().end()); }
                ;

instruction     : tRETURN                                                   { $$ = new m19::return_node(LINE); }
                | tCONTINUE                                                 { $$ = new m19::continue_node(LINE); }
                | tSTOP                                                     { $$ = new m19::stop_node(LINE); }
                | expr ';'                                                  { $$ = new m19::evaluation_node(LINE, $1); }
                | expr '!'                                                  { $$ = new m19::print_node(LINE, $1, false); }
                | expr tPRINTNL                                             { $$ = new m19::print_node(LINE, $1, true); }
                | cond_i                                                    { $$ = $1; }
                | iter_i                                                    { $$ = $1; }
                | block                                                     { $$ = $1; }
                ;

cond_i          : '[' expr ']' '#' instruction                              { $$ = new m19::if_node(LINE, $2, $5); }
                | '[' expr ']' '?' instruction                              { $$ = new m19::if_node(LINE, $2, $5); }
                | '[' expr ']' '?' instruction ':' instruction              { $$ = new m19::if_else_node(LINE, $2, $5, $7); }
                ;

iter_i          : '[' vardecls  ';' exprs ';' exprs ']' instruction         { $$ = new m19::for_node(LINE, $2, $4, $6, $8); }
                | '[' exprs     ';' exprs ';' exprs ']' instruction         { $$ = new m19::for_node(LINE, $2, $4, $6, $8); }
                ;

exprs           : /* empty */                                               { $$ = new cdk::sequence_node(LINE); }
                | exprs_in                                                  { $$ = $1; }
                ;

exprs_in        : expr                                                      { $$ = new cdk::sequence_node(LINE, $1); }
                | exprs ',' expr                                            { $$ = new cdk::sequence_node(LINE, $3, $1); }
                ;

expr            : integer                                                   { $$ = $1; }
                | real                                                      { $$ = $1; }    
                | string                                                    { $$ = new cdk::string_node(LINE, $1); }
                | '@'                                                       { $$ = new m19::read_node(LINE); }

                | '-' expr %prec tUNARY                                     { $$ = new cdk::neg_node(LINE, $2); }
                | '+' expr %prec tUNARY                                     { $$ = new m19::identity_node(LINE, $2); }
                | '~' expr                                                  { $$ = new cdk::not_node(LINE, $2); }

                | expr '+' expr	                                            { $$ = new cdk::add_node(LINE, $1, $3); }
                | expr '-' expr	                                            { $$ = new cdk::sub_node(LINE, $1, $3); }

                | expr '*' expr	                                            { $$ = new cdk::mul_node(LINE, $1, $3); }
                | expr '/' expr	                                            { $$ = new cdk::div_node(LINE, $1, $3); }
                | expr '%' expr	                                            { $$ = new cdk::mod_node(LINE, $1, $3); }

                | expr '<' expr	                                            { $$ = new cdk::lt_node(LINE, $1, $3); }
                | expr '>' expr	                                            { $$ = new cdk::gt_node(LINE, $1, $3); }
                | expr tGE expr	                                            { $$ = new cdk::ge_node(LINE, $1, $3); }
                | expr tLE expr                                             { $$ = new cdk::le_node(LINE, $1, $3); }
                | expr tNE expr	                                            { $$ = new cdk::ne_node(LINE, $1, $3); }
                | expr tEQ expr	                                            { $$ = new cdk::eq_node(LINE, $1, $3); }

                | expr tAND expr                                            { $$ = new cdk::and_node(LINE, $1, $3); }
                | expr tOR  expr                                            { $$ = new cdk::or_node(LINE, $1, $3); }

                | '(' expr ')'                                              { $$ = $2; }
                | '[' expr ']'                                              { $$ = new m19::stack_alloc_node(LINE, $2); }

                | tID '(' exprs ')'                                         { $$ = new m19::function_call_node(LINE, *$1, $3); delete $1; }
                | '@' '(' exprs ')'                                         { $$ = new m19::function_call_node(LINE, *$1, $3); delete $1; }
                
                | lval                                                      { $$ = new cdk::rvalue_node(LINE, $1); }
                | lval '=' expr                                             { $$ = new cdk::assignment_node(LINE, $1, $3); }    
                | '@'  '=' expr                                             { $$ = new cdk::assignment_node(LINE, $1, $3); delete $1; }                               
                | lval '?'                                                  { $$ = new m19::address_node(LINE, $1); }
                ;

lval            : tID                                                       { $$ = new cdk::variable_node(LINE, $1); delete $1; }  
                | lval              '[' expr ']'                            { $$ = new m19::index_node(LINE, new cdk::rvalue_node(LINE, $1), $3); }
                |     '(' expr  ')' '[' expr ']'                            { $$ = new m19::index_node(LINE, $2, $5); }
                | tID '(' exprs ')' '[' expr ']'                            { $$ = new m19::index_node(LINE, new m19::function_call_node(LINE, *$1, $3), $6); delete $1; }
                | '@' '(' exprs ')' '[' expr ']'                            { $$ = new m19::index_node(LINE, new m19::function_call_node(LINE, *$1, $3), $6); delete $1; }
                ;

integer         : tINTEGER                                                  { $$ = new cdk::integer_node(LINE, $1); };

real            : tREAL                                                     { $$ = new cdk::double_node(LINE, $1); };

string          : tSTRING                                                   { $$ = $1; }
                | string tSTRING                                            { $$ = new std::string(*$1 + *$2); delete $1; delete $2; }
                ;

%%