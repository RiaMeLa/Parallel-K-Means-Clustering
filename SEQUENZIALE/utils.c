#include "utils.h"

#include <math.h>

double euclidean(struct point p1, struct point p2, int dimension){
    double distance_squared = 0;

    for (int i = 0; i < dimension; i++) {
        distance_squared += (p1.coordinates[i] - p2.coordinates[i]) * (p1.coordinates[i] - p2.coordinates[i]);
    }
    
    return sqrt(distance_squared);
};

int equal(struct point p1, struct point p2, int dimension) {
    for (int i = 0; i < dimension; i++) 
        if (p1.coordinates[i] != p2.coordinates[i])
            return 0;
    return 1;
}

void add(struct point p1, struct point p2, int dimension) { // adds p2's coordiate values into p1's
    for (int i = 0; i < dimension; i++)
        p1.coordinates[i] += p2.coordinates[i];
}

void subtract(struct point p1, struct point p2, int dimension) {
    for (int i = 0; i < dimension; i++)
        p1.coordinates[i] -= p2.coordinates[i];
}



/*
 * Allocates the memory for the space
*/
void space_init(struct space* sp, int M, int N, int K) {
    sp->dimensions = N;
    sp->number_of_points = M;
    sp->clusters = K;
    sp->points = malloc(M * sizeof(struct point));
    sp->centroids = malloc(K * sizeof(struct centroid));
    for(int i = 0; i < M; i++){
        sp->points[i].coordinates = malloc(N * sizeof(double));
        sp->points[i].cluster = 0;
    }
    for(int i = 0; i < K; i++) {
        sp->centroids[i].accumulator.coordinates = malloc(N * sizeof(double));
        sp->centroids[i].centroid.coordinates    = malloc(N * sizeof(double));
        sp->centroids[i].points_in_cluster = 0;
    }
}

/*
frees the memory of an allocated space
*/
void space_free(struct space* sp, int M, int N, int K) {
    for(int i = 0; i < M; i++)
        free(sp->points[i].coordinates);
    for(int i = 0; i < K; i++) {
        free(sp->centroids[i].accumulator.coordinates);
        free(sp->centroids[i].centroid.coordinates); 
    }
    free(sp->points);
    free(sp->centroids);
}



