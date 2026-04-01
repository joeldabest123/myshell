#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <dirent.h>
#include <sys/types.h>
#include "linkedList.h"

char* bufferBuilder(const char *, struct dirent*);
void buffToList(char* , off_t , List* );
void tokenize(const char *, off_t , List* );
void paceDirectories(const char *, List*** , int* , int* );

#endif
