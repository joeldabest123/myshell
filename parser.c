#include <stdio.h>
#include <string.h>
#include "structs.h"

#define MAX_TOKENS 100

//tokenizes the command line into an array and returns said array
char** tokenizeLine(char * line, int* out_count) {
    int capacity = 10;
    int count = 0;

    char** tokens = malloc(capacity * sizeof(char*));

    char* comment = strchr(line, '#');

    if(comment) {
        *comment = '\0';
    }

    char* token = strtok(line, " \t\n"); //jumps to the first character that's not a space, indent, or newline
    while(token != NULL) {
        if(count >= capacity - 1) {
            capacity *= 2;
            tokens = realloc(tokens, capacity * sizeof(char*)); //extends the size of the array
        }

        tokens[count++] = token;
        token = strtok(NULL, " \t\n"); //after each word is read and tokenized, does it again
    }

    tokens[count] = NULL;

    if(out_count) {
        *out_count = count;
    }

    return tokens;

}

//we're building the pipelines from tokens
void buildPipeline(char** tokens, Pipeline* pipeline) {

    int commandCapacity = 4; //just assuming we start with 4 commands

    pipeline->commands = malloc(sizeof(Command) * commandCapacity); //allocating room for 4 commands
    pipeline->commandCount = 1;

    int commandIndex = 0;
    int argumentIndex = 0;

    pipeline->commands = malloc(sizeof(Command) * 8);
    pipeline->commandCount = 1;

    //initializing first command
    pipeline->commands[0].arguments = malloc(sizeof(char*) * 32);
    pipeline->commands[0].inputFile = NULL;
    pipeline->commands[0].outputFile = NULL;

    //to process the command left to right
    for(int tokenIndex = 0; tokens[tokenIndex] != NULL; tokenIndex++) {

        char* token = tokens[tokenIndex];

        //finds pipes
        if(strcmp(token, "|") == 0) {
            pipeline->commands[commandIndex].arguments[argumentIndex] = NULL;

            commandIndex++;
            pipeline->commandCount++;
            argumentIndex = 0;

            if(commandIndex >= commandCapacity) {
                commandCapacity *= 2;
                pipeline->commands = realloc(pipeline->commands, sizeof(Command) * commandCapacity);
            }

            pipeline->commands[commandIndex].arguments = malloc(sizeof(char*) * 32);
            pipeline->commands[commandIndex].inputFile = NULL;
            pipeline->commands[commandIndex].outputFile = NULL;

        }

        //redirects input
        else if (strcmp(token, "<") == 0) {
            pipeline->commands[commandIndex].inputFile = tokens[tokenIndex + 1];
            tokenIndex++;
        }

        //redirects output
        else if (strcmp(token, ">") == 0) {
            pipeline->commands[commandIndex].outputFile = token[tokenIndex + 1];
            tokenIndex++;
        }

        //literally anything else :/
        else {
            pipeline->commands[commandIndex].arguments[argumentIndex] = token;
            argumentIndex++;
        }
    }

    //terminate last argument in argv so execv doesn't read into le garbage memory
    pipeline->commands[commandIndex].arguments[argumentIndex] = NULL;

}