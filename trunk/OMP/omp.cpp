#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#ifndef _WIN32
#include <sys/time.h>
#endif // _WIN32
   
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

	#pragma omp parallel private(th_id)
	{
		if(th_id == 0){
			nthreads = omp_get_num_threads();
			printf("There are %d threads\n",nthreads);
		}
		#pragma omp barrier
		th_id = omp_get_thread_num();
		int endFor;
		if (th_id+1==nthreads)
		{
			endFor=N;
		}
		else
		{
			endFor=(th_id+1)*N/(nthreads);
		}

		for (int i=th_id*(N/nthreads);i<endFor;i++)
		{
			printf("Processed string: %d\n",i);
			for(int j=0;j<N;j++)
			{
				c[i][j]=0.0;
				for(int k=0;k<N;k++)
					c[i][j]+=a[i][k]*b[k][j];
			}
		}
	}

#ifndef WIN32	
	gettimeofday(&tim2, NULL);
	duration=tim2.tv_sec+(tim2.tv_usec/1000000.0)-tim1.tv_sec+(tim1.tv_usec/1000000.0); 
#endif

	printf("All threads completed successfully!\nDuration: %10.5lf sec.",duration);
	return EXIT_SUCCESS;
}


