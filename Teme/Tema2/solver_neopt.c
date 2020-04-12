/*
 * tema 2 ASC
 * 2020 Spring
 */
#include "utils.h"

/*
 * Add your unoptimized implementation here
 */
void allocate_matrices(int N, double **C, double **A_t, double **BA_t,
	double **AA, double **AAB)
{
	*C = calloc(N * N, sizeof(**C));
	DIE(NULL == *C, "calloc C");

	*A_t = calloc(N * N, sizeof(**A_t));
	DIE(NULL == *A_t, "calloc A_t");

	*BA_t = calloc(N * N, sizeof(**BA_t));
	DIE(NULL == *BA_t, "calloc BA_t");

	*AA = calloc(N * N, sizeof(**AA));
	DIE(NULL == *AA, "calloc AA");

	*AAB = calloc(N * N, sizeof(**AAB));
	DIE(NULL == *AAB, "calloc AAB");
}

double *my_solver(int N, double *A, double* B) {
	double *C;
	double *A_t;
	double *BA_t;
	double *AA;
	double *AAB;
	int i, j, k;

	allocate_matrices(N, &C, &A_t, &BA_t, &AA, &AAB);

	/* Se transpune matricea A: A_t = A^t */
	for (i = 0; i < N; i++)
		for (j = i; j < N; j++)
			A_t[j * N + i] = A[i * N + j];

	/* BA_t = B * A_t */
	for (i = 0; i < N; i++)
		for (j = 0; j < N; j++)
			for (k = j; k < N; k++)
				BA_t[i * N + j] += B[i * N + k]
					* A_t[k * N + j];

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

	for (i = 0; i < N; i++)
		for (j = 0; j < N; j++)
			C[i * N + j] = BA_t[i * N + j] + AAB[i * N + j];

	/* Se elibereaza memoria auxiliara folosita */
	free(A_t);
	free(BA_t);
	free(AA);
	free(AAB);

	return C;
}
