#include "utils.h"
#include <math.h>

double euclidean(struct point p1, struct point p2, int dimensions){
    double distance_squared = 0;

    for (int i = 0; i < dimensions; i++) {
        distance_squared += (p1.coordinates[i] - p2.coordinates[i]) * (p1.coordinates[i] - p2.coordinates[i]);
    }
    
    return sqrt(distance_squared);
};

int equal(struct point p1, struct point p2, int dimensions) {
    for (int i = 0; i < dimensions; i++) 
        if (p1.coordinates[i] != p2.coordinates[i])
            return 0;
    return 1;
}

void add(struct point p1, struct point p2, int dimensions) { // adds p2's coordiate values into p1's
    for (int i = 0; i < dimensions; i++)
        p1.coordinates[i] += p2.coordinates[i];
}

void subtract(struct point p1, struct point p2, int dimensions) {
    for (int i = 0; i < dimensions; i++)
        p1.coordinates[i] -= p2.coordinates[i];
}

/*
 * Allocates the memory for the space
*/
void space_init(struct space* sp, int points, int dimensions, int clusters) {
    sp->dimensions = dimensions;
    sp->number_of_points = points;
    sp->clusters = clusters;
    sp->points = malloc(points * sizeof(struct point));
    sp->centroids = malloc(clusters * sizeof(struct centroid));
    for(int i = 0; i < points; i++){
        sp->points[i].coordinates = malloc(dimensions * sizeof(double));
        sp->points[i].cluster = 0;
    }
    for(int i = 0; i < clusters; i++) {
        sp->centroids[i].accumulator.coordinates = malloc(dimensions * sizeof(double));
        sp->centroids[i].centroid.coordinates    = malloc(dimensions * sizeof(double));
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