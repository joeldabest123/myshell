#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include "parser.h"
#include "builtins.h"
#include "exec.h"
#include "structs.h"
#include <fcntl.h>

#define BUFFER_SIZE 1024

ssize_t readLine(int fd, char * buffer, size_t max){
    size_t i = 0;
    char c;

    while (i<max-1) {
        ssize_t n = read(fd, &c, 1);
        if (n <= 0) return n;
        buffer[i++]=c;
        if (c=='\n') break;
    }
    buffer[i] = '\0';
    return i;
}

void printPrompt(int isInteractive){
    if(isInteractive){
        char cwd[BUFFER_SIZE];
        char* home = getenv("HOME");
        getcwd(cwd, sizeof(cwd));

        if (strncmp(cwd, home, strlen(home))==0) {
            printf("~%s$ ", cwd + strlen(home));
        } 
        else {
            printf("%s$ ", cwd);
        }
        fflush(stdout);
    }

    else{
        write(STDOUT_FILENO, "$ ", 2);
    }
}

int main (int argc, char* argv[]) {
    char buffer[BUFFER_SIZE];

    int isInteractive = (argc==1 && isatty(STDIN_FILENO));
    if (isInteractive) {
        printf("Welcome!\n");
    }

    int fd_input = STDIN_FILENO;
    if (argc == 2) {
        fd_input = open(argv[1], O_RDONLY);
        if (fd_input < 0) {
            perror("file");
            exit(1);
        }
    }

    while(1) {
        
        printPrompt(isInteractive); //for the prompt

        ssize_t bytes = readLine(fd_input, buffer, BUFFER_SIZE - 1);

        if(bytes <= 0) break;

        buffer[bytes] = '\0';

        int count;
        char** tokens = tokenizeLine(buffer, &count);

        Pipeline pipeline;
        buildPipeline(tokens, &pipeline);

        int handled = 0;

        if(pipeline.commandCount == 1) {
            handled = handleBuiltins(&pipeline.commands[0]);
        }
            
        if (!handled) {
            run_pipeline(&pipeline);
        }

        free_pipeline(&pipeline);
        free(tokens);
    }
    
    if (isInteractive) {
        printf("Exiting...\n");
    }

    return 0;
}