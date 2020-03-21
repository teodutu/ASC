#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/time.h>

#define N		2000

double A[N][N];
double B[N][N];
double C[N][N];

int main(void)
{
	double *initialAPtr, *aPtr, *bPtr;
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
		initialAPtr = A[i];

		for (j = 0; j != N; ++j)
		{
			aPtr = initialAPtr;
			bPtr = &B[0][j];

			register double sum = 0.0;

			for (k = 0; k != N; ++k, ++aPtr, bPtr += N)
			{
				sum += *aPtr * *bPtr;
			}

			C[i][j] = sum;
		}
	}

	gettimeofday(&end, NULL);

	float elapsed = ((end.tv_sec - start.tv_sec) * 1000000.0f
		+ end.tv_usec - start.tv_usec) / 1000000.0f;
	printf("Time for N = %d is %f seconds.\n", N, elapsed);

	return 0;
}