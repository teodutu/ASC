#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

#define N			1500
#define SECOND_MICROS		1000000.f

int main(void)
{
	double *cPtr, *bPtr, *aPtr;
	double *initialBPtr, *initialAPtr, *initialCPtr;
	int i, j, k;
	int numMatrixElems = N * N;
	struct timeval start, end;
	double* A;
	double* B;
	double* C;
	double* D;

	A = malloc(numMatrixElems * sizeof(*A));
	B = malloc(numMatrixElems * sizeof(*B));
	C = calloc(numMatrixElems, sizeof(*C));
	D = calloc(numMatrixElems, sizeof(*D));

	aPtr = A;
	bPtr = B;

	for (i = 0; i != numMatrixElems; ++i, ++aPtr, ++bPtr)
	{
		*aPtr = (double)rand() / RAND_MAX * 2.0 - 1.0;
		*bPtr = (double)rand() / RAND_MAX * 2.0 - 1.0;
	}


	gettimeofday(&start, NULL);

	initialBPtr = B;  /* linia k din B */
	initialAPtr = A;  /* coloana k din A */

	for (k = 0; k != N; ++k, initialBPtr += N, ++initialAPtr)
	{
		bPtr = initialBPtr;
		initialCPtr = C;  /* coloana j din C */

		for (j = 0; j != N; ++j, ++bPtr, ++initialCPtr)
		{
			aPtr = initialAPtr;
			cPtr = initialCPtr;

			for (i = 0; i != N; ++i, aPtr += N, cPtr += N)
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
				D[i * N + j] += A[i * N + k] * B[k * N + j];
			}

			if (fabs(D[i * N + j] - C[i * N + j]) > 0.001)
			{
				printf("Incorrect result value at positions"
					"(%d, %d): correct value is %lf; result"
					"is %lf\n", i, j, D[i * N + j],
					C[i * N + j]);
				exit(EXIT_FAILURE);
			}
		}
	}

	printf("Time for N = %d is %f seconds.\n", N, elapsed);

	free(A);
	free(B);
	free(C);
	free(D);

	return 0;
}
