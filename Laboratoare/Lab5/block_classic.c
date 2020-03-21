#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

void BMMultiply(int n, int blockSize, double** a, double** b, double** c)
{
	int bi, bj, bk;
	int i, j, k;
 
	for(bi = 0; bi < n; bi += blockSize)
	{
		for(bj = 0; bj < n; bj += blockSize)
		{
			for(bk = 0; bk < n; bk += blockSize)
			{
				for(i = 0; i != blockSize; ++i)
				{
					for(j = 0; j != blockSize; ++j)
					{
						for(k = 0; k != blockSize; ++k)
						{
							c[bi + i][bj + j] += a[bi + i][bk + k] *
									b[bk + k][bj + j];
						}
					}
				}
			}
		}
	}
}

int main(int argc, char** argv)
{
	int n;
	double** A;
	double** B;
	double** C;
	int numreps;
	int blockSize;
	int i, j;
	struct timeval tv1, tv2;
	struct timezone tz;
	double elapsed;

	// TODO: set matrix dimension n
	if (argc != 4)
	{
		printf("Usage: %s <N> <BLOCK_SIZE> <NUM_REPS>\n", argv[0]);
		return -1;
	}

	n = atoi(argv[1]);
	blockSize = atoi(argv[2]);
	numreps = atoi(argv[3]);

	// TODO: allocate matrices A, B & C
	A = malloc(n * sizeof(*A));
	B = malloc(n * sizeof(*B));
	C = malloc(n * sizeof(*C));

	for (i = 0; i != n; ++i)
	{
		A[i] = malloc(n * sizeof(**A));
		B[i] = malloc(n * sizeof(**B));
		C[i] = calloc(n, sizeof(**C));

		for (j = 0; j != n; ++j)
		{
			A[i][j] = 1.0;
			B[i][j] = 2.0;
		}
	}

	//multiply matrices

	printf("Multiply matrices %d times...\n", numreps);
	for (i = 0; i != numreps; ++i)
	{
		gettimeofday(&tv1, &tz);
		BMMultiply(n, blockSize, A, B, C);
		gettimeofday(&tv2, &tz);

		elapsed += (double)(tv2.tv_sec - tv1.tv_sec) + (double) (tv2.tv_usec - tv1.tv_usec) * 1.e-6;
	}

	printf("Time = %lf seconds\n", elapsed);

	//deallocate memory for matrices A, B & C
	for (i = 0; i != n; ++i)
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
