#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


#ifndef _WIN32
#include <sys/sysinfo.h>
#include <unistd.h>
#endif // _WIN32


#define NUM_THREADS     10

int numProcessors;

const int N=3;
double a[N][N],b[N][N],c[N][N];

pthread_cond_t cv;
pthread_mutex_t mtx;


void *task_code(void *argument)
{
   int tid;
 
   tid = *((int *) argument);
   
   pthread_mutex_lock(&mtx);
   
   int endFor;
   
   for (int i=tid*(N/numProcessors);i<(tid+1)*N/(numProcessors);i++)
   {
	   printf("%d\n",i);
	   for(int j=0;j<N;j++)
	   {
		   c[i][j]=0.0;
		   for(int k=0;k<N;k++)
			   c[i][j]+=a[i][k]*b[k][j];
	   }
   }
   printf("\n");   

   pthread_mutex_unlock(&mtx);
 
   return NULL;
}
 


int main(void)
{
   //matrix initialization
	for(int i=0;i<N;i++)
		for(int j=0;j<N;j++)a[i][j]=N*i+j;

	for(int i=0;i<N;i++)
		for(int j=0;j<N;j++)b[i][j]=N*i+j;

	pthread_t threads[NUM_THREADS];
	int thread_args[NUM_THREADS];
	int rc, i;

   pthread_mutex_init(&mtx, NULL);
   pthread_cond_init(&cv, NULL);

   printf("Hello from POSIX Matrix Multiplication Application!\n");

   // getting processors configuration

#ifndef _WIN32
   numProcessors=sysconf(_SC_NPROCESSORS_ONLN);
   printf("numProcessors: %d\n",numProcessors);
/*
   printf(" sysconf_configured=%ld\n", sysconf(_SC_NPROCESSORS_CONF));
   printf(" sysconf_online=%ld\n", sysconf(_SC_NPROCESSORS_ONLN));
   printf(" get_nprocs_conf=%d\n", get_nprocs_conf());
   printf(" get_nprocs=%d\n", get_nprocs());
*/
#endif // _WIN32

   // create all threads one by one
   for (i=0; i<numProcessors; ++i) {
      thread_args[i] = i;
      printf("Creating thread %d\n", i);
      rc = pthread_create(&threads[i], NULL, task_code, (void *) &thread_args[i]);
      assert(0 == rc);
   }
 
   // wait for each thread to complete
   for (i=0; i<numProcessors; ++i) {
      // block until thread i completes
      rc = pthread_join(threads[i], NULL);
      printf("Thread %d is complete\n", i);
      assert(0 == rc);
   }
 
   printf("All threads completed successfully\n");
   printf("\nC=\n");
	for(int i=0;i<N;i++){
		for(int j=0;j<N;j++){
			printf("%10.2lf  ",c[i][j]);
		}
		printf("\n");
	}
   pthread_mutex_destroy(&mtx);
   pthread_cond_destroy(&cv);

   return EXIT_SUCCESS;
}
