#include "kmean.h"
#include "utils_pthread.h"
#include "coordinateretrieval.h"
#include "utils.h"
#include <stdio.h>
#include <unistd.h>
#include <time.h>

int main(int argc, char** argv) {
    FILE* file_coordinate = fopen(argv[1], "r");
  	if (file_coordinate == NULL) {
      	perror("Unable to open file!");
      	exit(1);
    }
	long M = strtol(argv[2], NULL, 10);
	long N = strtol(argv[3], NULL, 10);
	long K = strtol(argv[4], NULL, 10);
	struct space sp1;
	space_init(&sp1, M, N, K);
	read_from_file(file_coordinate, &sp1, M, N);
	fclose(file_coordinate);


	pthread_t* thread_handles;
	thread_count = strtol(argv[5], NULL, 10) - 1;
	thread_handles = malloc(thread_count*sizeof(pthread_t));
	thread_count++; //consider the main thread too
	struct thread_args* t_args = malloc(thread_count * sizeof(struct thread_args));
	t_args[0].tid = 0;
	t_args[0].thread_count = thread_count;
	t_args[0].sp = &sp1;
	init_pthread_structures();


	for(long thread = 1; thread < thread_count; thread++) {
		t_args[thread].tid = thread;
		t_args[thread].thread_count = thread_count;
		t_args[thread].sp = &sp1;
		pthread_create(&thread_handles[thread-1], NULL, k_mean, &t_args[thread]);
	}
	time_t begin = time(NULL);
	k_mean(&t_args[0]);
	time_t end = time(NULL);

	for(long thread = 0; thread < thread_count-1; thread++) {
		pthread_join(thread_handles[thread], NULL);
	}

	destroy_pthread_structures(thread_count, t_args, thread_handles);

	/*PER CONOSCERE I RISULTATI DELLA COMPUTAZIONE, SCOMMENTARE. VERRÀ STAMPATA UNA TABELLA CON I SEGUENTI ARGOMENTI:
	coordinate delle dimensioni del punto     cluster di appartenenza      distanza punto-centroide*/

	/*for (int i = 0; i < sp1.number_of_points; i++) {
		for (int j = 0; j < sp1.dimensions; j++)
            printf("%lf\t", sp1.points[i].coordinates[j]);
		printf("%d\t%lf", sp1.points[i].cluster, sp1.points[i].distance_from_centroid);
        printf("\n");
    }*/

	printf("time elapsed : %lu", end - begin);
	
	space_free(&sp1, M, N, K);
}

