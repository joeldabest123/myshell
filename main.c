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

ssize_t readLine(int fd, char* buffer, size_t max) {
    size_t i = 0;
    char c;
    while (i < max - 1) {
        ssize_t num = read(fd, &c, 1);
        if (num < 0) return -1;
        if (num == 0) return (i > 0) ? (ssize_t)i : 0; // EOF
        buffer[i++] = c;
        if (c == '\n') break;
    }
    buffer[i] = '\0';
    return (ssize_t)i;
}

void printPrompt(int isInteractive) {
    if (isInteractive) {
        char cwd[BUFFER_SIZE];
        char* home = getenv("HOME");

        getcwd(cwd, sizeof(cwd));

        if (strncmp(cwd, home, strlen(home)) == 0) {
            printf("~%s$ ", cwd + strlen(home));
        } else {
            printf("%s$ ", cwd);
        }

        fflush(stdout);
    }
    else {
        printf("$ ");
        fflush(stdout);
    }
}

int main (int argc, char* argv[]) {
    char buffer[BUFFER_SIZE];

    int isInteractive = (argc == 1 && isatty(STDIN_FILENO));
    
    int input_fd = STDIN_FILENO;

    if (argc == 2) {
        input_fd = open(argv[1], O_RDONLY);
        if (input_fd < 0) {
            perror("file");
            exit(1);
        }
    }

    if (isInteractive) {
        printf("Welcome!\n");
    }

    while(1) {
        //write(STDOUT_FILENO, "$ ", 2); //for the prompt

        printPrompt(isInteractive);

        ssize_t bytes = readLine(STDIN_FILENO, buffer, BUFFER_SIZE);

        if(bytes <= 0) break;

        buffer[bytes] = '\0';

        int count;
        char** tokens = tokenizeLine(buffer, &count);

        Pipeline pipeline;
        buildPipeline(tokens, &pipeline);

        int handled = 0;

        if(pipeline.commandCount == 1) {
            handled = handleBuiltins(&pipeline.commands[0]);
        
            if (handled==2) {
                free_pipeline(&pipeline);
                free(tokens);
                break;
            }

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