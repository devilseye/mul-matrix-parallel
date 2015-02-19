#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#ifndef _WIN32
#include <sys/sysinfo.h>
#include <unistd.h>
#endif // _WIN32

#define NUM_THREADS     5

pthread_cond_t cv;
pthread_mutex_t mtx;
bool thread4done = false;


void *task_code(void *argument)
{
   int tid;
 
   tid = *((int *) argument);
   
   pthread_mutex_lock(&mtx);
   
   // let thread 4 executes first
   while (tid != 4 && !thread4done) pthread_cond_wait(&cv, &mtx);

   printf("Hello World! It's me, thread %d!\n", tid);

   if (tid == 4) { thread4done = true; pthread_cond_broadcast(&cv); /*pthread_cond_signal(&cv)*/}
 
   pthread_mutex_unlock(&mtx);

   /* optionally: insert more useful stuff here */
 
   return NULL;
}
 
int main(void)
{
   pthread_t threads[NUM_THREADS];
   int thread_args[NUM_THREADS];
   int rc, i;
 
   pthread_mutex_init(&mtx, NULL);
   pthread_cond_init(&cv, NULL);

   printf("Hello from POSIX threads app!\n");

   // getting processors configuration
#ifndef _WIN32
   printf(" sysconf_configured=%ld\n", sysconf(_SC_NPROCESSORS_CONF));
   printf(" sysconf_online=%ld\n", sysconf(_SC_NPROCESSORS_ONLN));
   printf(" get_nprocs_conf=%d\n", get_nprocs_conf());
   printf(" get_nprocs=%d\n", get_nprocs());
#endif // _WIN32

   // create all threads one by one
   for (i=0; i<NUM_THREADS; ++i) {
      thread_args[i] = i;
      printf("In main: creating thread %d\n", i);
      rc = pthread_create(&threads[i], NULL, task_code, (void *) &thread_args[i]);
      assert(0 == rc);
   }
 
   // wait for each thread to complete
   for (i=0; i<NUM_THREADS; ++i) {
      // block until thread i completes
      rc = pthread_join(threads[i], NULL);
      printf("In main: thread %d is complete\n", i);
      assert(0 == rc);
   }
 
   printf("In main: All threads completed successfully\n");

   pthread_mutex_destroy(&mtx);
   pthread_cond_destroy(&cv);

   return EXIT_SUCCESS;
}
