/*
 * tema 2 ASC
 * 2020 Spring
 */
#include <stdlib.h>

/** 
 * Functia aloca matricele C, BA_t = B * A', AA = A^2 si AAB = A^2 * B,
 * verificand daca au avut loc erori.
 */
void allocate_matrices(int N, double **C, double **BA_t, double **AA,
	double **AAB)
{
	*C = malloc(N * N * sizeof(**C));
	if (NULL == *C)
		exit(EXIT_FAILURE);

	*BA_t = calloc(N * N, sizeof(**BA_t));
	if (NULL == *BA_t)
		exit(EXIT_FAILURE);

	*AA = calloc(N * N, sizeof(**AA));
	if (NULL == *AA)
		exit(EXIT_FAILURE);

	*AAB = calloc(N * N, sizeof(**AAB));
	if (NULL == *AAB)
		exit(EXIT_FAILURE);
}

double *my_solver(int N, double *A, double* B)
{
	double *C;
	double *BA_t;
	double *AA;
	double *AAB;
	int i, j, k;

	allocate_matrices(N, &C, &BA_t, &AA, &AAB);

	/* BA_t = B * A' */
	for (i = 0; i < N; i++)
		for (j = 0; j < N; j++)
			for (k = j; k < N; k++)
				BA_t[i * N + j] += B[i * N + k]
					* A[j * N + k];

	/* AA = A^2 */
	for (i = 0; i < N; i++)
		for (j = i; j < N; j++)
			for (k = i; k <= j; k++)
				AA[i * N + j] += A[i * N + k]
					* A[k * N + j];

	/* AAB = A^2 * B = AA * B */
	for (i = 0; i < N; i++)
		for (j = 0; j < N; j++)
			for (k = i; k < N; k++)
				AAB[i * N + j] += AA[i * N + k]
					* B[k * N + j];

	/* C = B * A' + A^2 * B = BA_t + AAB */
	for (i = 0; i < N; i++)
		for (j = 0; j < N; j++)
			C[i * N + j] = BA_t[i * N + j] + AAB[i * N + j];

	/* Se elibereaza memoria auxiliara folosita */
	free(BA_t);
	free(AA);
	free(AAB);

	return C;
}
