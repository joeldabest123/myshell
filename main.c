#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include "parser.h"
#include "builtins.h"
#include "exec.h"
#include "structs.h"

#define BUFFER_SIZE 1024

int main (int argc, char* argv[]) {
    char buffer[BUFFER_SIZE];

    int isInteractive = isatty(STDIN_FILENO);
    if (isInteractive) {
        printf("Welcome!\n");
    }

    while(1) {
        write(STDOUT_FILENO, "$ ", 2); //for the prompt

        ssize_t bytes = read(STDIN_FILENO, buffer, BUFFER_SIZE - 1);

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