%option noyywrap

%{
int chars = 0;
int words = 0;
int lines = 0;

int total_chars = 0;
int total_words = 0;
int total_lines = 0;
%}

%%

[a-zA-Z]+ { words++; chars += strlen(yytext); }
\n        { chars++; lines++; }
.         { chars++; }

%%

int main(int argc, char** argv) {
  int i;

  if(argc < 2) {
    yylex();
    printf("%d lines, %d words, %d chars\n", lines, words, chars);
    return 0;
  }

  for(i=1; i<argc; i++) {
    FILE* fp = fopen(argv[i], "r");
    if(NULL == fp) {
      fprintf(stderr, "open file %s failed\n", argv[1]);
      return 1;
    }

    yyrestart(fp);
    /* yyin = fp; */
    yylex();
    fclose(fp);
    printf("%d lines, %d words, %d chars %s\n", lines, words, chars, argv[1]);
    total_chars += chars; chars = 0;
    total_words += words; words = 0;
    total_lines += lines; lines = 0;
  }

  printf("%d lines, %d words, %d chars total\n", total_lines, total_words, total_chars);
  return 0;
}
