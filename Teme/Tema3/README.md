# Tema 3 - Hashtable parelel implementat folosind CUDA

## Stocarea datelor
Hashtable-ul este implementat ca un vector de structuri de tip `Entry`. Aceasta
structura retine 2 valori de tip `int`: cheia si valoarea asociata acesteia si
se aseamana cu clasa `Map.Entry` din _Java_, dar e mult mai simplista, retinand
doar cele 2 valori mentionate.

Asadar, hashtable-ul este un sir continuu de astfel de `Entry`-uri alocate in
VRAM. Astfel, data fiind tehnica de hashing folosita, si anume _linear probing_,
accesul la elementele hashtable-ului in cazul unei coliziuni se face rapid,
datorita faptului ca urmatorii indecsi incercati in cadrul metodelor clasei
`GpuHashTable`, dupa calcularea hashului cheii, sunt adiacenti, ceea ce confera
localitate spatiala implementarii. Aceasta localitate spatiala imbunatateste
rata de hit in cache-urile GPU-ului, ceea ce duce la un timp mai mic de acces la
date in general.

## Implementare
Clasa `GpuHashTable` retine 3 campuri private: `hashMap` reprezentand vectorul
de bucketuri de tip `Entry` care vor stoca perechile adaugate in hasthable,
numarul maxim de elemente ce pot fi stocate in `hashMap` (`capacity`), precum si
numarul real de elemente stocate (`size`).

### Functia de hash
Functia de hash aleasa se bazeaza pe cea propusa de Robert Jenkins [0] si
analizata aici [1]. Numarul mare de operatii efectuate de aceasta functie
mareste uniformitatea distributiei rezultatelor acesteia.

### loadFactor()
Cea mai simpla functie din clasa implementata, aceasta doar returneaza
rezultatul raportului `size / capacity`.

### insertBatch()
Initial, functia va copia din RAM in VRAM cheile si valorile care trebuie
adaugate, dupa care va lansa in executie kernelurile care fac efectiv inserarea
in hashtable, cate unul pentru fiecare element adaugat. Numarul de threaduri per
bloc este numarul maxim admis de GPU, pentru a maximiza paralelismul.

Fiecare dintre aceste threaduri se va ocupa de inserarea in hashtable a **unei
singure perechi** cheie - valoare. Astfel, un thread va calcula hash-ul cheii si
apoi va folosi `atomicCAS()` pentru a scrie aceasta cheie in hashtable. Valoarea
cu care se compara este aceea care reprezinta un `Entry` gol (`KEY_INVALID`),
iar cand valoarea intoarsa de functie este tot `KEY_INVALID`, inseamna ca s-a
gasit o pozitie libera in hashtable, iar valoarea este scrisa la aceasta.
Acelasi lucru se intampla si atunci cand valoarea intoarsa este fix cheia care
se incearca sa se insereze.

Se garanteaza ca nu vor exista duplicate, intrucat la fiecare calculare a
hashului, rezultatul va fi acelasi pentru aceeasi cheie, iar indecsii din
hashtable verificati si pe ai caror chei se va apela `atomicCAS()` vor fi mereu
aceiasi.

Cand o pozitie din hashtable este deja ocupata de alta cheie, se incearca
urmatoarea pozitie din hashtable parcurgandu-se circular vectorul `hashMap`.
Dat fiind ca mereu procentajul de incarcare al hashtable-ului este subunitar,
se garanteaza ca se va gasi intotdeauna o pozitie la care sa se adauge o noua
pereche cheie - valoare.

Mai mult decat atat, `atomicCAS()` actioneaza ca un mutex. Cand acesta intoarce
cheia de la indexul threadului sau `KEY_INVALID`, inseamna ca cheia a fost
scrisa la adresa care i s-a dat ca parametru si, deci, urmatoarele apeluri nu
vor putea sa scrie la acea adresa. Asadar, in continuare, threadul care a reusit
sa efectueze `atomicCAS()` va avea acces exclusiv la valoarea asociata cheii.

### getBatch()
Gasirea cheii functioneaza similar celei din `insertBatch()`, ar acum nu mai
este nevoie de `atomicCAS`, din moment ce hashtable-ul nu mai trebuie modificat.

Asadar, acum fiecare thread va cauta una dintre cheile ale caror valori se cer,
se calculeaza hash in aceeasi maniera ca mai sus si cand cheia de la pozitia
indicata de hash este cea cautata, se scrie intr-un vector partajat intre GPU si
RAM valoarea asociata acesteia.

### reshape()
Oarecum o combinatie dintre functiile de mai sus, aceasta va aloca spatiu pentru
noile perechi si il va zeroiza. Apoi, va lansa cate un thread pentru fiecare
pozitie din hashtable-ul vechi. Daca la aceasta pozitie nu exista o cheie valida,
threadul se incheie. Altfel, urmeaza ca valoarea asociata cheii sa se adauge in
hashtable-ul nou alocat in acelasi mod ca in cazul kernelului apelat de
`insertBatch()`.

## Rezultate
Rezultatele obtinute in urma rularii scriptului `bench.py` pe coada **hp-sl.q**
sunt urmatoarele:

