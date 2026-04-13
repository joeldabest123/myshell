#include <stdio.h>
#include <string.h>

#define MAX_TOKENS 100

//tokenizes the command line into an array and returns said array
char** tokenize(char * line, int* out_count) {
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