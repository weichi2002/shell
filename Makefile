all: 
	gcc -Wall wei-chi-sh.c -ggdb3 -o shell    

run: 
	./shell

try: all run