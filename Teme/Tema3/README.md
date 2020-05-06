# Tema 3 - Hashtable parelel folosind CUDA

[link hash](https://gist.github.com/badboy/6267743)

## Stocarea datelor
Hashtable-ul este implementat ca un vector de structuri de tip `Entry`. Aceasta
structura retine 2 valori de tip `int`: cheia si valoarea asociata acesteia si
se aseamana cu clasa `Map.Entry` din _Java_, dar e mult mai simplista, reinan
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
Functia de hash aleasa se baseaza pe aceeas propusa de Robert Jenkins [0] si
analizata aici [1]. Numarul mare de operatii efectuata de aceasta functie
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
hashului, rezultatul va fi acelasi pentru aceeasi cheie, iar din hashtable
verificati si pe care se va apela `atomicCAS` vor fi mereu aceiasi.

Cand o pozitie din hashtable este deja ocupata de alta cheie, se incearca
urmatoarea pozitie din hashtable parcurgandu-se circular vectorul `hashMap`.
Dat fiind ca mereu procentajul de incarcare al hashtable-ului este subunitar,
se garanteaza ca se va gasi intotdeauna o pozitie la care sa se adauge o noua
pereche cheie - valoare.

Mai mult decat atat, `atomicCAS()` actioneaza ca un mutex. Cand aceasta intoarce
cheia de la indexul threadului sau `KEY_INVALID`, inseamna ca cheia a fost
scrisa la adresa care i s-a dat ca parametru si, deci, urmatoarele apeulri nu
vor putea sa scrie la acea adresa. Asadar, in continuare, threadul care a reusit
sa efectueze `atomicCAS()`.

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
threadul se incheiei. Altfel, urmeaza ca valoarea asociata cheii sa se adauge in
hashtable-ul nou alocat in acelasi mod ca in cazul kernelului apelat de
`insertBatch()`.

## Rezultate
Rezultatele obtinute pe coada **hp-sl.q** sunt urmatoarele:

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
factpr de umplere ajunge la 90% la dimensiunea la care noul factor de umplere
va fi 85%.

Dati fiind timpii foarte mici de executie ai programului, din cand in cand acsta
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

## Bibliografie
- [0] https://en.wikipedia.org/wiki/Jenkins_hash_function
- [1] https://gist.github.com/badboy/6267743


De rezerva:
```
-------------- Test T1 --------------
OK	 +10 pts	 HASH_BATCH_INSERT, 1000000, 100, 85
OK	 +10 pts	 HASH_BATCH_GET, 1000000, inf, 85.0054
TOTAL	 +20 pts

-------------- Test T2 --------------
OK	 +5 pts	 HASH_BATCH_INSERT, 2000000, 100, 85
OK	 +5 pts	 HASH_BATCH_GET, 2000000, 200, 85.0027
TOTAL	 +10 pts

-------------- Test T3 --------------
OK	 +5 pts	 HASH_BATCH_INSERT, 2000000, 100, 85
OK	 +5 pts	 HASH_BATCH_INSERT, 2000000, 66.6667, 85
OK	 +5 pts	 HASH_BATCH_GET, 2000000, inf, 85.0014
OK	 +5 pts	 HASH_BATCH_GET, 2000000, 100, 85.0014
TOTAL	 +20 pts

-------------- Test T4 --------------
OK	 +5 pts	 HASH_BATCH_INSERT, 2500000, 83.3333, 85
OK	 +5 pts	 HASH_BATCH_INSERT, 2500000, 83.3333, 85
OK	 +5 pts	 HASH_BATCH_INSERT, 2500000, 62.5, 85
OK	 +5 pts	 HASH_BATCH_INSERT, 2500000, 50, 85
OK	 +5 pts	 HASH_BATCH_GET, 2500000, 250, 85.0005
OK	 +5 pts	 HASH_BATCH_GET, 2500000, 250, 85.0005
OK	 +5 pts	 HASH_BATCH_GET, 2500000, inf, 85.0005
OK	 +5 pts	 HASH_BATCH_GET, 2500000, 250, 85.0005
TOTAL	 +40 pts
```