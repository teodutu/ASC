# Laborator 6
Profiling

## Testare timp de executie (ex 1)
In cele ce urmeaza sunt scrisi timpii de rulare ai algoritmilor secventiali
si paraleli in functie de numarul pana la care se cauta numerele prime.

### Rulare secventiala
- `end = 4000	real	0m0.005s`
- `end = 40000	real	0m0.150s`
- `end = 10000	real	0m0.914s`
- `end = 20000	real	0m2.949s`
- `end = 30000	real	0m6.330s`
- `end = 40000	real	0m11.283s`

Se observa o crestere a timpului de executie cu direct proportionala cu patratul
numarului de numere testate, (din cauza algoritmului ineficient de testare care
testeaza un numar `n` in `O(n)` pasi).

### Rulare paralela
- `end = 4000	real	0m0.011s`
- `end = 40000	real	0m0.075s`
- `end = 10000	real	0m0.159s`
- `end = 20000	real	0m0.393s`
- `end = 30000	real	0m0.669s`
- `end = 40000	real	0m0.721s`

Se observa o scadere mai accentuata a timpilor de rulare cu cat numarul de
numere testate este mai mare, astfel incat pentru `n = 4000`, timpul de rulare
al versiunii paralele este chiar mai mare decat cel secvential, din cauza
overheadului cread de apelurile de sistem necesare pentru deschiderea si
inchiderea threadurilor.

De asemenea, in cazul rularii paralele, afisarea nu se mai efectueaza in ordine
deoarece fiecare thread va afisa ce vrea, cand vrea in acelasi loc (terminal). 


## Oracle Solaris (ex 2)
Urmeaza descrierile toolurilor puse la dispozitie de _Solaris_:

### Functions
Ofera date despre timpii de rulare inclusivi (cu tot cu functiile apelate) si
exclusivi (fara functiile apelate) si dimensiunea in octeti a functiei.

### Timeline
Precizeaza timpii de rulare ai tuturor threadurilor.

### Call Tree
Exemplifica arborere de apeluri de functii, precum si timpii de rulare a
fiecareia dintre acestea pentru a observa zonele de cod care se executa cel
mai mult timp si bottleneckuri.

### Source
Arata timpul de executie al fiecarei instructiune. Cea mai costisitoare
instructiune este cea care executa `n % i` in `TestForPrime()` intrucat se
executa de un numar mare de ori: `O(n)`, iar fiecare apel face tot `O(n)`
operatii, deci in complexitatea totala este `O(n^2)`.

### Disassembly
Identic in functionare cu _Source_, dar are o granularitate mai mica, intrucat
analizeaza individual instructiunile in limbaj de _Assembly_.

### Callers-Callees
Expliciteaza ordinea in care sunt apelate functiile din program, impreuna cu
timpii exclusivi si inclusivi (similar cu _Call Tree_).


## Tachyon raytracing engine (ex 3)
Pentru versiunea paralela timpul de rulare pe CPU este de 10.844s, iar pentru
cea secventiala este de 20.902s. Speedupul relativ mic este cauzat de numarul
mare de afisari + comunicarea prin ssh dintre _fep_ cu PC-ul meu, care scad
paralelismul.

## Testare performante Taychon (ex4)
### Executie seriala
Timp pe CPU: 16.782s

Se observa ca 59.5% din timpul executie se petrece in functia
`render_one_pixel()`, aproximativ 22% din timp este petrecut in functia
`grid_intersect()`, iar 16% in `sphere_intersect()`. Asa cum era de asteptat,
functiile care consuma cel mai mult timp sunt cele care efectueaza prelucrarile
grafice propriu-zise.

### Executie paralela
Timp pe CPU: 9.452s

Cel mai bun speedup se observa la nivelul funcitei `render_one_pixel()`, al
carui timp exclusiv de executie este acum 0! In schimb, timpii de executie sunt
similari in cazul functiilor `grid_intersect()` si `sphere_intersect()`, care
cel mai probabil sunt _IO bound_ si al caror paralelism este slab. Astfel, 
majoritatea speedupului provine din reducerea timpului petrecut in functia
`render_one_pixel()`.

## Valgrind on the Tachyon Code (ex5)
### Implementarea seriala
Asa cum am mentionat deja, cea mai mare parte a timpului de executie este
petrecuta in functia `render_one_pixel()`, in principiu la calcularea operatiei
`iteration_count % 3`, dar si la accesele de memorie.

### Implementarea paralela:
Interpretarea este similara celei de la exercitiul 4.


## Perf (ex6)
```
    25.90%    25.87%  tachyon_find_ho  tachyon_find_hotspots               [.] render_one_pixel
+   13.36%     0.00%  swapper          [kernel.kallsyms]                   [k] 0xffffffff9e6000d5
+   13.36%     0.01%  swapper          [kernel.kallsyms]                   [k] 0xffffffff9e6d7b53
+   11.96%     0.00%  swapper          [kernel.kallsyms]                   [k] 0xffffffff9e65a69b
+   11.84%     0.00%  swapper          [kernel.kallsyms]                   [k] 0xffffffff9e6d790c
+   11.83%     0.00%  swapper          [kernel.kallsyms]                   [k] 0xffffffff9e6d7693
+   11.83%     0.00%  swapper          [kernel.kallsyms]                   [k] 0xffffffff9ee06fa7
+   11.72%    11.70%  tachyon_find_ho  tachyon_find_hotspots               [.] grid_intersect
+   11.35%     0.00%  swapper          [kernel.kallsyms]                   [k] 0xffffffff9ee06cf4
+   11.22%    11.22%  swapper          [kernel.kallsyms]                   [k] 0xffffffff9efbf967
+    8.73%     8.71%  tachyon_find_ho  tachyon_find_hotspots               [.] sphere_intersect
```

La fel ca pana acum, cea mai mare parte din timp (25.9%) dureaza functia
`render_one_pixel()`, alte functii care dureaza mult fiind `grid_intersect()` si
`sphere_intersect()`.
