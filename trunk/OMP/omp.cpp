#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
   
int main (int argc, char *argv[])
{
	int th_id, nthreads;
	const int N=30;

	double a[N][N],b[N][N],c[N][N];
	double duration;

	for(int i=0;i<N;i++)
		for(int j=0;j<N;j++)a[i][j]=N*i+j;

	for(int i=0;i<N;i++)
		for(int j=0;j<N;j++)b[i][j]=N*i+j;

	#ifndef WIN32
	struct timeval tim1,tim2;   
    gettimeofday(&tim1, NULL);
	#endif
	nthreads = omp_get_num_threads();
	printf("There are %d threads\n",nthreads);
	#pragma omp parallel private(th_id)
	{
		th_id = omp_get_thread_num();
		printf("Hello World from thread %d\n", th_id);
		
		#pragma omp barrier

		if(th_id == 0){
			nthreads = omp_get_num_threads();
			printf("There are %d threads\n",nthreads);
		}
	}
	return EXIT_SUCCESS;
}


