#ifndef COORD_RETRIEVAL
#define COORD_RETRIEVAL
#include "utils.h"
#include <stdio.h>

void read_from_file(FILE* textfile, struct space* sp, int M, int N);
int rows_in_file(FILE* textfile);
char* get_line(FILE * textfile);

#endif