```
-------------- Test T1 --------------
OK	 +10 pts	 HASH_BATCH_INSERT, 1000000, 100, 84.962
OK	 +10 pts	 HASH_BATCH_GET, 1000000, inf, 84.8858
TOTAL	 +20 pts

-------------- Test T2 --------------
OK	 +5 pts	 HASH_BATCH_INSERT, 2000000, 100, 84.9619
OK	 +5 pts	 HASH_BATCH_GET, 2000000, 200, 84.9238
TOTAL	 +10 pts

-------------- Test T3 --------------
OK	 +5 pts	 HASH_BATCH_INSERT, 2000000, 200, 84.9619
OK	 +5 pts	 HASH_BATCH_INSERT, 2000000, 66.6667, 84.981
OK	 +5 pts	 HASH_BATCH_GET, 2000000, 200, 84.9619
OK	 +5 pts	 HASH_BATCH_GET, 2000000, 200, 84.9619
TOTAL	 +20 pts

-------------- Test T4 --------------
OK	 +5 pts	 HASH_BATCH_INSERT, 2500000, 83.3333, 84.9793
OK	 +5 pts	 HASH_BATCH_INSERT, 2500000, 62.5, 84.9897
OK	 +5 pts	 HASH_BATCH_INSERT, 2500000, 62.5, 84.9931
OK	 +5 pts	 HASH_BATCH_INSERT, 2500000, 41.6667, 84.9948
OK	 +5 pts	 HASH_BATCH_GET, 2500000, inf, 84.9872
OK	 +5 pts	 HASH_BATCH_GET, 2500000, inf, 84.9872
OK	 +5 pts	 HASH_BATCH_GET, 2500000, 250, 84.9872
OK	 +5 pts	 HASH_BATCH_GET, 2500000, 250, 84.9872
TOTAL	 +40 pts
```
**Precizare:** Date fiind cerintele problemei, care necesita ca factorul de
umplere minim sa fie de 80%, am ales sa redimensionez hashtable-ul cand acest
factor de umplere ajunge la 90%, astfel incat noul factor de umplere sa ajunga
la 85%.

Dati fiind timpii foarte mici de executie ai programului, din cand in cand acesta
ajunge sa tinda la 0, drept care throughputul apare a fi infinit.

Se observa ca pe masura ce dimensiunea hashtable-ului creste, throughput-ul este
din ce in ce mai mic din cauza necesitatii rehashuirii cheilor (prin functia
`reshape()` descrisa mai sus).

Intrucat nu este nevoie de sincronizare, se observa ca, nemodificand
hashtable-ul si, in consecinta, neavand nevoie de operatii atomice si nici de
`reshape()`, functia `getBatch()` ruleaza cu mult mai repede decat inserarea.
De altfel, rezultatul acesteia (throughput-ul) este mult mai putin afectat de
dimensiunea hashtable-ului (variind mult mai putin de la un chunk la altul),
tocmai pentru ca se elimina pasul de `reshape()`.

De asemenea, am rulat ultimul test din `bench.py` cu `nvprof`, iar timpii
petrecuti in fiecare functie sunt urmatorii:

```
==35415== NVPROF is profiling process 35415, command: ./gpu_hashtable 10000000 4
==35415== Profiling application: ./gpu_hashtable 10000000 4
==35415== Profiling result:
            Type  Time(%)      Time     Calls       Avg       Min       Max  Name
 GPU activities:   34.92%  67.963ms         5  13.593ms  203.07us  22.138ms  kernel_insert(Entry*, int*, int*, int*, unsigned long)
                   29.11%  56.659ms        14  4.0471ms  1.0560us  6.2675ms  [CUDA memcpy HtoD]
                   23.35%  45.457ms         5  9.0914ms  3.0080us  23.557ms  kernel_rehash(Entry*, Entry*, unsigned long, int)
                   12.61%  24.549ms         4  6.1372ms  3.6138ms  13.694ms  kernel_search(Entry*, int*, int*, unsigned long, int)
                    0.00%  9.5360us         6  1.5890us  1.1200us  3.0720us  [CUDA memset]
      API calls:   93.65%  3.77212s        20  188.61ms  10.685us  3.76640s  cudaMalloc
                    3.43%  138.07ms        14  9.8619ms  8.5890us  23.566ms  cudaDeviceSynchronize
                    1.59%  64.183ms        14  4.5845ms  11.465us  7.5643ms  cudaMemcpy
                    0.66%  26.722ms         9  2.9691ms  218.03us  20.882ms  cudaMallocManaged
                    0.31%  12.320ms        25  492.81us  16.820us  1.0872ms  cudaFree
                    0.21%  8.6586ms        14  618.47us  573.47us  733.21us  cudaGetDeviceProperties
                    0.06%  2.3457ms        14  167.55us  85.104us  398.26us  cudaLaunch
                    0.05%  2.1168ms       282  7.5060us     248ns  284.22us  cuDeviceGetAttribute
                    0.02%  934.65us         3  311.55us  307.10us  313.77us  cuDeviceTotalMem
                    0.01%  372.50us         6  62.082us  35.181us  72.332us  cudaMemset
                    0.00%  192.67us         3  64.223us  59.918us  72.070us  cuDeviceGetName
                    0.00%  24.138us        65     371ns     181ns  2.0370us  cudaSetupArgument
                    0.00%  18.505us        14  1.3210us     594ns  2.6330us  cudaConfigureCall
                    0.00%  5.2460us         6     874ns     319ns  2.6770us  cuDeviceGet
                    0.00%  3.1600us         3  1.0530us     311ns  1.9550us  cuDeviceGetCount
```
Dupa cum se poate observa, cel mai mult timp dureaza executia kernelurilor, ceea
ce este un lucru bun, insemnand ca programul nu are overheaduri semnificative.
Totusi, acesta are un overhead vizibil, si anume acela de copiere a datelor din
RAM in VRAM (cheile sau valorile, pentru `kernel_insert()` sau
`kernel_search()`). Acest overhead nu poate fi, insa, evitat, data fiind
alocarea datelor in RAM facuta in `test_map.cpp`, caci pentru a avea kernelurile
acces la date, este necesar ca acestea sa fie copiate si in VRAM. 

## Bibliografie
- [0] https://en.wikipedia.org/wiki/Jenkins_hash_function
- [1] https://gist.github.com/badboy/6267743
