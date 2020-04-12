/*
 * Tema 2 ASC
 * 2020 Spring
 */
#include "utils.h"

/*
 * Add your optimized implementation here
 */
void allocate_matrices(int N, double **C, double **A_t, double **B_t,
	double **AA)
{
	register int size = N * N * sizeof(**C);

	*C = malloc(size);
	DIE(NULL == *C, "malloc C");

	*A_t = malloc(size);
	DIE(NULL == *A_t, "malloc A_t");

	*B_t = malloc(size);
	DIE(NULL == *B_t, "malloc B_t");

	*AA = malloc(size);
	DIE(NULL == *AA, "malloc AA");
}

double* my_solver(int N, double *A, double* B)
{
	double *C;
	double *A_t;
	double *B_t;
	double *AA;
	register double *A_ptr, *A_t_ptr;
	register double *B_ptr, *B_t_ptr;

	allocate_matrices(N, &C, &A_t, &B_t, &AA);

	/* Se transpun matricele A: A_t = A^t si B: B_t = B^t */
	for (register int i = 0; i < N; ++i) {
		A_t_ptr = A + i;
		B_t_ptr = B + i;

		A_ptr = A + i * N;
		B_ptr = B + i * N;

		for (register int j = 0; j < N; ++j, A_t_ptr += N, B_ptr += N,
			++A_ptr, ++B_ptr) {
			*A_t_ptr = *A_ptr;
			*B_t_ptr = *B_ptr;
		}
	}

	/*
	 * Initial C = B * A^t
	 * A_t = A parcurs pe linii
	 */
	for (register int i = 0; i < N; ++i)
		for (register int j = 0; j < N; ++j) {
			register double result = 0;

			for (register int k = i; k < N; ++k)
				result += B[i * N + k] * A[j * N + k];

			C[i * N + j] = result;
		}

	/*
	 * AA = A^2
	 * Se va folosi A_t parcursa pe linii
	 */
	for (register int i = 0; i < N; ++i)
		for (register int j = i; j < N; ++j){
			register double result = 0;

			for (register int k = i; k <= j; ++k)
				result += A[i * N + k] * A_t[j * N + k];

			AA[i * N + j] = result;
		}

	/*
	 * C += A^2 * B <=> C += AA * B
	 * Se foloseste B_t parcurs pe linii
	 */
	for (register int i = 0; i < N; ++i)
		for (register int j = 0; j < N; ++j) {
			register double result = 0;

			for (register int k = i; k < N; ++k)
				result += AA[i * N + k] * B_t[j * N + k];

			C[i * N + j] += result;
		}

	/* Se elibereaza memoria auxiliara folosita */
	free(A_t);
	free(B_t);
	free(AA);

	return C;
}
