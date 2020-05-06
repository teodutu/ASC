#include <iostream>
#include <limits.h>
#include <stdlib.h>
#include <ctime>
#include <sstream>
#include <string>

#include "gpu_hashtable.hpp"

#define MAX_LOAD_FACTOR		.9f
#define MIN_LOAD_FACTOR		.85f

/* INIT HASH
 */
__device__ size_t computeHash(int key)
{
	size_t hash = (size_t)key;

	hash = ~hash + (hash << 15); // key = (key << 15) - key - 1;
	hash = hash ^ (hash >> 12);
	hash = hash + (hash << 2);
	hash = hash ^ (hash >> 4);
	hash = hash + (hash << 3) + (hash << 11);
	hash = hash ^ (hash >> 16);

	return hash;
}

__global__ void kernel_insert(Entry* hashMap, int* devKeys, int* devValues,
	size_t capacity)
{
	int oldKey;
	bool inserted = false;
	size_t hash;
	Entry insertedEntry;
	size_t idx = blockIdx.x * blockDim.x + threadIdx.x;

	if (idx > capacity)
	{
		return;
	}

	hash = computeHash(devKeys[idx]) % capacity;
	insertedEntry = {devKeys[idx], devValues[idx]};

	for (; !inserted; hash = (hash + 1) % capacity)
	{
		oldKey = atomicCAS(&hashMap[hash].key, KEY_INVALID, insertedEntry.key);

		if (KEY_INVALID == oldKey || insertedEntry.key == oldKey)
		{
			hashMap[hash].value = insertedEntry.value;
			inserted = true;
		}
	}
}

__global__ void kernel_search(Entry* hashMap, int* devKeys, int* values,
	size_t capacity, int numKeys)
{
	bool found = false;
	size_t hash;
	size_t idx = blockIdx.x * blockDim.x + threadIdx.x;

	if (idx > numKeys)
	{
		return;
	}

	hash = computeHash(devKeys[idx]) % capacity;

	for (; !found; hash = (hash + 1) % capacity)
	{
		if (devKeys[idx] == hashMap[hash].key)
		{
			values[idx] = hashMap[hash].value;
			found = true;
		}
	}
}

__global__ void kernel_rehash(Entry* resizedHashMap, Entry* hashMap,
	size_t initialCapacity, int finalCapacity)
{
	bool reinserted = false;
	size_t hash;
	size_t idx = blockIdx.x * blockDim.x + threadIdx.x;

	if (idx > initialCapacity || KEY_INVALID == hashMap[idx].key)
	{
		return;
	}

	hash = computeHash(hashMap[idx].key) % finalCapacity;

	for (; !reinserted; hash = (hash + 1) % finalCapacity)
	{
		if (KEY_INVALID ==
			atomicCAS(&resizedHashMap[hash].key, KEY_INVALID, hashMap[idx].key))
		{
			resizedHashMap[hash].value = hashMap[idx].value;
			reinserted = true;
		}
	}
}

GpuHashTable::GpuHashTable(int size) :
	capacity(size), size(0)
{
	cudaError_t ret;
	size_t numBytes = capacity * sizeof(*hashMap);

	ret = cudaMalloc(&hashMap, numBytes);
	ASSERT(ret, "cudaMalloc(hashMap) failed", exit(errno));

	// O pozitie din hashtable nu contine un element daca cheia de pe aceasta
	// este 0
	ret = cudaMemset(hashMap, 0, numBytes);
	ASSERT(ret, "cudaMemset(hashMap) failed", exit(errno));
}

/* DESTROY HASH
 */
GpuHashTable::~GpuHashTable()
{
	cudaFree(hashMap);
}

/* RESHAPE HASH
 */
void GpuHashTable::reshape(int numBucketsReshape)
{
	cudaError_t ret;
	int numBlocks;
	int numThreads;
	Entry* resizedHashMap;
	size_t numBytes = numBucketsReshape * sizeof(*resizedHashMap);

	ret = cudaMalloc(&resizedHashMap, numBytes);
	ASSERT(ret, "cudaMalloc(hashMap) failed", exit(errno));

	ret = cudaMemset(resizedHashMap, 0, numBytes);
	ASSERT(ret, "cudaMemset(resizedHashMap) failed", exit(EXIT_FAILURE));

	ret = getNumBlocksThreads(numBlocks, numThreads, capacity);
	ASSERT(ret, "getNumBlocksThreads() failed", exit(EXIT_FAILURE));

	kernel_rehash<<<numBlocks, numThreads>>>(resizedHashMap, hashMap,
		capacity, numBucketsReshape);

	ret = cudaDeviceSynchronize();
	ASSERT(ret, "cudaDeviceSynchronize() failed", exit(EXIT_FAILURE));

	ret = cudaFree(hashMap);
	ASSERT(ret, "cudaFree(hashMap) failed", exit(EXIT_FAILURE));

	hashMap = resizedHashMap;
	capacity = numBucketsReshape;
}

/* INSERT BATCH
 */
