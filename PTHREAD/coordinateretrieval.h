#ifndef COORD_RETRIEVAL
#define COORD_RETRIEVAL
#include "utils.h"
#include <stdio.h>

extern int point_index;
extern char** thread_lines;

void read_from_file(FILE* textfile, struct space* sp, int points, int dimensions);
int rows_in_file(FILE* textfile);
char* get_line(FILE* textfile);
void init_thread_lines(long thread_count);

#endif