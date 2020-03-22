#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define N			1500
#define BLOCK_SIZE		50
#define NUM_REPS		10

void BMMultiply(double** a, double** b, double** c)
{
	int bi, bj, bk;
	int i, j, k;
 	double *cPtr, *aPtr, *bPtr;
 
	for(bi = 0; bi < N; bi += BLOCK_SIZE)
	{
		for(bj = 0; bj < N; bj += BLOCK_SIZE)
		{
			for(bk = 0; bk < N; bk += BLOCK_SIZE)
			{
				for(i = 0; i != BLOCK_SIZE; ++i)
				{
					aPtr = a[bi + i] + bk;
					cPtr = c[bi + i] + bj;

					for(j = 0; j != BLOCK_SIZE; ++j, ++cPtr)
					{
						register double sum;

						for(k = 0; k != BLOCK_SIZE; ++k, ++aPtr, bPtr += N)
						{
							sum += *aPtr * b[bj + j][bk + k];
						}

						*cPtr = sum;
					}
				}
			}
		}
	}
}

int main(void)
{
	int i, j;
	struct timeval tv1, tv2;
	struct timezone tz;
	double elapsed;
	double** A;
	double** B;
	double** C;

	A = malloc(N * sizeof(*A));
	B = malloc(N * sizeof(*B));
	C = calloc(N, sizeof(*C));

	for (i = 0; i != N; ++i)
	{
		A[i] = malloc(N * sizeof(**A));
		B[i] = malloc(N * sizeof(**B));
		C[i] = calloc(N, sizeof(**C));

		for (j = 0; j != N; ++j)
		{
			A[i][j] = 1.0;
			B[i][j] = 2.0;
		}
	}

	//multiply matrices

	printf("Multiply matrices %d times...\n", NUM_REPS);
	for (i = 0; i != NUM_REPS; ++i)
	{
		gettimeofday(&tv1, &tz);
		BMMultiply(A, B, C);
		gettimeofday(&tv2, &tz);

		elapsed += (double)(tv2.tv_sec - tv1.tv_sec) + (double) (tv2.tv_usec - tv1.tv_usec) * 1.e-6;
	}

	printf("Average time for N = %d, BLOCK_SIZE = %d is %lf seconds\n",
		N, NUM_REPS, elapsed / NUM_REPS);

	for (i = 0; i != N; ++i)
	{
		free(A[i]);
		free(B[i]);
		free(C[i]);
	}

	free(A);
	free(B);
	free(C);

	return 0;
}
