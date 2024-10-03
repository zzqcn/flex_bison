%option nodefault noyywrap yylineno

%x COMMENT
%x IFILE

/* some complex named patterns */
/* Universal Character Name */
UCN   (\\u[0-9a-fA-F]{4}|\\U[0-9a-fA-F]{8})
/* float exponent */
EXP   ([Ee][-+]?[0-9]+)
/* integer length */
ILEN  ([Uu](L|l|LL|ll)?|(L|l|LL|ll)[Uu]?)

%{
#include "cref.h"
%}

%%

  /* comments */
"/*"           { BEGIN(COMMENT) ; }
<COMMENT>"*/"  { BEGIN(INITIAL); }
<COMMENT>([^*]|\en)+|.

  /* C++ comment, a common extension */
"//".*\n

  /* declaration keywords */
_Bool |
_Complex |
_Imaginary |
auto |
bool |
char |
const |
double |
enum |
extern |
float |
inline |
int |
long |
register |
restrict |
short |
signed |
static |
struct |
typedef |
union |
unsigned |
void |
volatile { defining = 1; }


  /* keywords */
break
case
continue
default
do
else
for
goto
if
return
sizeof
switch
while

  /* constants */

  /* integers */
0[0-7]*{ILEN}?
[1-9][0-9]*{ILEN}?
0[Xx][0-9a-fA-F]+{ILEN}?

  /* decimal float */
([0-9]*\.[0-9]+|[0-9]+\.){EXP}?[flFL]?
[0-9]+{EXP}[flFL]?

  /* hex float */
0[Xx]([0-9a-fA-F]*\.[0-9a-fA-F]+|[0-9a-fA-F]+\.?)[Pp][-+]?[0-9]+[flFL]?


  /* char const */
\'([^'\\]|\\['"?\\abfnrtv]|\\[0-7]{1,3}|\\[Xx][0-9a-fA-F]+|{UCN})+\'

  /* string literal */
L?\"([^\"\\]|\\['"?\\abfnrtv]|\\[0-7]{1,3}|\\[Xx][0-9a-fA-F]+|{UCN})+\"

  /* punctuators */
"{"|"<%"|";"         { defining = 0; }


"["|"]"|"("|")"|"{"|"}"|"."|"->"
"++"|"--"|"&"|"*"|"+"|"-"|"~"|"!"
"/"|"%"|"<<"|">>"|"<"|">"|"<="|">="|"=="|"!="|"^"|"|"|"&&"|"||"
"?"|":"|";"|"..."
"="|"*="|"/="|"%="|"+="|"-="|"<<="|">>="|"&="|"^=""|="
","|"#"|"##"
"<:"|":>"|"%>"|"%:"|"%:%:"

  /* identifier */
([_a-zA-Z]|{UCN})([_a-zA-Z0-9]|{UCN})* {
      add_ref(yylineno, cur_filename, yytext, defining);
    }

  /* white space */
[ \t\n]+
  /* continued line */
\\$

  /* some preprocessor stuff */
"#"" "*if.*\n
"#"" "*else.*\n
"#"" "*endif.*\n
"#"" "*define.*\n
"#"" "*line.*\n

  /* recognize an include */
^"#"[ \t]*include[ \t]*[\"<] { BEGIN IFILE; }

<IFILE>[^>\"]+  { 
      {
        int c;
        while((c = yyinput()) && c != '\n') ;
      }
      new_file(strdup(yytext));
      BEGIN INITIAL;
    }

<IFILE>.|\n     {
      fprintf(stderr, "%s:%d bad include line\n", cur_filename, yylineno);
      BEGIN INITIAL;
    }

<<EOF>>         { if(!pop_file()) yyterminate(); }

  /* invalid character */
.               {
      printf("%s:%d: Mystery character '%s'\n",
        cur_filename, yylineno, yytext);
      yyterminate();
    }
