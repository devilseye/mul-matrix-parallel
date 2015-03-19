#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


#ifndef _WIN32
#include <sys/time.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#endif // _WIN32


#define NUM_THREADS     30
int numProcessors;
int N;
double **a, **b,**c;
pthread_cond_t cv;
pthread_mutex_t mtx;

void *task_code(void *argument)
{
   int tid;
 
   tid = *((int *) argument);
   pthread_mutex_lock(&mtx);
   
   int endFor;
   
   if (tid+1==numProcessors)
   {
	   endFor=N;
   }
   else
   {
	   endFor=(tid+1)*N/(numProcessors);
   }

   for (int i=tid*(N/numProcessors);i<endFor;i++)
   {
	   //printf("Processed string: %d\n",i);
	   for(int j=0;j<N;j++)
	   {
		   c[i][j]=0.0;
		   for(int k=0;k<N;k++)
			   c[i][j]+=a[i][k]*b[k][j];
	   }
   } 

   pthread_mutex_unlock(&mtx);
 
   return NULL;
}
 


int main(void)
{
	int sizes[]={30,60,100,1000,3000};
	for (int index=0;index<(sizeof(sizes)/sizeof(int));index++)
	{
		N=sizes[index];
		printf("Matrix size: %d x %d\n",N,N);
		a= new double *[N];
		for (int i = 0; i < N; i++) 
		{
		  a[i] = new double[N];
		}
		b= new double *[N];
		for (int i = 0; i < N; i++) 
		{
		  b[i] = new double[N];
		}
		c= new double *[N];
		for (int i = 0; i < N; i++) 
		{
		  c[i] = new double[N];
		}
		double duration;
		//matrix initialization
		for(int i=0;i<N;i++)
			for(int j=0;j<N;j++) a[i][j]=N*i+j;

		for(int i=0;i<N;i++)
			for(int j=0;j<N;j++) b[i][j]=N*i+j;

		pthread_t threads[NUM_THREADS];
		int thread_args[NUM_THREADS];
		int rc, i;

		pthread_mutex_init(&mtx, NULL);
		pthread_cond_init(&cv, NULL);

		#ifndef _WIN32
		numProcessors=sysconf(_SC_NPROCESSORS_ONLN);
		printf("numProcessors: %d\n",numProcessors);
		#else
		numProcessors=NUM_THREADS;
		#endif // _WIN32

		#ifndef WIN32
		struct timeval tim1,tim2;   
		gettimeofday(&tim1, NULL);
		#endif
		// create all threads one by one
		for (i=0; i<numProcessors; ++i) {
			thread_args[i] = i;
			//printf("Creating thread %d\n", i);
			rc = pthread_create(&threads[i], NULL, task_code, (void *) &thread_args[i]);
			assert(0 == rc);
		}
 
		// wait for each thread to complete
		for (i=0; i<numProcessors; ++i) {
			// block until thread i completes
			rc = pthread_join(threads[i], NULL);
			//printf("Thread %d is complete\n", i);
			assert(0 == rc);
		}
		#ifndef WIN32	
		gettimeofday(&tim2, NULL);
		duration=tim2.tv_sec+(tim2.tv_usec/1000000.0)-tim1.tv_sec+(tim1.tv_usec/1000000.0); 
		#endif
		printf("All threads completed successfully!\nDuration: %10.5lf sec.\n",duration);
		pthread_mutex_destroy(&mtx);
		pthread_cond_destroy(&cv);
		for (int count = 0; count < N; count++)
			delete []a[count];
		for (int count = 0; count < N; count++)
			delete []b[count];
		for (int count = 0; count < N; count++)
			delete []c[count];
	}
    return EXIT_SUCCESS;
}
