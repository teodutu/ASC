# Tema 2 - Optimizarea inmultirilor de matrice
Se implementeaza operatia `B * A' + A^2 * B` cu diverse optimizari si se compara
performantele.

## Optimizari generale ale algoritmului de inmultire de matrice
Dat fiind ca matricea `A` este superior triunghiulara, indicii dupa care se va
parcurge aceasta matrice au valori initiale si finale modificate, pentru a nu
efectua operatii inutile (inmultiri cu 0). Aceste optimizari sunt aplicate atat
in varianta optimizata, cat si in cea neoptimizata a rezolvarii.

**Conventie:** Se va folosi simbolul din _Matlab/Octave_ pentru o matrice
transpusa ('), pentru claritate si simplitate.

### BA_t = B * A'
In primul rand, nu are sens transpunerea lui `A`. Parcurgerea coloanelor lui
`A'` este echivalenta cu parcurgerea liniilor lui `A`, in cadrul careia, pe
deasupra, vor avea loc mai multe cache missuri datorita localitatii spatiale a
datelor. Astfel, `B * A'` va deveni `B * A`, in care `A` se parcurge pe linii.

In al doilea rand, pentru a calcula `BA_t[i][j]`, nu este necesara calcularea
intregii sume a produselor elementelor de pe linia `i` a lui `B` cu elementele
de pe linia `j` a lui `A`, deoarece primele `j - 1` elemente ale acesteia din
urma sunt `0`. Este de ajuns sa se faca operatia pentru elementele de pe linia
`j` a lui `A`, **incepand de la coloana j**.

## AA = A^2
Produsul a doua matrice simetrice este tot o matrice simetrica. Asadar, nu ne
intereseaza decat elementele `AA[i][j]`, cu `j >= i`. In mod similar optimizarii
de la inmultirea `B * A'`, suma de produse dintre elementele de pe linii si de
pe coloane ale matricei, va incepe la indicele `i`.

## AAB = A^2 * B = AA * B
Matricea `AA` a fost explicata mai sus. Optimizarea de indici in cadrul
calcularii `AAB[i][j]` se foloseste tot de faptul ca `AA` este superior
triunghiulara si este foarte similara celei folosite pentru calcularea unui
element din `BA_t[i][j]`: suma va incepe de la coloana `i`, intrucat
`AA[i][j] = 0`, pentru `j < i`.

## neopt
Metoda aplica optimizarile generala de mai sus, care tin cont in mare parte doar
de forma matricei `A` si obtine urmatoarele performante.

### Timpi de rulare ibm-nehalem.q
```
Run=./tema2_neopt: N=400: Time=0.678856
Run=./tema2_neopt: N=600: Time=2.132154
Run=./tema2_neopt: N=800: Time=5.137768
Run=./tema2_neopt: N=1000: Time=9.706286
Run=./tema2_neopt: N=1200: Time=16.770065
Run=./tema2_neopt: N=1400: Time=27.576996
Run=./tema2_neopt: N=1600: Time=47.555859
```

## opt_blas
De departe cea mai spectaculoasa si surprinzatoare metoda (fapt evidentiat de
timpii de rulare extrem de mici), implementarea se bazeaza pe optimizarile pe
care biblioteca _BLAS_ le face la nivelul functiei sale, `cblas_dtrmm()`[0],
care inmulteste o matrice triunghiulara (`A` in cazul de fata) cu un scalar (`1`
in acest caz) si apoi inmulteste rezultatul cu alta matrice (`B` sau `A`). Dupa
ce se calculeaza `B * A'` si `A^2 * B`, aceste rezultate sunt adunate "de mana",
intrucat _BLAS_ nu pune la dispozitie o functie care face acest lucru :(.

**Mentiune:** pe cat de performanta este biblioteca, pe atat de neintuitiva este
conventia de nume pe care o foloseste. Mi-a luat ceva sa ma prind ca `dtrmm`
provine de la **D**ouble **Tr**iangular **M**atrix **M**ultiply...

### Timpi de rulare ibm-nehalem.q
```
Run=./tema2_blas: N=400: Time=0.028394
Run=./tema2_blas: N=600: Time=0.088466
Run=./tema2_blas: N=800: Time=0.202133
Run=./tema2_blas: N=1000: Time=0.386675
Run=./tema2_blas: N=1200: Time=0.656173
Run=./tema2_blas: N=1400: Time=1.031261
Run=./tema2_blas: N=1600: Time=1.524863
```

## opt_f
Aceasta implementare este cea neoptimizata, compilata cu flagul `-O3`. Pentru
comparatie cu `opt_f_extra`, am notat timpii acestei metode:

### Timpi de rulare ibm-nehalem.q
```
Run=./tema2_opt_f: N=400: Time=0.148869
Run=./tema2_opt_f: N=600: Time=0.324037
Run=./tema2_opt_f: N=800: Time=0.828579
Run=./tema2_opt_f: N=1000: Time=1.563727
Run=./tema2_opt_f: N=1200: Time=2.821983
Run=./tema2_opt_f: N=1400: Time=4.659523
Run=./tema2_opt_f: N=1600: Time=8.720551
```

## opt_f_extra
Am plecat de la flagul `-ffast-math`. Folosind documentatoa gcc-ului[1], am
"descompus" aceasta grupare de flaguri in flagurile care o compun. Dintre
acestea, le-am ales pe acelea care influenteaza problema curenta:

### -fno-signed-zeros
Conform standardului _IEEE_, exista `-0.0` si `+0.0` si se comporta diferit in
calcule. Daca se adauga acest flag, semnul lui `0.0` nu mai este relevant, iar
pentru problema noastra, nu este relevant.

### -ffinite-math-only
Presune ca nu se vor face operatii cu valori `NaN` si/sau `+/-Inf` si permite
optimizarea acestora in consecinta. Cum aceste valori nu se vor intalni printre
rezultatele obtinute (se garanteaza ca datele sunt corecte), se poate face
optimizarea.

### -fno-signaling-nans -fno-trapping-math
Intrucat nu se fac operatii de impartire, iar datele sunt valide, nu ne
intereseaza erorile, iar aceste flaguri fac ca aceste erori sa nu mai fie luate
in considerare, eliminand astfel overheadul necesar verificarilor.

### -fassociative-math
Se permite reordonarea calculelor si a operanzilor in vederea imbunatatirii
timpului de executie.

### -fexcess-precision=fast
Aceasta optimizare permite efectuarea calculelor intr-o precizie mai mare decat
cea a datelor insesi, daca acest lucru duce la o performanta marita.

### -mfpmath=sse
Intrucat procesorul pe care se ruleaza este pe 64 de biti, acest flag activeaza
extensia SSE si face procesorul sa efectueze calculele folosind aceste
instructiuni noi (_Streaming SIMD Extensionss_) pentru calculele in virgula
mobila. Aceasta extensie este utila pentru problema curenta, deoarece operatia
de inmultire de matrice se preteaza unor sisteme (sau extensii) SIMD.

Prin urmare, timpii obtinuti in urma compilarii cu aceste flaguri sunt **cu
aproximativ 16% mai buni** decat cei obtinuti doar cu flagul `-O3`:
```
Run=./tema2_opt_f_extra: N=400: Time=0.133441
Run=./tema2_opt_f_extra: N=600: Time=0.343231
Run=./tema2_opt_f_extra: N=800: Time=0.698895
Run=./tema2_opt_f_extra: N=1000: Time=1.315393
Run=./tema2_opt_f_extra: N=1200: Time=2.355023
Run=./tema2_opt_f_extra: N=1400: Time=4.012690
Run=./tema2_opt_f_extra: N=1600: Time=7.574487
```

## opt_m
S-au folosit 3 tipuri de optimizari.

### Fortarea localitatii spatiale
In cadrul algoritmului clasic de inmultire de matrice, operantul din dreapta
este parcurs pe coloane, ceea ce nu confera algoritmului localitate spatiala,
ceea ce va rezulta intr-un numar mare de cache missuri. Solutia, asa cum am
mentionat si la inceputul README-ului, este sa transpunem termenul drept si sa-l
parcurgem aceasta matrice transpusa pe linii. Acest lucru s-a facut pentru
inmultirile `B * A' <=> B * A(pe linii)`, `A* A <=> A * A'(pe linii)` si
`A^2 * B <=> A^2 * B'(pe linii)`.

### Eliminarea constantelor din bucle
In mod similar implementarilor din _Laboratorul 5_[2], s-a renuntat la calculul
indecsilor de tipul `i * N + j` prin folosire apointerilor, care sunt
incrementati pentru fiecare pozitie din matrice sau pentru fiecare termen din
suma, astfel scazand numarul total de operaii (nu in virgula mobila) facute de
compilator.

Aceasta procedura s-a aplicat tuturor celor 3 inmultiri de matrice (`B * A'`,
`A * A` si `(A * A) * B`), cat si pentru transpunerile matricelor `A` si `B`
descrise mai sus.

### Folosirea registrelor procesorului
Toate datele necesare unei inmultiri de matrice (indicii pentru pozitiile din
matrice ale operanzilor, pointerii mentionati mai sus) sunt retinute direct in
registrele procesorului pentru a se elimina overheadurile de acces la memorie
(fie ea si cache) pentru aceste variabile. Intrucat sistemul de pe
_ibm-nehalem.q_ este pe 64 de biti, numarul de registre este suficient de mare
pentru a nu aparea intarzieri din cauza comutarilor intre acestea.

Astfel, se obtin urmatorii timpi de executie:
```
Run=./tema2_opt_m: N=400: Time=0.175844
Run=./tema2_opt_m: N=600: Time=0.423902
Run=./tema2_opt_m: N=800: Time=0.996612
Run=./tema2_opt_m: N=1000: Time=1.930095
Run=./tema2_opt_m: N=1200: Time=3.338577
Run=./tema2_opt_m: N=1400: Time=5.412837
Run=./tema2_opt_m: N=1600: Time=8.424620
```

## Bibliografie
- [0] https://developer.apple.com/documentation/accelerate/1513132-cblas_dtrmm?language=objc
- [1] https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html
- [2] https://ocw.cs.pub.ro/courses/asc/laboratoare/05#detectarea_constantelor_din_bucle
