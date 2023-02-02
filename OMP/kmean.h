#ifndef KMEAN_H
#define KMEAN_H

#include "utils.h"


void k_mean(struct space sp, int clusters, int thread_count);
void recompute_centroids(struct centroid* centroids, struct point* points, int M, int K, int N);



#endif
