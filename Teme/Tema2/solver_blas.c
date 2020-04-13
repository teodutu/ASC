/*
 * Tema 2 ASC
 * 2020 Spring
 */
#include <string.h>

#include "utils.h"
#include "cblas.h"

/* 
 * Add your BLAS implementation here
 */
void allocate_matrices(int N, double **C, double **AA)
{
	*C = calloc(N * N, sizeof(**C));
	DIE(NULL == *C, "calloc C");

	*AA = calloc(N * N, sizeof(**AA));
	DIE(NULL == *AA, "calloc AA");
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

	for (i = 0; i < N; i++)
		for (j = 0; j < N; j++)
			C[i * N + j] += B[i * N + j];

	/* Se elibereaza memoria auxiliara folosita */
	free(AA);

	return C;
}

