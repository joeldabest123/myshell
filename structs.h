#ifndef STRUCTS_H
#define STRUCTS_H

typedef struct {
    char* **arguments;
    char* *inputFile;
    char* *outputFile;
} Command;

typedef struct {
    Command *commands;
    int commandCount;
} Pipeline;

#endif