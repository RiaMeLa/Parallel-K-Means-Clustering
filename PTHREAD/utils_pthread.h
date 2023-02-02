#ifndef UTILS_PTHREAD_H
#define UTILS_PTHREAD_H

#include "utils.h"
#include <pthread.h>

extern long thread_count;


extern pthread_mutex_t convergence_mutex;
extern pthread_mutex_t accumulator_mutex;
extern pthread_mutex_t barrier_mutex;
extern pthread_cond_t  cond_var;
extern int barrier_counter;
extern unsigned int seed;



struct thread_args {
    long tid;
    long thread_count;
    struct space* sp;
};

void barrier();
void init_pthread_structures();
void destroy_pthread_structures(long t_count, struct thread_args* t_args, pthread_t* thread_handles);
void compute_indexes(long local_points, long M, long K, long N, long thread_count,long tid, long remainder, 
long* my_first_point, long* my_last_point, long* my_first_cluster, long* my_last_cluster,
long* my_first_n, long* my_last_n, long* my_first_nn, long* my_last_nn);
#endif