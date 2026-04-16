
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "builtins.h"

char* find_path(char *command) {

    if (!command) {
        return NULL;
    }

    if (strchr(command, '/')) {
        return strdup(command);
    }

    //used for searching the 3 required directories
    const char *dirs[] = {"/usr/local/bin", "/usr/bin", "/bin"};
    char path[1024];

    //runs through em
    for (int i = 0; i < 3; i++) {

        snprintf(path, sizeof(path), "%s/%s", dirs[i], command);

        if (access(path, X_OK) == 0) {
            return strdup(path);
        }
    }

    return NULL;
}

int handleBuiltins(Command* command) {
    char** args = command->arguments;

    if(args[0]==NULL) { //no command
        return 1;
    }

    if(strcmp(args[0], "exit")==0) {
        return 2;
    }

    if (strcmp(args[0], "cd") == 0) {
        char* dir = NULL;

        if (args[1] == NULL) {
            dir = getenv("HOME");
        } else if (strcmp(args[1], "~") == 0) {
            dir = getenv("HOME");
        } else if (args[1][0] == '~' && args[1][1] == '/') {
            static char expanded[4096];
            snprintf(expanded, sizeof(expanded), "%s/%s", getenv("HOME"), args[1] + 2);
            dir = expanded;
        } else {
            dir = args[1];
        }

        if (chdir(dir) != 0) {
            perror("cd");
        }
        return 1;
    }

    if (strcmp(args[0], "pwd")==0) {
        char cwd[4096];
        getcwd(cwd, sizeof(cwd));
        printf("%s\n", cwd);
        return 1;
    }

    if (strcmp(args[0], "which") == 0) {
        if (args[1]==NULL||args[2]!=NULL) {
            return 1;
        }

        if (strcmp(args[1], "cd")==0 || strcmp(args[1], "pwd")==0 || strcmp(args[1], "exit")==0 || strcmp(args[1], "which")==0) 
        {
            return 1;
        }

        char* path=find_path(args[1]);
        if (path) {
            printf("%s\n", path);
        }
        return 1;
    }

    return 0; //if no if statements ran
}