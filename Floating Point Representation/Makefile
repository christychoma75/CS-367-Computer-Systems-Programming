all: fp all_values 

all_values:	output_all_values.c fp.h
	gcc -o all_values output_all_values.c -lm

fp:	fp_program.o fp_functs.o lex.yy.o test_pgm.o
	gcc -o fp fp_program.o fp_functs.o test_pgm.o lex.yy.o -lfl -lm

fp_functs.o:	fp_functs.c fp.h
	gcc -g -c fp_functs.c	

fp_program.o:	fp_program.c fpParse.h
	gcc -c fp_program.c	

lex.yy.o:	lex.yy.c fpParse.h
	gcc -c lex.yy.c

lex.yy.c:	fp.l
	flex fp.l

clean:
	rm -rf lex.yy.c lex.yy.o fp_program.o fp_functs.o 

reallyclean:
	rm -rf fp all_values lex.yy.c lex.yy.o fp_program.o fp_functs.o 
