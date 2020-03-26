#include <stdio.h>
#include <stdlib.h>

int start = 1, end = 400000;
// TODO 1: Start testing for primes up to 4000 numbers. Compile a serial version with: gcc -o prime-ex primes.c
// TODO 2: Increase the number of prime numbers tested to 400000 (test at least 40000 / 100000 / 200000 / 300000 / 400000). You might test more numbers. Record runtimes (e.g. time ./prime-ex).
// TODO 3: Compile a parallel version of the code (with OpenMP) with: gcc -fopenmp -o prime-omp primes.c
// TODO 4: Record runtimes for the same dimensions as in TODOs 1 and 2.

// Allocating space for 1/10 of tested numbers is safe. Thus the allocation below is ok for testing prime numbers up to 400000.
int globalPrimes [40000];
int gPrimesFound = 0;
int gProgress = 0;

int TestForPrime(int n)
{
	int i, flag = 0;

	for (i = 2; i <= n / 2; ++i)
	{
	        // condition for non-prime
        	if (n % i == 0)
        	{
            		flag = 1;
            		break;
        	}
    	}
	return flag;	
}

void ShowProgress (int val, int range)
{
	int percentDone = 0;

	#pragma omp critical
	{
		gProgress ++;
		percentDone = (int)((float)gProgress / (float)range * 200.0f + 0.5f); 
	}

	if (percentDone % 10 == 0)
	{
		printf("\b\b\b\b%3d%%", percentDone);
	}
}

void FindPrimes(int start, int end)
{
	// start is always odd
	int range = end - start + 1; 
	int i; 

	#pragma omp parallel for schedule(static)
	for(i = start; i <= end; i += 2)
	{
		if(!TestForPrime(i))
		{
			#pragma omp critical
			{
				globalPrimes[gPrimesFound] = i;
				gPrimesFound++;
			}
		}

		ShowProgress(i, range);
	}
}

int main()
{
	int i, printNr = 100;
	FindPrimes(start, end);

	printf("\n");

	// TODO 5: Print the first "printNr" prime numbers from the saved list. Test & explain output when using serial / parallel (OpenMP) version for points 1 through 4 (above).
	for(i = 0; i < printNr; i++)
	{
		printf("%d \t", globalPrimes[i]);
	}

	printf("\n");

	return 0;
}
