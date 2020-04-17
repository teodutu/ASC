#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void fill_array_int(int *a, int N) {
    for (int i = 0; i < N; ++i) {
        a[i] = i;
    }
}

void fill_array_float(float *a, int N) {
    for (int i = 0; i < N; ++i) {
        a[i] = (float) i;
    }
}

void fill_array_random(float *a, int N) {
    for (int i = 0; i < N; ++i) {
        a[i] = (float) rand() / RAND_MAX;
    }
}
