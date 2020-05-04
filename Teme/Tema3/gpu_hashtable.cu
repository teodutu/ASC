#include <iostream>
#include <limits.h>
#include <stdlib.h>
#include <ctime>
#include <sstream>
#include <string>

#include "gpu_hashtable.hpp"

#define hash_func	hashShift

/* INIT HASH
 */
GpuHashTable::GpuHashTable(int size)
{
}

/* DESTROY HASH
 */
GpuHashTable::~GpuHashTable()
{
}

/* RESHAPE HASH
 */
void GpuHashTable::reshape(int numBucketsReshape)
{
}

/* INSERT BATCH
 */
bool GpuHashTable::insertBatch(int *keys, int* values, int numKeys)
{
	return false;
}

/* GET BATCH
 */
int* GpuHashTable::getBatch(int* keys, int numKeys)
{
	return NULL;
}

/* GET LOAD FACTOR
 * num elements / hash total slots elements
 */
float GpuHashTable::loadFactor()
{
	return 0.f; // no larger than 1.0f = 100%
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
