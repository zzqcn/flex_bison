%{
#include <stdio.h>

extern int yylex (void);
void yyerror(const char* s);
%}

%define parse.error verbose

%token NUMBER
%token ADD SUB MUL DIV
%token EOL

%%

calclist:
  | calclist exp EOL { printf("= %d\n", $2); }
  ;

exp: term { $$ = $1; }
  | exp ADD term { $$ = $1 + $3; }
  | exp SUB term { $$ = $1 - $3; }
  ;

term: NUMBER { $$ = $1; }
  | term MUL NUMBER { $$ = $1 * $3; }
  | term DIV NUMBER { $$ = $1 / $3; }
  ;

%%

void yyerror(const char* s) {
  fprintf(stderr, "error: %s\n", s);
}

int main(int argc, char** argv) {
  yydebug = 1;
  yyparse();
}

