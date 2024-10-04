%option nodefault noyywrap case-insensitive

%{
#include "filter.h"
#include "filter_syn.h"
%}

%%

id      { yylval.val = ID; return ID; }
name    { yylval.val = NAME; return NAME; }
age     { yylval.val = AGE; return AGE; }
lang    { yylval.val = LANG; return LANG; }

all     { return ALL; }

"=="  { yylval.val = CMP_EQ; return CMP; }
"!="  { yylval.val = CMP_NE; return CMP; }
"<"   { yylval.val = CMP_LT; return CMP; }
">"   { yylval.val = CMP_GT; return CMP; }
"<="  { yylval.val = CMP_LE; return CMP; }
">="  { yylval.val = CMP_GE; return CMP; }

"||"  { yylval.val = CMP_OR; return LOGIC; }
"&&"  { yylval.val = CMP_AND; return LOGIC; }

"("|")" { return yytext[0]; }

[0-9]+    { yylval.val = atoi(yytext); return NUMBER; }

[a-zA-Z]+ { yylval.str = yytext; return STRING; }

[ \t] {}

\n  { return EOL; }

.   { printf("bad input: %s\n", yytext);
      yyterminate();
    }
