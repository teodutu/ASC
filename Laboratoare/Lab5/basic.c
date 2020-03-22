#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/time.h>

#define N			1500
#define SECOND_MICROS		1000000.f

int main(void)
{
	int i, j, k;
	int numMatrixElems = N * N;
	struct timeval start, end;
	double *A;
	double *B;
	double *C;

	A = malloc(numMatrixElems * sizeof(*A));
	B = malloc(numMatrixElems * sizeof(*B));
	C = calloc(numMatrixElems, sizeof(*C));

	srand(42);

	for (i = 0; i != numMatrixElems; ++i)
	{
		A[i] = (double)rand() / RAND_MAX * 2.0 - 1.0;
		B[i] = (double)rand() / RAND_MAX * 2.0 - 1.0;
	}

	gettimeofday(&start, NULL);

	for (i = 0; i != N; ++i)
	{
		for (j = 0; j != N; ++j)
		{
			for (k = 0; k != N; ++k)
			{
				C[i * N + j] += A[i * N + j] * B[k * N + j];
			}
		}
	}

	gettimeofday(&end, NULL);

	float elapsed = ((end.tv_sec - start.tv_sec) * SECOND_MICROS
		+ end.tv_usec - start.tv_usec) / SECOND_MICROS;
	printf("Time for N = %d is %f seconds.\n", N, elapsed);

	free(A);
	free(B);
	free(C);

	return 0;
}
