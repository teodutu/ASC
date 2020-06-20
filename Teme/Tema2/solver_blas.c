/*
 * Tema 2 ASC
 * 2020 Spring
 */
#include <string.h>
#include <stdlib.h>

#include "cblas.h"

/** 
 * Functia aloca matricele C si AA = A^2, verificand daca au avut loc erori.
 */
void allocate_matrices(int N, double **C, double **AA)
{
	*C = malloc(N * N * sizeof(**C));
	if (NULL == *C)
		exit(EXIT_FAILURE);

	*AA = malloc(N * N * sizeof(**AA));
	if (NULL == *AA)
		exit(EXIT_FAILURE);
}

double *my_solver(int N, double *A, double* B)
{
	double *C;
	double *AA;
	int i, j;

	allocate_matrices(N, &C, &AA);

	/* C = B * A_t */
	memcpy(C, B, N * N * sizeof(*C));

	cblas_dtrmm(
		CblasRowMajor,
		CblasRight,
		CblasUpper,
		CblasTrans,
		CblasNonUnit,
		N, N,
		1.0, A, N,
		C, N
	);

	/* AA = A * A */
	memcpy(AA, A, N * N * sizeof(*C));

	cblas_dtrmm(
		CblasRowMajor,
		CblasLeft,
		CblasUpper,
		CblasNoTrans,
		CblasNonUnit,
		N, N,
		1.0, A, N,
		AA, N
	);

	/* B = AA * B = A^2 * B */
	cblas_dtrmm(
		CblasRowMajor,
		CblasLeft,
		CblasUpper,
		CblasNoTrans,
		CblasNonUnit,
		N, N,
		1.0, AA, N,
		B, N
	);

	/* C += A^2 * B <=> C += AAB */
	for (i = 0; i < N; i++)
		for (j = 0; j < N; j++)
			C[i * N + j] += B[i * N + j];

	/* Se elibereaza memoria auxiliara folosita */
	free(AA);

	return C;
}

