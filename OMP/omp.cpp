#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#ifndef _WIN32
#include <sys/time.h>
#endif // _WIN32
   
int main (int argc, char *argv[])
{
	double **a, **b,**c;
	int sizes[]={30,60,100,1000,3000};
	for (int index=0;index<(sizeof(sizes)/sizeof(int));index++)
	{
		int th_id, nthreads;
		int N=sizes[index];
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
			nthreads = omp_get_num_threads();
			th_id = omp_get_thread_num();
			int endFor;
			if (th_id+1==nthreads)
			{
				endFor=N;
			}
			else
			{
				endFor=(th_id+1)*(N/nthreads);
			}

			for (int i=th_id*(N/nthreads);i<endFor;i++)
			{
				//printf("Processed string: %d\n",i);
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

		printf("All threads completed successfully!\nDuration: %10.5lf sec.\n",duration);

		// cleaning matrix Ñ
		for(int i=0;i<N;i++)
		for(int j=0;j<N;j++) c[i][j]=0.0;
	
		#ifndef WIN32 
		gettimeofday(&tim1, NULL);
		#endif
		// serial matrix multiplication
		for(int i=0;i<N;i++){
			for(int j=0;j<N;j++){
				c[i][j]=0.0;
				for(int k=0;k<N;k++)c[i][j]+=a[i][k]*b[k][j];
			}
		}
		#ifndef WIN32	
		gettimeofday(&tim2, NULL);
		duration=tim2.tv_sec+(tim2.tv_usec/1000000.0)-tim1.tv_sec+(tim1.tv_usec/1000000.0); 
		#endif
		printf("Serial Multiplication duration: %10.5lf sec.\n",duration);
		for (int count = 0; count < N; count++)
			delete []a[count];
		for (int count = 0; count < N; count++)
			delete []b[count];
		for (int count = 0; count < N; count++)
			delete []c[count];
	}
	return EXIT_SUCCESS;
}


