#ifndef KMEAN_H
#define KMEAN_H

#include "utils_pthread.h"
#include "utils.h"


void* k_mean(void* args);
void recompute_centroids(struct centroid* centroids, struct point* points, 
long N, long K, long my_first, long my_last, long first_cl, long last_cl, long first_n, 
long last_n, long tid, long M, long first_nn, long last_nn);


#endif
