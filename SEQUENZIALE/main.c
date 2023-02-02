#include "kmean.h"
#include "coordinateretrieval.h"
#include "utils.h"



int main(int argc, char** argv) {
    FILE* file_coordinate = fopen(argv[1], "r");
  	/*if (file_coordinate == NULL) {
      	perror("Unable to open file!");
      	exit(1);
    }*/
	int M = strtol(argv[2], NULL, 10);
	int N = strtol(argv[3], NULL, 10);
	int K = strtol(argv[4], NULL, 10);

	struct space sp1;
	space_init(&sp1, M, N, K);
	read_from_file(file_coordinate, &sp1, M, N);
	fclose(file_coordinate);

	k_mean(&sp1, K);

	/*for (int i = 0; i < sp1.number_of_points; i++) {
		for (int j = 0; j < sp1.dimensions; j++)
            printf("%lf\t", sp1.points[i].coordinates[j]);
		printf("%d\t%lf", sp1.points[i].cluster, sp1.points[i].distance_from_centroid);
        printf("\n");
    }*/


	space_free(&sp1, M, N, K);


	
}