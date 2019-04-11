%{
//-- don't change *any* of these: if you do, you'll break the compiler.
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
  int                   i;	/* integer value */
  double                d;  /* real value*/
  std::string          *s;	/* symbol name or string literal */
  cdk::basic_node      *node;	/* node pointer */
  cdk::sequence_node   *sequence;
  cdk::expression_node *expression; /* expression nodes */
  cdk::lvalue_node     *lvalue;
  basic_type           *type;
  m19::block_node      *block;
};

%token <i> tINTEGER
%token <d> tREAL
%token <s> tID tSTRING
%token tWHILE tPRINT tREAD tBEGIN tEND tPUBLIC tEXTERN tPRIVATE
%token tSTOP tCONTINUE tRETURN tELIF tAND tOR tMAIN
%token tBEGINS tENDS tNULL tIF tPRINTNL

%nonassoc tIFX
%nonassoc tELSE

%right '='
%left tGE tLE tEQ tNE '>' '<'
%left '+' '-'
%left '*' '/' '%'
%nonassoc tUNARY

%type <node> declaration vardecl fundecl fundef init_section end_section section instruction
%type <node> cond_i iter_i 
%type <sequence> expressions args body sections declarations innerdecls 
%type <sequence> opt_instructions instructions vars exprs file
%type <expression> expr literal integer real
%type <lvalue> lval
%type <type> data_type pure_type
%type <i> qualifier
%type <block> block
%type <s> string

%{
//-- The rules below will be included in yyparse, the main parsing function.
%}
%%

file    	    : /* empty */                                       { compiler->ast($$ = new cdk::sequence_node(LINE)); }
                | declarations                                      { compiler->ast($$ = $1); }
                ;

declarations    : declaration                                       { $$ = new cdk::sequence_node(LINE, $1); }
                | declarations declaration                          { $$ = new cdk::sequence_node(LINE, $2, $1); }
                ;

declaration     : vardecl ';'                                       { $$ = $1; }
                | fundecl                                           { $$ = $1; }
                | fundef                                            { $$ = $1; }
                ;

vardecl         : data_type tID                                     { $$ = new m19::variable_declaration_node(LINE, tPRIVATE, $1, *$2, nullptr); delete $2; }
                | data_type tID '=' expr                            { $$ = new m19::variable_declaration_node(LINE, tPRIVATE, $1, *$2, $4); delete $2; }
                | data_type tID '!'                                 { $$ = new m19::variable_declaration_node(LINE, tPUBLIC, $1, *$2, nullptr); delete $2; }
                | data_type tID '!' '=' expr                        { $$ = new m19::variable_declaration_node(LINE, tPUBLIC, $1, *$2, $5); delete $2; }
                | data_type tID '?'                                 { $$ = new m19::variable_declaration_node(LINE, tEXTERN, $1, *$2, nullptr); delete $2; }
                ;

data_type       : pure_type                                         { $$ = $1; }
                | '<'data_type'>'                                   { $$ = new basic_type(4, basic_type::TYPE_POINTER); $$->_subtype = $2; }
                ;

pure_type       : '#'                                               { $$ = new basic_type(4, basic_type::TYPE_INT); }      
                | '%'                                               { $$ = new basic_type(8, basic_type::TYPE_DOUBLE); }
                | '$'                                               { $$ = new basic_type(4, basic_type::TYPE_STRING); }
                ;

fundecl         : data_type tID                  		            { $$ = new m19::function_declaration_node(LINE, tPRIVATE, $1, *$2, nullptr); delete $2; }
                | '!' tID            			 		            { $$ = new m19::function_declaration_node(LINE, tPRIVATE,     *$2, nullptr); delete $2; }
				| data_type tID qualifier        		            { $$ = new m19::function_declaration_node(LINE, $3, $1, *$2, nullptr); delete $2; }
                | '!' tID qualifier        		 		            { $$ = new m19::function_declaration_node(LINE, $3,     *$2, nullptr); delete $2; }

				| data_type tID     	  '(' args ')'              { $$ = new m19::function_declaration_node(LINE, tPRIVATE, $1, *$2, nullptr); delete $2; }
                | '!' tID           	  '(' args ')'              { $$ = new m19::function_declaration_node(LINE, tPRIVATE,     *$2, nullptr); delete $2; }
				| data_type tID qualifier '(' args ')'              { $$ = new m19::function_declaration_node(LINE, $3, $1, *$2, nullptr); delete $2; }
				| '!' tID qualifier       '(' args ')'              { $$ = new m19::function_declaration_node(LINE, $3,           *$2, nullptr); delete $2; }
                ;

