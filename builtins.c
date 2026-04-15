
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "builtins.h"

int handleBuiltins(Command* command) {
    char** args = command->arguments;

    if(args[0]==NULL) { //no command
        return 1;
    }

    if(strcmp(args[0], "exit")==0) {
        exit(0);
    }

    if (strcmp(args[0], "cd")==0) {
        if (args[1]==NULL) { //no 2nd argument, return to home path
            chdir(getenv("HOME"));
        } else {
            if(chdir(args[1])!=0) {
                perror("cd"); //if failed
            }
        }
        return 1;
    }

    if (strcmp(args[0], "pwd")==0) {
        char cwd[4096];
        getcwd(cwd, sizeof(cwd));
        printf("%s\n", cwd);
        return 1;
    }

    //TODO: if first command is which...

    return 0; //if no if statements ran
}