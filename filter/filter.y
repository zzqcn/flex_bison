%{
#include "filter.h"
#include "filter_lex.h"
%}

%define parse.error verbose

%union
{
  ast* tree;
  int val;
  char* str;
}

%token <val> ID NAME AGE LANG
%token ALL
%token <val> LOGIC CMP NUMBER
%token <str> STRING
%token EOL

%type<tree> exp term

%%

cmd:
  | cmd exp EOL { ast_show($2); ast_free($2); }
  ;

exp: ALL { $$ = nullptr; }
  | term { $$ = $1; }
  | exp LOGIC term { $$ = ast_new_lr($2, $1, $3); }
  ;

term: ID CMP NUMBER { $$ = ast_new_cmp($2, $1, $3); }
  | NAME CMP STRING { $$ = ast_new_cmp($2, $1, $3);}
  | AGE CMP NUMBER { $$ = ast_new_cmp($2, $1, $3);}
  | LANG CMP STRING { $$ = ast_new_cmp($2, $1, $3);}
  | '(' exp ')' { $$ = $2; }
  ;