qualifier		: '!'									            { $$ = tPUBLIC; }
				| '?'									            { $$ = tEXTERN; }
				;

args			: vardecl								            { $$ = new cdk::sequence_node(LINE, $1); }
				| args ',' vardecl						            { $$ = new cdk::sequence_node(LINE, $3, $1); }
				;

fundef			: data_type tID 	'(' args ')' body				{ $$ = new m19::function_definition_node(LINE, tPRIVATE, $1, *$2, $4, $6); delete $2;}
				| '!' tID			'(' args ')' body				{ $$ = new m19::function_definition_node(LINE, tPRIVATE,     *$2, $4, $6); delete $2; }
				| data_type tID '!' '(' args ')' body				{ $$ = new m19::function_definition_node(LINE, tPUBLIC,  $1, *$2, $5, $7); delete $2; }
				| '!' tID '!'       '(' args ')' body				{ $$ = new m19::function_definition_node(LINE, tPUBLIC,      *$2, $5, $7); delete $2; }
				| data_type tID 	'(' args ')' '=' literal body	{ $$ = new m19::function_definition_node(LINE, tPRIVATE, $1, *$2, $4, $8, $7); delete $2; }
				| '!' tID			'(' args ')' '=' literal body	{ $$ = new m19::function_definition_node(LINE, tPRIVATE,     *$2, $4, $8, $7); delete $2; }
				| data_type tID '!' '(' args ')' '=' literal body	{ $$ = new m19::function_definition_node(LINE, tPUBLIC,  $1, *$2, $5, $9, $8); delete $2; }
				| '!' tID '!'       '(' args ')' '=' literal body	{ $$ = new m19::function_definition_node(LINE, tPUBLIC,      *$2, $5, $9, $8); delete $2; }
				;

literal			: integer                                           { $$ = $1; }
				| real                                              { $$ = $1; }
				| string                                            { $$ = $1; }
				;

body			: init_section sections                             { $$ = new cdk::sequence_node(LINE, $1, $2); }
				| init_section end_section                          { $$ = new cdk::sequence_node(LINE, $1, $2); }
                | init_section                                      { $$ = new cdk::sequence_node(LINE, $1); }
                | sections                                          { $$ = new cdk::sequence_node(LINE, $1); }
                | end_section                                       { $$ = new cdk::sequence_node(LINE, $1); }
				; 

init_section    : '<<' block                                        { $$ = new m19::section_init_node(LINE, $2); };

sections        : section                                           { $$ = new cdk::sequence_node(LINE, $1); }
                | sections section                                  { $$ = new cdk::sequence_node(LINE, $1, $2); }
                | section end_section                               { $$ = new cdk::sequence_node(LINE, $1, $2); }
                ;

section         : '[' expr ']' block                                { $$ = new m19::section_node(LINE, 2, $4, $2); }
                | '(' expr ')' block                                { $$ = new m19::section_node(LINE, 1, $4, $2); }
                | block                                             { $$ = new m19::section_node(LINE, 0, $1    ); }
                ;

end_section     : '>>' block                                        { $$ = new m19::section_end_node(LINE, $2); };

block           : '{' innerdecls opt_instructions '}'              { $$ = new m19::block_node(LINE, $2, $3); }
                | '{'             opt_instructions '}'              { $$ = new m19::block_node(LINE, nullptr, $2); }
                ;

innerdecls      :            vardecl ';'                            { $$ = new cdk::sequence_node(LINE, $1); }
                | innerdecls vardecl ';'                            { $$ = new cdk::sequence_node(LINE, $2, $1); }
                ;

opt_instructions: /* empty */                                       { $$ = cdk::sequence_node(LINE); }
                | instructions                                      { $$ = $1; }
                ;

instructions    : instruction                                       { $$ = cdk::sequence_node(LINE, $1); }
                | instruction instructions                          { $$ = $1} //FIXME dk the order
                ;

