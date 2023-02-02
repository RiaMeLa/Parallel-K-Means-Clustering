#include "utils.h"
#include "kmean.h"

int* points_in_cluster;

void k_mean(struct space sp, int clusters, int thread_count)
{
    int M = sp.number_of_points;
    int N = sp.dimensions;
    int K = clusters;
    points_in_cluster = malloc(sizeof(int) * K);
    struct point *points = sp.points;
    struct centroid *centroids = sp.centroids;
    int converged = 1;
    unsigned int seed = 1998;

#pragma omp parallel default(none) shared(points, centroids, M, N, K, converged, points_in_cluster, seed) num_threads(thread_count)

    {
        /* Choose random centroid between points at first*/
        
/*PER FINI DIDATTICI, COME IL CONTROLLO DELLA COERENZA DEI RISULTATI,
SCEGLIAMO COME CENTROIDI INIZIALI I PRIMI K PUNTI. È POSSIBILE SCOMMENTARE IL CICLO COMMENTATO PER LA SCELTA RANDOMICA
DEI PRIMI CENTOIDI.
    */
    #   pragma omp single nowait
        for (int i = 0; i < K; i++)
        {
                centroids[i].centroid.cluster = i;
                centroids[i].centroid.dimensions = N;
                for (int coord = 0; coord < N; coord++)
                    centroids[i].centroid.coordinates[coord] = points[i].coordinates[coord];
        }
                    
    /*#pragma omp for nowait
            for (int i = 0; i < K; i++)
            {
                if(omp_get_thread_num() < K) {
                    int random = rand_r(&seed) % M;
                    centroids[i].centroid.cluster = random;
                    centroids[i].centroid.dimensions = N;
                    for (int coord = 0; coord < N; coord++)
                        centroids[i].centroid.coordinates[coord] = points[random].coordinates[coord];
                }
            }*/

    #pragma omp barrier

        while (1)
        {
    #pragma omp for nowait
            for (int i = 0; i < M; i++)
            {
                /*int t_c = omp_get_num_threads();
                printf("%i\n", t_c);*/
                /* For each point calculates the distance between the first centroid */
                int index_of_nearest = 0;
                double distance_from_nearest = euclidean(points[i], centroids[0].centroid, N);
                /* Then calculates the distance for every other centroids and checks if */
                /* ... the newly calculated distance is lower the the previous one 		*/
                for (int j = 1; j < K; j++)
                {
                    /*int t_c2 = omp_get_num_threads();
                    printf("%i\n", t_c2);*/
                    double dist = euclidean(points[i], centroids[j].centroid, N);
                    if (dist < distance_from_nearest)
                    {
                        distance_from_nearest = dist;
                        index_of_nearest = j;
                    }
                }
                /* If the cluster's index per point has not been modified
                    * ... the algorithm has converged 			 */
                if (points[i].cluster != index_of_nearest && converged) // convergence control
                    converged = 0;
                points[i].cluster = index_of_nearest;
                points[i].distance_from_centroid = distance_from_nearest;
            }
    #pragma omp barrier
            if (converged) {
                break;
            }
            /* If not converged, recompute the centroids for every point */
            recompute_centroids(centroids, points, M, K, N);
    #pragma omp single nowait
            converged = 1;
    #pragma omp barrier
        }
    }
/*######END OF PARALLEL REGION########*/
free(points_in_cluster);
}
/* Takes in input the array of centroids, the array of points, the number of points, centroids and
 * dimension of the util space	*/
void recompute_centroids(struct centroid *centroids, struct point *points, int M, int K, int N)
{
#pragma omp for nowait
    for (int i = 0; i < K; i++)
        points_in_cluster[i] = 0;

#pragma omp for collapse(2) nowait
    for (int i = 0; i < K; i++)
    {
        for (int coord = 0; coord < N; coord++)
            centroids[i].accumulator.coordinates[coord] = 0.0;
    }
#pragma omp barrier
    /* For each point in the space, increases the cluster point-counter corresponding to that point, adds the point to
      the cluster accumulator*/

#pragma omp for reduction(+ : points_in_cluster[:K])
    for (int i = 0; i < M; i++)
    {
        points_in_cluster[points[i].cluster]++;
    }

    for (int i = 0; i < M; i++)
    { // All iterate from i to M
#pragma omp for nowait
        for (int coord = 0; coord < N; coord++)
        { // each works on its portion of coords
            centroids[points[i].cluster].accumulator.coordinates[coord] += points[i].coordinates[coord];
        }
    }

    /* Compute the mean point(i.e. the new centroid) by dividing the accumulated coordinates
     * of the point by the number of the points in the cluster */
#pragma omp barrier
#pragma omp for collapse(2) nowait
    for (int i = 0; i < K; i++)
        for (int coord = 0; coord < N; coord++)
            centroids[i].centroid.coordinates[coord] = centroids[i].accumulator.coordinates[coord] / points_in_cluster[i];//centroids[i].points_in_cluster;
}
