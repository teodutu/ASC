#include <stdio.h>
#include <math.h>
#include "utils/utils.h"

// TODO 6: Write the code to add the two arrays element by element and 
// store the result in another array
__global__ void add_arrays(const float *a, const float *b, float *c, int N)
{
	unsigned int i = threadIdx.x + blockDim.x * blockIdx.x;

	if (i < N)
	{
		c[i] = a[i] + b[i];
	}
}

int main(void)
{
	cudaSetDevice(0);
	int N = 1 << 20;
	cudaError_t err;

	const size_t block_size = 256;
	size_t num_blocks;

	float *host_array_a = 0;
	float *host_array_b = 0;
	float *host_array_c = 0;

	float *device_array_a = 0;
	float *device_array_b = 0;
	float *device_array_c = 0;

	// TODO 1: Allocate the host's arrays
	// TODO 3: Check for allocation errors
	host_array_a = (float*)malloc(N * sizeof(*host_array_a));
	DIE(host_array_a == NULL, "malloc(host_array_a)");

	host_array_b = (float*)malloc(N * sizeof(*host_array_b));
	DIE(host_array_b == NULL, "malloc(host_array_b)");

	host_array_c = (float*)malloc(N * sizeof(*host_array_c));
	DIE(host_array_c == NULL, "malloc(host_array_c)");

	// TODO 2: Allocate the device's arrays
	err = cudaMalloc(&device_array_a, N * sizeof(*device_array_a));
	DIE(err != cudaSuccess || device_array_a == NULL,
		"cudaMalloc(device_array_a)");

	err = cudaMalloc(&device_array_b, N * sizeof(*device_array_b));
	DIE(err != cudaSuccess || device_array_b == NULL,
		"cudaMalloc(device_array_b)");

	err = cudaMalloc(&device_array_c, N * sizeof(*device_array_c));
	DIE(err != cudaSuccess || device_array_c == NULL,
		"cudaMalloc(device_array_c)");

	// TODO 4: Fill array with values; use fill_array_float to fill
	// host_array_a and fill_array_random to fill host_array_b. Each
	// function has the signature (float *a, int n), where n = number of elements.
	fill_array_float(host_array_a, N);
	fill_array_random(host_array_b, N);

	// TODO 5: Copy the host's arrays to device
	err = cudaMemcpy(device_array_a, host_array_a,
		N * sizeof(*host_array_a), cudaMemcpyHostToDevice);
	DIE(err != cudaSuccess, "cudaMemcpy(host_array_a)");

	err = cudaMemcpy(device_array_b, host_array_b,
		N * sizeof(*host_array_b), cudaMemcpyHostToDevice);
	DIE(err != cudaSuccess, "cudaMemcpy(host_array_b)");

	// TODO 6: Execute the kernel, calculating first the grid size
	// and the amount of threads in each block from the grid
	// Hint: For this execise the block_size can have any value lower than the
	//      API's maximum value (it's recommended to be close to the maximum
	//      value).
	num_blocks = N / block_size;

	if (N % block_size)
	{
		++num_blocks;
	}

	add_arrays<<<num_blocks, block_size>>>(device_array_a, device_array_b,
		device_array_c, N);

	err = cudaDeviceSynchronize();
	DIE(err != cudaSuccess, "cudaDeviceSynchronize");

	// TODO 7: Copy back the results and then uncomment the checking function
	err = cudaMemcpy(host_array_c, device_array_c,
		N * sizeof(*host_array_c), cudaMemcpyDeviceToHost);
	DIE(err != cudaSuccess, "cudaMemcpy(device_array_c)");

	check_task_2(host_array_a, host_array_b, host_array_c, N);

	// TODO 8: Free the memory
	free(host_array_a);
	free(host_array_b);
	free(host_array_c);

	err = cudaFree(device_array_a);
	DIE(err != cudaSuccess, "cudaFree(device_array_a)");

	err = cudaFree(device_array_b);
	DIE(err != cudaSuccess, "cudaFree(device_array_b)");

	err = cudaFree(device_array_c);
	DIE(err != cudaSuccess, "cudaFree(device_array_c)");
   
	return 0;
}