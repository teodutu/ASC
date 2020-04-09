# Laborator 8
Inmultire paralela de matrice in _CUDA_

## Exercitiul 1
Executand 14000 de operatii intr-un kernel, se observa ca fiecare numarul de
_GFLOPS_ tinde sa se stabilizeze in jurul valorii de 440 - 450.

## Exercitiul 2
```
[teodor_stefan.dutu@hpsl-wn02 Lab8]$ ./matrix_multiplication
Timing simple implementation... done.
Timing optimized implementation... done.
Matrix size: 1024x1024
Tile size: 16x16
Throughput of simple kernel: 96.9487 GFLOPS
Throughput of optimized kernel: 283.351 GFLOPS
Performance improvement: 2.92269x
```

## Profiling
Ruland `nvpp` se observa ca 25% din timp este petrecut in kernelurile
`matrix_multiply`, pe cand 75% este petrecut in `matrix_multiply_simple`,
lucru evidentiat si de speedupul calculat mai sus.

`nvprof` afiseaza acelasi raport de timp intre cele doua kerneluri. Pe deasupra,
acesta calculeaza si timpii medii pentru fiecare "apel" al fiecarui kernel:
- `22.162ms` pentru `matrix_multiply_simple`

- `7.5740ms` pentru `matrix_multiply`

Urmatoarele apeluri ca timp de executie (dupa cele de `cudaEventSynchronize`,
in care este cuprins timpul de inmultire efectiva a matricelor), sunt cele de
`cudaMalloc`, care dureaza in total `470.12ms`, adica aproximativ 13.5% din
timpul total de rulare.
