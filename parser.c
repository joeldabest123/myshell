#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include <dirent.h>

#define MAX_TOKENS 100

int hasWildcard(char * str) {
    return strchr(str,'*')!=NULL;
}

void expandWildcard(char * token, char*** args, int * argIndex){
    
    char* slash = strrchr(token, '/');
    char dir[512] = ".";
    char pattern[256];

    if(slash){
        int len = slash - token;
        strncpy(dir, token, len);
        dir[len] = '\0';
        strcpy(pattern, slash + 1);
    } 
    
    else {
        strcpy(pattern, token);
    }

    
    DIR * d = opendir(dir);
    if(!d){
        (*args)[(*argIndex)++] = token;
        return;
    }

    struct dirent* entry;
    int found=0;

    while((entry=readdir(d))!=NULL) {
        char* name = entry->d_name;
        if (name[0] == '.' && pattern[0] != '.') continue;

        char* star = strchr(pattern, '*');
        int prefixLen = star - pattern;
        int suffixLen = strlen(pattern) - prefixLen - 1;

        if (strncmp(pattern, name, prefixLen) != 0) continue;
        if (suffixLen > 0) {
            if (strlen(name) < suffixLen) continue;
            if (strcmp(name + strlen(name) - suffixLen,
                       star + 1) != 0) continue;
        }

        found = 1;
        char full[1024];
        if (slash){
            snprintf(full,sizeof(full),"%s/%s",dir,name);
            (*args)[(*argIndex)++]=strdup(full);
        } 
        else {
            (*args)[(*argIndex)++]=strdup(name);
        }
    }

    closedir(d);

    if (!found) {
        (*args)[(*argIndex)++]=token;
    }
}

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

void free_pipeline(Pipeline *p) {
    for(int i = 0; i < p->commandCount; i++) {
        free(p->commands[i].arguments);
    }
    free(p->commands);
}

//we're building the pipelines from tokens
void buildPipeline(char** tokens, Pipeline* pipeline) {

    int commandCapacity = 4; //just assuming we start with 4 commands

    pipeline->commands = malloc(sizeof(Command) * commandCapacity); //allocating room for 4 commands
    pipeline->commandCount = 1;

    int commandIndex = 0;
    int argumentIndex = 0;

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
            pipeline->commands[commandIndex].outputFile = tokens[tokenIndex + 1];
            tokenIndex++;
        }

        //literally anything else :/
        else {
            if (hasWildcard(token)) {
                expandWildcard(token, &pipeline->commands[commandIndex].arguments,&argumentIndex);
            } else {
                pipeline->commands[commandIndex].arguments[argumentIndex++] = token;
            }
        }
    }
    //terminate last argument in argv so execv doesn't read into le garbage memory
    pipeline->commands[commandIndex].arguments[argumentIndex] = NULL;

}