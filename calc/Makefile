all: calc

calc: calc.l calc.y
	bison -d calc.y
	flex calc.l
	gcc -o $@ -DYYDEBUG=1 calc.tab.c lex.yy.c -lfl

clean:
	rm -f ./calc
	rm -f ./calc.tab.c ./calc.tab.h
	rm -f lex.yy.c
