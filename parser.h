
#include "structs.h"

#ifndef PARSER_H
#define PARSER_H

char** tokenizeLine(char*, int*);
void buildPipeline(char**, Pipeline*);
void free_pipeline(Pipeline *);

#endif