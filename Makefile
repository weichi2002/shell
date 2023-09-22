all: 
	gcc -Wall wei-chi-sh.c -o shell    

run: 
	./shell

try: all run