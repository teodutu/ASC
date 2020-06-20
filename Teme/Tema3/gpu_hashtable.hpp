#ifndef HASHGPU_
#define HASHGPU_

using namespace std;

#define	KEY_INVALID		0

#define DIE(assertion, call_description) 		\
	do {										\
		if (assertion) {						\
			fprintf(stderr, "(%s, %d): ",		\
				__FILE__, __LINE__);			\
			perror(call_description);			\
			exit(errno);						\
		}										\
	} while (0)

// Similar cu DIE-ul de mai sus; singura diferenta este ca DIE apeleaza
// exit(errno), pe cand ASSERT foloseste o instructiune primita ca parametru.
#define ASSERT(assertion, call_description, instr) 	\
	do {											\
		if (assertion) {							\
			fprintf(stderr, "(%s, %d): ",			\
				__FILE__, __LINE__);				\
			perror(call_description);				\
			instr;									\
		}											\
	} while (0)

// o pereche cheie-valoare din hashtable
struct Entry
{
	int key, value;
};

//
// GPU HashTable
//
class GpuHashTable
{
private:
	// Hashtable-ul
	Entry* hashMap;

	// Numarul total de elemente perechi din hashtable
	size_t capacity;

	// Numarul de perechi adaugate in hashtable
	size_t size;

	// Interogheaza GPU-ul pentru a-i afla numarul maxim de threaduri dintr-un
	// bloc si calculeaza numarul de blocuri pe baza acestuia si pe baza
	// numarului de elemente ce vor fi cautate/adaugate.
	cudaError_t getNumBlocksThreads(int& numBlocks, int& numThreads,
		int numItems);

public:
	// Aloca structura pentru hashtable in VRAM si o zeroizeaza.
	GpuHashTable(int size);

	// Redimensioneaza hashtable-ul la dimensiunea data.
	void reshape(int sizeReshape);

	// Adauga `numKeys` perechi cheie-valoare, reprezentate prin indici identici
	// in vectorii `keys` si `values`.
	bool insertBatch(int *keys, int* values, int numKeys);

	// Returneaza un vector de `numItems` elemente ce contine valorile care
	// corespund cheilor date.
	int* getBatch(int* key, int numItems);

	// Returneaza procentajul de umplere al hashtable-ului, adica
	// `size`/`capacity`.
	float loadFactor();

	~GpuHashTable();
};

#endif  // HASHGPU_
