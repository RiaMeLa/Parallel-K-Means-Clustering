#include "utils.h"
#include "kmean.h"

void k_mean(struct space* sp, int clusters) {
    int M = sp->number_of_points;       
    int N = sp->dimensions;             
    int K = clusters;                  
    struct point* points = sp->points;
    struct centroid* centroids = sp->centroids;

    /* Choose random centroid between points at first*/
    /*ATTUALMENTE, AL FINE DI POTER VERIFICARE LA COERENZA DEI RISULTATI FRA LE VARIE VERSIONI DEL PROGRAMMA
     SI SCELGONO I PRIMI TRE PUNTI COME CENTROIDI INIZIALI, CAMBIARE LA PORZIONE DI CODICE 
     SEGNATA PER SCEGLIERE PUNTI RANDOMICI*/
    srand(10);
    for (int i = 0; i < K; i++) {
        int random = rand() % M;
        centroids[i].centroid.cluster = i;
        centroids[i].centroid.dimensions = N;
        for (int coord = 0; coord < N; coord++)
            centroids[i].centroid.coordinates[coord] = points[i].coordinates[coord];
            // SOSTITUIRE i CON random---------------------->qui
    }

    while(1){
        int converged = 1;

        for (int i = 0; i < M; i++) {
            /* For each point calculates the distance between the first centroid */
            int index_of_nearest = 0;
            double distance_from_nearest = euclidean(points[i], centroids[0].centroid, N);
            /* Then calculates the distance for every other centroids and checks if */
            /* ... the newly calculated distance is lower the the previous one 		*/
            for (int j = 1; j < K; j++) {
                double dist = euclidean(points[i], centroids[j].centroid, N);
                if(dist < distance_from_nearest){
                    distance_from_nearest = dist;
                    index_of_nearest = j;
                }
            }
            /* If the cluster's index per point has not been modified 
             * ... the algorithm has converged 			 */
            if (points[i].cluster != index_of_nearest && converged) //convergence control
                converged = 0;
            points[i].cluster = index_of_nearest;
            points[i].distance_from_centroid = distance_from_nearest;
        }  
        if (converged) break;
	/* If not converged, recompute the centroids for every point */
        recompute_centroids(centroids, points, M, K, N);
    }





}
/* Takes in input the array of centroids, the array of points, the number of points, centroids and
 * dimension of the util space	*/
void recompute_centroids(struct centroid* centroids, struct point* points, int M, int K, int N) {
    for (int i = 0; i < K; i++) {
        centroids[i].points_in_cluster = 0;
        for (int coord = 0; coord < N; coord++) 
            centroids[i].accumulator.coordinates[coord] = 0.0;
    }
    /* For each point in the space, increases the cluster corresponding to that point, adds the point to
     * the cluster accumulator*/
    for (int i = 0; i < M; i++) { // Pensare al fatto che i thread possono lavorare su cluster diversi
        centroids[points[i].cluster].points_in_cluster++;
        add(centroids[points[i].cluster].accumulator, points[i], N);  
    }
    /* Compute the mean point(i.e. the new centroid) by dividing the accumulated coordinates  
    * of the points by the number of the points in the cluster */
    for (int i = 0; i < K; i++) 
        for (int coord = 0; coord < N; coord++) 
            centroids[i].centroid.coordinates[coord] = 
            centroids[i].accumulator.coordinates[coord] / centroids[i].points_in_cluster;


}
