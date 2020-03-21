#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

#define N			1500
#define SECOND_MICROS		1000000.f

double A[N][N];
double B[N][N];
double C[N][N];
double D[N][N];

int main(void)
{
	double *initialBPtr, *cPtr, *bPtr, *aPtr;
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

	for (k = 0; k != N; ++k)
	{
		initialBPtr = B[k];  /* linia k din B */
		aPtr = &A[0][k];  /* coloana k din A */

		for (i = 0; i != N; ++i, aPtr += N)
		{
			cPtr = C[i];  /* linia i din C */
			bPtr = initialBPtr;

			for (j = 0; j != N; ++j, ++bPtr, ++cPtr)
			{
				*cPtr += *aPtr * *bPtr;
			}
		}
	}

	gettimeofday(&end, NULL);

	float elapsed = ((end.tv_sec - start.tv_sec) * SECOND_MICROS
		+ end.tv_usec - start.tv_usec) / SECOND_MICROS;

	/* Verificarea corectitudinii */
	for (i = 0; i != N; ++i)
	{
		for (j = 0; j != N; ++j)
		{
			for (k = 0; k != N; ++k)
			{
				D[i][j] += A[i][k] * B[k][j];
			}

			if (fabs(D[i][j] - C[i][j]) > 0.001)
			{
				printf("Incorrect result value at positions (%d, %d): correct value is %lf; result is %lf\n",
					i, j, D[i][j], C[i][j]);
				exit(EXIT_FAILURE);
			}
		}
	}

	printf("Time for N = %d is %f seconds.\n", N, elapsed);

	return 0;
}