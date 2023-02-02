#include "utils_pthread.h"

long thread_count;
int counter;


pthread_mutex_t convergence_mutex;
pthread_mutex_t accumulator_mutex;
pthread_mutex_t barrier_mutex;
pthread_cond_t  cond_var;
struct thread_args* t_args;
int barrier_counter = 0;
unsigned int seed;

void init_pthread_structures() {
    pthread_mutex_init(&convergence_mutex, NULL);
    pthread_mutex_init(&accumulator_mutex, NULL);
    pthread_mutex_init(&barrier_mutex, NULL);
    pthread_cond_init(&cond_var, NULL);
    seed = 1998;
}


void destroy_pthread_structures(long t_count, struct thread_args* t_args, pthread_t* thread_handles) {
    pthread_mutex_destroy(&convergence_mutex);
    pthread_mutex_destroy(&accumulator_mutex);
    pthread_mutex_destroy(&barrier_mutex);
    pthread_cond_destroy(&cond_var);
    free(thread_handles);
    free(t_args);

}

inline void barrier() {
    pthread_mutex_lock(&barrier_mutex);
    barrier_counter++;
    if (barrier_counter == thread_count) {
        barrier_counter = 0;
        pthread_cond_broadcast(&cond_var);
    } else {
        while(pthread_cond_wait(&cond_var, &barrier_mutex) != 0);
    }
    pthread_mutex_unlock(&barrier_mutex);
}

void compute_indexes(long local_points, long M, long K, long N, long thread_count,long tid, long remainder, 
long* my_first_point, long* my_last_point, long* my_first_cluster, long* my_last_cluster,
long* my_first_n, long* my_last_n, long* my_first_nn, long* my_last_nn) {
    long first_p;
    long last_p;
    long first_c;
    long last_c;
    long first_n;
    long last_n;
    long first_nn;
    long last_nn;

    if (tid < remainder) {
        first_p = (tid) * (M / thread_count + 1);
        last_p = first_p + local_points -1;
    } else {
        first_p = (remainder) * (M / thread_count + 1) + (tid - remainder) * (M / thread_count);
        last_p  = first_p + local_points -1;
    }
    *my_first_point = first_p;
    *my_last_point  = last_p;

    if (tid < N%thread_count) {
        first_nn = (tid) * (N / thread_count + 1);
        last_nn = first_nn + N/thread_count;
    } else {
        first_nn = (N%thread_count) * (N / thread_count + 1) + (tid - N%thread_count) * (N / thread_count);
        last_nn  = first_nn + N/thread_count -1;
    }
    *my_first_nn = first_nn;
    *my_last_nn  = last_nn;


    long local_clusters =  K / thread_count;
    long Kremainder = K % thread_count;
    if (Kremainder && tid < Kremainder) local_clusters++;
    if (K < thread_count) {
        local_clusters  = 1;
        first_c = (tid % K);
        last_c =  first_c;
        int threads_in_my_cluster;
        if (first_c  < thread_count % K) 
            threads_in_my_cluster = (thread_count + K) / K; //CORREGERE
        else 
            threads_in_my_cluster = (thread_count + K) / K - 1;
        long local_coords = N / threads_in_my_cluster;
        long remainderN = N % threads_in_my_cluster;
        if (tid/K < remainderN)
            local_coords ++;

        if (tid/K < remainderN) {
            first_n = (tid/K) * (N / threads_in_my_cluster + 1);
            last_n  = first_n + local_coords - 1;
        } else {
            first_n = (remainderN) * (N / threads_in_my_cluster + 1) + (tid/K - remainderN) * (N / threads_in_my_cluster);
            last_n  = first_n + local_coords - 1;
        }
    } else {
        if (tid < Kremainder) {
            first_c = (tid) * (K / thread_count + 1);
            last_c = first_c + local_clusters -1;
            //first_n = 0;
            //last_n = N;
        } else {
            first_c = (Kremainder) * (K / thread_count + 1) + (tid - Kremainder) * (K / thread_count);
            last_c  = first_c + local_clusters -1;
            //first_n = 0;
            //last_n = N;
        }
        first_n = 0;
        last_n = N -1;
    }
    *my_first_cluster = first_c;
    *my_last_cluster  = last_c;
    *my_first_n = first_n;
    *my_last_n  = last_n;
}