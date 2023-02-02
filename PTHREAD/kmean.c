#include <stdlib.h>
#include "utils.h"
#include "utils_pthread.h"
#include "kmean.h"

int converged = 1;

void* k_mean(void* args) {
    struct thread_args* t_args = (struct thread_args*) args;
    long thread_count = t_args->thread_count;
    long tid = t_args->tid;
    struct space* sp = t_args->sp;
    long M = sp->number_of_points;       
    long N  = sp->dimensions;             
    long K = sp->clusters;                  
    struct point* points = sp->points;
    struct centroid* centroids = sp->centroids;

    long local_points = M / (t_args->thread_count); 
    long resto = M % (t_args->thread_count);
    if (resto && tid < resto) local_points++;

   
    long local_first_point; 
    long local_last_point;  
    long local_first_cluster;
    long local_last_cluster;
    long local_first_n;
    long local_last_n;
    long local_first_nn;
    long local_last_nn;
    compute_indexes(local_points, M, K, N, thread_count, tid,
     resto, &local_first_point, &local_last_point, &local_first_cluster, &local_last_cluster,
     &local_first_n, &local_last_n, &local_first_nn, &local_last_nn);
    //printf("tid = %d, myfc = %d, mylc = %d\n", tid, local_first_cluster, local_last_cluster);
   

    //PER VERIFICHE FAR FARE IL LAVORO A THREAD 0 
    /*LA PORZIONE DI CODICE ATTUALMENTE COMMENTATA UTILIZZA LA rand_r(reentrant), per verificare la correttezza dei
    RISULTATI E CONFRONTARLI CON LE ALTRE DUE VERSIONI DEL PROGRAMMA UTILIZZARE IL CODICE PRECEDUTO DA
    if (tid == 0), ATTUALMENTE SCOMMENTATO*/
    /*for (long i = local_first_cluster; i <= local_last_cluster; i++) {
        if (tid >= K) break;
        int random = rand_r(&seed) % M;
        //printf("random = %d\n", random);
        centroids[i].centroid.cluster = random;
        centroids[i].centroid.dimensions = N;
        for (int coord = 0; coord < N; coord++)
            centroids[i].centroid.coordinates[coord] = points[random].coordinates[coord];
    }*/
    if (tid == 0) {
        for (int i = 0; i < K; i++) {
        centroids[i].centroid.cluster = i;
        centroids[i].centroid.dimensions = N;
        for (int coord = 0; coord < N; coord++)
            centroids[i].centroid.coordinates[coord] = points[i].coordinates[coord];
    }
    }
    /*#######----BARRIER----########*/
    barrier();


    while(1) {

        for (long i = local_first_point ; i <= local_last_point; i++) {
            /* For each point calculates the distance between
             * the point and the first centroid */
            int index_of_nearest = 0;
            double distance_from_nearest = euclidean(points[i], centroids[0].centroid, N);
            /* Then calculates the distance from every other centroid and checks if 
             * ... the newly calculated distance is lower than the previous one 		*/
            for (int j = 1; j < K; j++) {
                double dist = euclidean(points[i], centroids[j].centroid, N);
                if(dist < distance_from_nearest) {
                    distance_from_nearest = dist;
                    index_of_nearest = j;
                }
            }
            /* If the cluster's index per point has not been modified 
             * ... the algorithm has converged 			 */
            if (points[i].cluster != index_of_nearest  && converged) { //convergence control
                pthread_mutex_lock(&convergence_mutex);
                converged = 0;
                pthread_mutex_unlock(&convergence_mutex);
            }
            points[i].cluster = index_of_nearest;
            points[i].distance_from_centroid = distance_from_nearest;
        }                                                            
      	/* Wait all threads. If some thread goes ahead it can mess up the data in point array */
    /*#######----BARRIER----########*/
        barrier();

        if (converged) break;

    /* If not converged, recompute the centroids*/
        recompute_centroids(centroids, points, N, K, local_first_point, local_last_point, 
        local_first_cluster, local_last_cluster, local_first_n, local_last_n, tid, M, local_first_nn, local_last_nn);
        if (tid == 0)
          	converged = 1;

        barrier();   
    }
    return 0;
}


/* Takes in input the array of centroids, the array of points, the number of points, centroids and
 * dimension of the util space	*/
void recompute_centroids(struct centroid* centroids, struct point* points,
 long N, long K, long my_first, long my_last, long first_cl, long last_cl, long first_n, long last_n, long tid, long M, long first_nn, long last_nn) {

    if (tid < K) //questo if esclude race-condition nel caso in cui thread_count > K
        for (int i = first_cl; i <= last_cl; ++i)
            centroids[i].points_in_cluster = 0;
    

    for (long i = first_cl; i <= last_cl; i++) 
        for (int coord = first_n; coord <= last_n; coord++) 
            centroids[i].accumulator.coordinates[coord] = 0.0;
    
    
    /*#######----BARRIER----########*/
    barrier();

    if(tid == 0) 
        for (long i = 0; i < M; i++) 
            centroids[points[i].cluster].points_in_cluster++;
    
    for (long i = 0; i < M; i++)
        for(long coord = first_nn; coord <= last_nn; coord++) 
            centroids[points[i].cluster].accumulator.coordinates[coord] += points[i].coordinates[coord];



    /*#######----BARRIER----########*/
    barrier();
 
    /* Compute the mean point(i.e. the new centroid) by dividing the accumulated coordinates  
    * by the number of the points in the cluster */
   /*#########----QUI C'È POSSIBILITÀ DI UNA BELLA PARALLEL, DA STUDIARCI-------#################*/
 
    for (long i = first_cl; i <= last_cl; i++) 
        for (int coord = first_n; coord <= last_n; coord++) 
            centroids[i].centroid.coordinates[coord] = 
            centroids[i].accumulator.coordinates[coord] / centroids[i].points_in_cluster;
}