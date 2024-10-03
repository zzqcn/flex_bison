all: wc3

wc3: wc3.l
	flex wc3.l
	gcc -o $@ lex.yy.c

clean:
	rm -f ./wc3
	rm -f ./lex.yy.c
