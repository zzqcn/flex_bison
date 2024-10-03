%option noyywrap

%{
int chars = 0;
int words = 0;
int lines = 0;
%}

%%

[a-zA-Z]+ { words++; chars += strlen(yytext); }
\n        { chars++; lines++; }
.         { chars++; }

%%

int main(int argc, char** argv) {
  if(argc > 1) {
    yyin = fopen(argv[1], "r");
    if(NULL == yyin) {
      fprintf(stderr, "open file %s failed\n", argv[1]);
      return 1;
    }
  }

  yylex();
  printf("%d lines, %d words, %d chars\n", lines, words, chars);
  return 0;
}
