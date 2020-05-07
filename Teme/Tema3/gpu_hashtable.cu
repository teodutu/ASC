#include <iostream>
#include <limits.h>
#include <stdlib.h>
#include <ctime>
#include <sstream>
#include <string>

#include "gpu_hashtable.hpp"

#define MAX_LOAD_FACTOR		.9f
#define MIN_LOAD_FACTOR		.85f


// Calculeaza hash-ul unei chei prin algoritmul descris aici
// https://gist.github.com/badboy/6267743
// Dintre functiile propuse de Bob Jenkins, aceasta avea
// performantele cele mai bune
static __device__ size_t computeHash(int key)
{
	size_t hash = (size_t)key;

	hash = ~hash + (hash << 15);
	hash = hash ^ (hash >> 12);
	hash = hash + (hash << 2);
	hash = hash ^ (hash >> 4);
	hash = (hash + (hash << 3)) + (hash << 11);
	hash = hash ^ (hash >> 16);

	return hash;
}

// Kernelul se ocupa cu inserarea unui singur element in hahstable folosind
// tehnica "linear probing".
static __global__ void kernel_insert(Entry* hashMap, int* devKeys,
	int* devValues, int* numUpdates, size_t capacity)
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

	// se calculeaza hashul initial
	hash = computeHash(devKeys[idx]) % capacity;
	insertedEntry = {devKeys[idx], devValues[idx]};

	// Se parcurg indecsii in ordine incepand de la `hash` si se cauta o pozitie
	// libera sau pe care se afla aceeasi cheie (caz de update)
	for (; !inserted; hash = (hash + 1) % capacity)
	{
		// Cheia veche se schimba cu cea noua doar daca aceasta era KEY_INVALID
		// (0).
		oldKey = atomicCAS(&hashMap[hash].key, KEY_INVALID, insertedEntry.key);

		// In situatia in care cheia era `KEY_INVALID` (locul era liber) sau era
		// aceeasi cu noua cheie (update), valoarea se modifica
		if (KEY_INVALID == oldKey || insertedEntry.key == oldKey)
		{
			if (oldKey == insertedEntry.key)
			{
				atomicAdd(numUpdates, 1);
			}

			hashMap[hash].value = insertedEntry.value;
			inserted = true;
		}
	}
}

// Kernelul cauta sa puna in vectorul `values` valoarea corespunzatoare cheii
// indexului sau
static __global__ void kernel_search(Entry* hashMap, int* devKeys, int* values,
	size_t capacity, int numKeys)
{
	bool found = false;
	size_t hash;
	size_t idx = blockIdx.x * blockDim.x + threadIdx.x;

	if (idx > numKeys)
	{
		return;
	}

	// Initial se calculeaza hashul cheii care ii revine threadului curent.
	hash = computeHash(devKeys[idx]) % capacity;

	// Exact ca la inert, se parcurg indecsii pe rand, cautandu-se cel la care
	// este stocata de fapt cheia.
	for (; !found; hash = (hash + 1) % capacity)
	{
		if (devKeys[idx] == hashMap[hash].key)
		{
			values[idx] = hashMap[hash].value;
			found = true;
		}
	}
}

// Kernelul rehashuieste cheia din bucketurile vechi care corespunde fiecarui
// thread si o plaseaza impreuna cu valoarea sa in noul set de bucketuri.
// Daca cheia la pozitia care ii corespunde, threadul nu gaseste un element,
// acesta se termina imediat. 
static __global__ void kernel_rehash(Entry* resizedHashMap, Entry* hashMap,
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

	// Se urmeaza aceeasi logica de la `kernel_insert`.
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

/* INIT HASH
 */
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
	int* numUpdates;  // cateva chei vor fi update-uri
	int* devKeys;  // se vor copia cheile pe GPU
	int* devValues;  // aceeasi copiere se face si pentru valori
	size_t numBytes = numKeys * sizeof(*devKeys);

	ret = cudaMalloc(&devKeys, numBytes);
	ASSERT(ret, "cudaMalloc(devKeys) failed", return false);

	ret = cudaMemcpy(devKeys, keys, numBytes, cudaMemcpyHostToDevice);
	ASSERT(ret, "cudaMemcpy(devKeys) failed", return false);

	ret = cudaMalloc(&devValues, numBytes);
	ASSERT(ret, "cudaMalloc(devValues) failed", return false);

	ret = cudaMemcpy(devValues, values, numBytes, cudaMemcpyHostToDevice);
	ASSERT(ret, "cudaMemcpy(devValues) failed", return false);

	ret = cudaMallocManaged(&numUpdates, sizeof(*numUpdates));
	ASSERT(ret, "cudaMallocManaged(numUpdates) failed", return false);

	// Hashtable-ul isi modifica dimensiunea cand se depaseste procentajul
	// maxim de umplere.
	if ((size + numKeys) / float(capacity) >= MAX_LOAD_FACTOR)
	{
		reshape((size + numKeys) / MIN_LOAD_FACTOR);
	}

	ret = getNumBlocksThreads(numBlocks, numThreads, numKeys);
	ASSERT(ret, "getNumBlocksThreads() failed", return false);

	kernel_insert<<<numBlocks, numThreads>>>(hashMap, devKeys, devValues,
		numUpdates, capacity);

	ret = cudaDeviceSynchronize();
	ASSERT(ret, "cudaDeviceSynchronize() failed", return false);

	// S-au adaugat `numKeys` - cheile care au fost updatate (numUpdates).
	size += numKeys - *numUpdates;

	ret = cudaFree(devKeys);
	ASSERT(ret, "cudaFree(devKeys) failed", return false);

	ret = cudaFree(devValues);
	ASSERT(ret, "cudaFree(devValues) failed", return false);

	ret = cudaFree(numUpdates);
	ASSERT(ret, "cudaFree(numUpdates)", return false);

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
	int numItems)
{
	cudaError_t ret;
	cudaDeviceProp devProp;

	// Se presupune ca toate placile sunt de acelasi tip, motiv pentru care se
	// interogheaza placa 0.
	ret = cudaGetDeviceProperties(&devProp, 0);
	ASSERT(ret, "cudaGetDeviceProperties failed", return ret);

	numThreads = devProp.maxThreadsPerBlock;
	numBlocks = numItems / numThreads;

	if (numBlocks * numThreads != numItems)
	{
		++numBlocks;
	}

	return cudaSuccess;
}

/*********************************************************/

#define HASH_INIT GpuHashTable GpuHashTable(1);
#define HASH_RESERVE(size) GpuHashTable.reshape(size);

#define HASH_BATCH_INSERT(keys, values, numKeys) GpuHashTable.insertBatch(keys, values, numKeys)
#define HASH_BATCH_GET(keys, numKeys) GpuHashTable.getBatch(keys, numKeys)

#define HASH_LOAD_FACTOR GpuHashTable.loadFactor()

#include "test_map.cpp"
