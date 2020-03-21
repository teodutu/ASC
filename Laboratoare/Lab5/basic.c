#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/time.h>

#define N			1500
#define SECOND_MICROS		1000000.f

double A[N][N];
double B[N][N];
double C[N][N];

int main(void)
{
	double *aPtr, *bPtr;
	int i, j, k;
	int numMatrixElems = N * N;
	struct timeval start, end;

	srand(42);

	aPtr = A[0];
	bPtr = B[0];

	for (i = 0; i != numMatrixElems; ++i, ++aPtr, ++bPtr)
	{
		*aPtr = (double)rand() / RAND_MAX * 2.0 - 1.0;
		*bPtr = (double)rand() / RAND_MAX * 2.0 - 1.0;
	}

	gettimeofday(&start, NULL);

	for (i = 0; i != N; ++i)
	{
		for (j = 0; j != N; ++j)
		{
			for (k = 0; k != N; ++k)
			{
				C[i][j] += A[i][k] * B[k][j];
			}
		}
	}

	gettimeofday(&end, NULL);

	float elapsed = ((end.tv_sec - start.tv_sec) * SECOND_MICROS
		+ end.tv_usec - start.tv_usec) / SECOND_MICROS;
	printf("Time for N = %d is %f seconds.\n", N, elapsed);

	return 0;
}
