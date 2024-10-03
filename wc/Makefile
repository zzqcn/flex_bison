all: wc

wc: wc.l
	flex wc.l
	gcc -o $@ lex.yy.c -lfl

clean:
	rm -f ./wc
	rm -f ./lex.yy.c
