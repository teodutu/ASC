# Laborator 5
Optimizarea operatiei de inmultire de matrice

## basic
Toate rezultatele vor fi comparate cu timpul de rulare al algoritmului clasic:
```
teo@obor Lab5 $ ./basic
Time for N = 1500 is 19.592505 seconds.
```

## const_elim
Prima optimizare presupune eliminarea constantelor precum `C[i][j]` din bucle
prin inlocuire cu pointeri. Imbunatatirea se poate vedea:
```
teo@obor Lab5 $ ./const_elim
Time for N = 1500 is 11.513561 seconds.
```

## loop_reorder
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
Am analizat doua abordari. In prima matricele au fost liniarizate, iar speedupul
este urmatorul pentru o dimensiune relativ optima a blocului:
```
teo@obor Lab5 $ ./block_lin 1500 25 1
Multiply matrices 1 times...
Time = 14.317692 seconds
```

### Bonus
Cea de-a doua abordare este cea clasica, folosind matrice alocate ca vectori de
vectori. Performantele sunt dupa cum urmeaza:
```
teo@obor Lab5 $ ./block_classic 1500 25 1
Multiply matrices 1 times...
Time = 14.993494 seconds
```

Motivul pentru care diferentele de timp sunt mici este acela ca daca datele
pentru un bloc se afla in cache, nu e prea relevant unde in memorie sunt acestea
in RAM, intrucat calculele din cadrul unui bloc vor opera modifica initial doar
datele din cache (acesta fiind write-back) si doar cand se va trece la urmatorul
bloc, datele vor fi scrise in RAM.