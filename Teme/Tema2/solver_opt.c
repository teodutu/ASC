/*
 * Tema 2 ASC
 * 2020 Spring
 */
#include <stdlib.h>

/*
 * Add your optimized implementation here
 */
void allocate_matrices(int N, double **C, double **A_t, double **B_t,
	double **AA)
{
	register int size = N * N * sizeof(**C);

	*C = malloc(size);
	if (NULL == *C)
		exit(EXIT_FAILURE);

	*A_t = malloc(size);
	if (NULL == *A_t)
		exit(EXIT_FAILURE);

	*B_t = malloc(size);
	if (NULL == *B_t)
		exit(EXIT_FAILURE);

	*AA = malloc(size);
	if (NULL == *AA)
		exit(EXIT_FAILURE);
}

double* my_solver(int N, double *A, double* B)
{
	double *C;
	double *A_t;
	double *B_t;
	double *AA;

	allocate_matrices(N, &C, &A_t, &B_t, &AA);

	/* Se transpun matricele A: A_t = A^t si B: B_t = B^t */
	for (register int i = 0; i != N; ++i) {
		register double *A_t_ptr = A_t + i;
		register double *B_t_ptr = B_t + i;

		register double *A_ptr = A + i * N;
		register double *B_ptr = B + i * N;

		for (register int j = 0; j != N; ++j, A_t_ptr += N, B_t_ptr += N,
			++A_ptr, ++B_ptr) {
			*A_t_ptr = *A_ptr;
			*B_t_ptr = *B_ptr;
		}
	}

	/*
	 * Initial C = B * A^t
	 * A_t = A parcurs pe linii
	 */
	for (register int i = 0; i != N; ++i) {
		register double *C_ptr = C + i * N;
		register double *B_copy = B + i * N;

		for (register int j = 0; j != N; ++j, ++C_ptr) {
			register double result = 0;
			register double *B_ptr = B_copy + j;
			register double *A_ptr = A + j * (N + 1);

			for (register int k = j; k < N; ++k, ++B_ptr, ++A_ptr)
				result += *B_ptr * *A_ptr;

			*C_ptr = result;
		}
	}

	/*
	 * AA = A^2
	 * Se va folosi A_t parcursa pe linii
	 */
	for (register int i = 0; i != N; ++i) {
		register double *AA_ptr = AA + i * (N + 1);
		register double *A_copy = A + i * N;

		for (register int j = i; j != N; ++j, ++AA_ptr) {
			register double result = 0;
			register double *A_ptr = A_copy + i;
			register double *A_t_ptr = A_t + j * N + i;

			for (register int k = i; k <= j; ++k, ++A_ptr,
				++A_t_ptr)
				result += *A_ptr * *A_t_ptr;

			*AA_ptr = result;
		}
	}

	/*
	 * C += A^2 * B <=> C += AA * B
	 * Se foloseste B_t parcurs pe linii
	 */
	for (register int i = 0; i != N; ++i) {
		register double *C_ptr = C + i * N;
		register double *AA_copy = AA + i * N;

		for (register int j = 0; j != N; ++j, ++C_ptr) {
			register double result = 0;
			register double *AA_ptr = AA_copy + i;
			register double *B_t_ptr = B_t + j * N + i;

			for (register int k = i; k != N; ++k, ++AA_ptr,
				++B_t_ptr)
				result += *AA_ptr * *B_t_ptr;

			*C_ptr += result;
		}
	}

	/* Se elibereaza memoria auxiliara folosita */
	free(A_t);
	free(B_t);
	free(AA);

	return C;
}
