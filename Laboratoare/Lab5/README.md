# Laborator 5
Optimizarea operatiei de inmultire de matrice

## Implementarea clasica
Toate rezultatele vor fi comparate cu timpul de rulare al algoritmului clasic,
fara optimizari:
```
teo@obor Lab5 $ ./basic
Time for N = 1500 is 18.748564 seconds.
```

## Eliminarea constantelor
Prima optimizare presupune eliminarea constantelor precum `C[i][j]` din bucle
prin inlocuire cu pointeri. Imbunatatirea se poate vedea:
```
teo@obor Lab5 $ ./const_elim
Time for N = 1500 is 11.513561 seconds.
```

## Reordonarea buclelor
Se analizeaza abordarile `k-i-j` in _loop\_reorder\_better_ si `k-j-i` in
_loop\_reorder\_worse_, unde diferentele intre timpii de rulare sunt majore.

Reordonarea buclelor a fost combinata cu eliminarea constantelor pentru
performante mai bune (si varinta `k-j-i` tot are rezultate dezastruoase):
```
teo@obor Lab5 $ ./loop_reorder_better
Time for N = 1500 is 7.045534 seconds.
teo@obor Lab5 $ ./loop_reorder_worse
Time for N = 1500 is 26.200640 seconds.
```

## Inmultire folosing blocuri
Am analizat doua abordari. In prima matricele au fost liniarizate si s-au si
eliminat constantele din bucle, in mod similar cu _const\_elim_, iar speedupul
este urmatorul pentru o dimensiune relativ optima a blocului (50):
```
teo@obor Lab5 $ ./block_lin
Multiply matrices 10 times...
Average time for N = 1500, BLOCK_SIZE = 10 is 5.864619 seconds
```

### Bonus
Cea de-a doua abordare este cea clasica, folosind matrice alocate ca vectori de
vectori. Pentru o compatie mai relevanta, si ina ceasta abordare se efectueaza
eliminarea constantelor din bulca, in mod similar cu _block\_lin_.

Performantele sunt dupa cum urmeaza:
```
teo@obor Lab5 $ ./block_vect
Multiply matrices 10 times...
Average time for N = 1500, BLOCK_SIZE = 10 is 7.149727 seconds
```

Motivul pentru care si abordarea aceasta inregistreaza un speedup considerabil
fata de programul _basic_ este acela ca daca toate datele pentru un bloc se afla
in cache, nu e prea relevant unde in sunt acestea in RAM, intrucat calculele din
cadrul unui bloc vor opera modifica initial doar datele din cache (acesta fiind
write-back) si doar cand se va trece la urmatorul bloc, datele vor fi scrise in
RAM.
