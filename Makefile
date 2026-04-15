MYSHELL: main.c builtins.c exec.c parser.c
	gcc -Wall -g -o mysh main.c builtins.c exec.c parser.c -lm