instruction     : tRETURN                                           { $$ = m19::return_node(LINE); }
                | tCONTINUE                                         { $$ = m19::continue_node(LINE); }
                | tSTOP                                             { $$ = m19::stop_node(LINE); }
                | expr ';'                                          { $$ = m19::evaluation_node(LINE, $1); }
                | expr '!'                                          { $$ = m19::print_node(LINE, false); }
                | expr tPRINTNL                                     { $$ = m19::print_node(LINE, true); }
                | cond_i                                            { $$ = $1; }
                | iter_i                                            { $$ = $1; }
                | block                                             { $$ = $1; }
                ;

cond_i          : '[' expr ']' '#' instruction                      { $$ = m19::if_node(LINE, $2, $5); }
                | '[' expr ']' '?' instruction                      { $$ = m19::if_node(LINE, $2, $5); }
                | '[' expr ']' '?' instruction ':' instruction      { $$ = m19::if_else_node(LINE, $2, $5, $7); }
                ;

iter_i          : '[' vars ';' exprs ';' exprs ']' instruction      { $$ = m19::for_node(LINE, $2, $4, $6, $8); }
                | '[' exprs ';' exprs ';' exprs ']' instruction     { $$ = m19::for_node(LINE, $2, $4, $6, $8); }
                ;

vars            : /* empty */                                       { $$ = cdk::sequence_node(LINE); }
                | args                                              { $$ = $1; }
                ;

exprs           : /* empty */                                       { $$ = cdk::sequence_node(LINE); }
                | expr                                              { $$ = cdk::sequence_node(LINE, $1); }
                | exprs ',' expr                                    { $$ = cdk::sequence_node(LINE, $1, $3); }
                ;

expr            : literal                                           { $$ = $1; }
                | '@'                                               { $$ = new m19::read_node(LINE); }

                | '-' expr %prec tUNARY                             { $$ = new cdk::neg_node(LINE, $2); }
                | '+' expr %prec tUNARY                             { $$ = new m19::identity_node(LINE, $2); }
                | '~' expr %prec tUNARY                             { $$ = new cdk::not_node(LINE, $2); }

                | expr '+' expr	                                    { $$ = new cdk::add_node(LINE, $1, $3); }
                | expr '-' expr	                                    { $$ = new cdk::sub_node(LINE, $1, $3); }

                | expr '*' expr	                                    { $$ = new cdk::mul_node(LINE, $1, $3); }
                | expr '/' expr	                                    { $$ = new cdk::div_node(LINE, $1, $3); }
                | expr '%' expr	                                    { $$ = new cdk::mod_node(LINE, $1, $3); }

                | expr '<' expr	                                    { $$ = new cdk::lt_node(LINE, $1, $3); }
                | expr '>' expr	                                    { $$ = new cdk::gt_node(LINE, $1, $3); }
                | expr tGE expr	                                    { $$ = new cdk::ge_node(LINE, $1, $3); }
                | expr tLE expr                                     { $$ = new cdk::le_node(LINE, $1, $3); }
                | expr tNE expr	                                    { $$ = new cdk::ne_node(LINE, $1, $3); }
                | expr tEQ expr	                                    { $$ = new cdk::eq_node(LINE, $1, $3); }

                | expr tAND expr                                    { $$ = new cdk::and_node(LINE, $1, $3); }
                | expr tOR expr                                     { $$ = new cdk::or_node(LINE, $1, $3); }

                | '(' expr ')'                                      { $$ = $2; }
                | '[' expr ']'                                      { $$ = new m19::stack_alloc_node(LINE, $2); }

                | tID '(' args ')'                                  { $$ = new m19::function_call_node($1, $3);}

                | lval                                              { $$ = new cdk::rvalue_node(LINE, $1); }  //FIXME
                | lval '=' expr                                     { $$ = new cdk::assignment_node(LINE, $1, $3); }
                | lval '?'                                          { $$ = new m19::address_node(LINE, $1); }
                ;

lval            : tID                                               { $$ = new cdk::variable_node(LINE, $1); }
                | lval '[' expr ']'                               { $$ = new m19::index_node(LINE, $1, $3); }
                // | '@'                                               { $$ = new m19::index_node(LINE, )}
                // |
                // |
                ;

integer         : tINTEGER                                          { $$ = new cdk::integer_node(LINE, $1); };

real            : tREAL                                             { $$ = new cdk::double_node(LINE, $1); };

string          : tSTRING                                           { $$ = $1; }
                | string tSTRING                                    { $$ = std::string(*$1, *$2); delete $1; delete $2; }
                ;

%%