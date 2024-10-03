all: wc2

wc2: wc2.l
	flex wc2.l
	gcc -o $@ lex.yy.c

clean:
	rm -f ./wc2
	rm -f ./lex.yy.c
