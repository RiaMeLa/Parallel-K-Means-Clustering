#include "coordinateretrieval.h"
#include "utils.h"



void read_from_file(FILE* textfile, struct space* sp, int M, int N) {
	int point_index = 0;
  	while(1) {
		char* line = get_line(textfile);
		if (feof(textfile)) break;
      	/*Tokenize the line and assign point coordinates*/
		int coord = 0;
      	for (char* p = strtok(line," \t"); p != NULL; p = strtok(NULL, " ")) {
			char* endptr; 
  			sp->points[point_index].coordinates[coord] = strtod(p, &endptr);
          	coord++;
		}
      	point_index++;
    }
}




/* Counts the number of lines in the file,  UNUSED in current version*/
int rows_in_file(FILE* textfile) {
  	int lines = 0;
	char ch;

 	while(!feof(textfile)) {
    	ch = fgetc(textfile);
    	if(ch == '\n') 
    		lines++;
  	}
	rewind(textfile);
	
  	return lines;
}


/*POSIX getline() equivalent*/
char* get_line(FILE* textfile) {
    size_t size = 0;
    size_t len  = 0;
    size_t last = 0;
    char *buf = NULL;

    do {
        size += BUFSIZ; /* BUFSIZ is defined as "the optimal read size for this platform" */
        buf = realloc(buf, size);   
        if (buf == NULL) return NULL;
        fgets(buf + last, BUFSIZ, textfile);
        len = strlen(buf);
        last = len - 1;
    } while (!feof(textfile) && buf[last] != '\n');

    return buf;
}