bool GpuHashTable::insertBatch(int* keys, int* values, int numKeys)
{
	cudaError_t ret;
	int numBlocks;
	int numThreads;
	int* devKeys;
	int* devValues;
	size_t numBytes = numKeys * sizeof(*devKeys);

	ret = cudaMalloc(&devKeys, numBytes);
	ASSERT(ret, "cudaMalloc(devKeys) failed", return false);

	ret = cudaMemcpy(devKeys, keys, numBytes, cudaMemcpyHostToDevice);
	ASSERT(ret, "cudaMemcpy(devKeys) failed", return false);

	ret = cudaMalloc(&devValues, numBytes);
	ASSERT(ret, "cudaMalloc(devValues) failed", return false);

	ret = cudaMemcpy(devValues, values, numBytes, cudaMemcpyHostToDevice);
	ASSERT(ret, "cudaMemcpy(devValues) failed", return false);

	if ((size + numKeys) / float(capacity) >= MAX_LOAD_FACTOR)
	{
		reshape((size + numKeys) / MIN_LOAD_FACTOR + 1);
	}

	ret = getNumBlocksThreads(numBlocks, numThreads, numKeys);
	ASSERT(ret, "getNumBlocksThreads() failed", return false);

	kernel_insert<<<numBlocks, numThreads>>>(hashMap, devKeys, devValues,
		capacity);

	ret = cudaDeviceSynchronize();
	ASSERT(ret, "cudaDeviceSynchronize() failed", return false);

	size += numKeys;

	ret = cudaFree(devKeys);
	ASSERT(ret, "cudaFree(devKeys) failed", return false);

	ret = cudaFree(devValues);
	ASSERT(ret, "cudaFree(devValues) failed", return false);

	return true;
}

/* GET BATCH
 */
int* GpuHashTable::getBatch(int* keys, int numKeys)
{
	cudaError_t ret;
	int numBlocks;
	int numThreads;
	int* devKeys;
	int* values;
	size_t numBytes = numKeys * sizeof(*devKeys);

	ret = cudaMalloc(&devKeys, numBytes);
	ASSERT(ret, "cudaMalloc(devKeys) failed", return NULL);

	ret = cudaMemcpy(devKeys, keys, numBytes, cudaMemcpyHostToDevice);
	ASSERT(ret, "cudaMemcpy(devKeys) failed", return NULL);

	ret = cudaMallocManaged(&values, numBytes);
	ASSERT(ret, "cudaMallocManaged(values) failed", return NULL);

	ret = getNumBlocksThreads(numBlocks, numThreads, numKeys);
	ASSERT(ret, "getNumBlocksThreads() failed", return NULL);

	kernel_search<<<numBlocks, numThreads>>>(hashMap, devKeys, values,
		capacity, numKeys);

	cudaDeviceSynchronize();

	ret = cudaFree(devKeys);
	ASSERT(ret, "cudaMalloc(devKeys) failed",);

	return values;
}

/* GET LOAD FACTOR
 * num elements / hash total slots elements
 */
float GpuHashTable::loadFactor()
{
	return (float)size / capacity; // no larger than 1.0f = 100%
}

cudaError_t GpuHashTable::getNumBlocksThreads(int& numBlocks, int& numThreads,
	int numKeys)
{
	cudaError_t ret;
	cudaDeviceProp devProp;

	ret = cudaGetDeviceProperties(&devProp, 0);
	ASSERT(ret, "cudaGetDeviceProperties failed", return ret);

	// fprintf(stderr, "threads = %d\n", devProp.maxThreadsPerBlock);
	numThreads = devProp.maxThreadsPerBlock;
	numBlocks = numKeys / numThreads;

	if (numBlocks * numThreads != numKeys)
	{
		++numBlocks;
	}

	return cudaSuccess;
}

int GpuHashTable::hashShift(int key)
{
	key = ~key + (key << 15); // key = (key << 15) - key - 1;
	key = key ^ (key >> 12);
	key = key + (key << 2);
	key = key ^ (key >> 4);
	key = key * 2057; // key = (key + (key << 3)) + (key << 11);
	key = key ^ (key >> 16);

	return key;
}

int GpuHashTable::hashJenkins(int key)
{
	key = (key + 0x7ed55d16) + (key << 12);
	key = (key ^ 0xc761c23c) ^ (key >> 19);
	key = (key + 0x165667b1) + (key << 5);
	key = (key + 0xd3a2646c) ^ (key << 9);
	key = (key + 0xfd7046c5) + (key << 3);
	key = (key ^ 0xb55a4f09) ^ (key >> 16);

	return key;
}

int GpuHashTable::hashShiftMult(int key)
{
	key = (key ^ 61) ^ (key >> 16);
	key = key + (key << 3);
	key = key ^ (key >> 4);
	key = key * 0x27d4eb2d;
	key = key ^ (key >> 15);

	return key;
}

int GpuHashTable::hashKnuth(int key)
{
	return key * 2654435761 % (1LLU << 32);
}

/*********************************************************/

#define HASH_INIT GpuHashTable GpuHashTable(1);
#define HASH_RESERVE(size) GpuHashTable.reshape(size);

#define HASH_BATCH_INSERT(keys, values, numKeys) GpuHashTable.insertBatch(keys, values, numKeys)
#define HASH_BATCH_GET(keys, numKeys) GpuHashTable.getBatch(keys, numKeys)

#define HASH_LOAD_FACTOR GpuHashTable.loadFactor()

#include "test_map.cpp"
