# Laborator 9
Toate rularile au fost efectuate, dupa cum se vede, pe coada `hp-sl.q`.

## Task 1
Ruland `nvpropf` pe implemenatare care foloseste `cudaMalloc` si `cudaMemcpy`
obtine urmatoarele rezultate:
```
[teodor_stefan.dutu@hpsl-wn03 Lab9]$ nvprof ./task1_mem_plain 
==38169== NVPROF is profiling process 38169, command: ./task1_mem_plain
==38169== Profiling application: ./task1_mem_plain
==38169== Profiling result:
            Type  Time(%)      Time     Calls       Avg       Min       Max  Name
 GPU activities:   57.50%  9.0416ms         1  9.0416ms  9.0416ms  9.0416ms  [CUDA memcpy DtoH]
                   35.70%  5.6141ms         1  5.6141ms  5.6141ms  5.6141ms  [CUDA memcpy HtoD]
                    6.80%  1.0688ms         1  1.0688ms  1.0688ms  1.0688ms  kernel_compute(float*, float*, int)
      API calls:   93.98%  418.07ms         2  209.03ms  346.16us  417.72ms  cudaMalloc
                    3.80%  16.917ms         2  8.4584ms  5.9026ms  11.014ms  cudaMemcpy
                    1.55%  6.9019ms         2  3.4509ms  339.54us  6.5623ms  cudaFree
                    0.36%  1.5922ms       188  8.4680us     332ns  322.20us  cuDeviceGetAttribute
                    0.19%  849.61us         2  424.80us  424.05us  425.56us  cuDeviceTotalMem
                    0.07%  319.36us         1  319.36us  319.36us  319.36us  cudaLaunch
                    0.03%  150.93us         2  75.463us  72.655us  78.271us  cuDeviceGetName
                    0.00%  14.673us         3  4.8910us     239ns  13.000us  cudaSetupArgument
                    0.00%  6.3880us         4  1.5970us     457ns  4.0920us  cuDeviceGet
                    0.00%  4.3410us         3  1.4470us     567ns  2.6280us  cuDeviceGetCount
                    0.00%  2.7740us         1  2.7740us  2.7740us  2.7740us  cudaConfigureCall
```

Acelasi profiler rulat pe implementarea care utilizeaza `cudaMallocManaged`
arata astfel:
```
[teodor_stefan.dutu@hpsl-wn03 Lab9]$ nvprof ./task1_mem_unified 
==38184== NVPROF is profiling process 38184, command: ./task1_mem_unified
==38184== Profiling application: ./task1_mem_unified
==38184== Profiling result:
            Type  Time(%)      Time     Calls       Avg       Min       Max  Name
 GPU activities:  100.00%  1.0693ms         1  1.0693ms  1.0693ms  1.0693ms  kernel_compute(float*, float*, int)
      API calls:   93.83%  452.49ms         2  226.24ms  7.1030ms  445.39ms  cudaMallocManaged
                    3.23%  15.575ms         2  7.7876ms  7.3086ms  8.2666ms  cudaFree
                    2.40%  11.555ms         1  11.555ms  11.555ms  11.555ms  cudaLaunch
                    0.33%  1.6072ms       188  8.5490us     338ns  322.82us  cuDeviceGetAttribute
                    0.17%  838.20us         2  419.10us  415.94us  422.27us  cuDeviceTotalMem
                    0.03%  152.17us         2  76.085us  74.174us  77.997us  cuDeviceGetName
                    0.00%  9.5710us         3  3.1900us     256ns  8.3830us  cudaSetupArgument
                    0.00%  5.9580us         4  1.4890us     431ns  3.8820us  cuDeviceGet
                    0.00%  4.1050us         3  1.3680us     427ns  2.6480us  cuDeviceGetCount
                    0.00%  3.1890us         1  3.1890us  3.1890us  3.1890us  cudaConfigureCall
```

Se observa cum cu prima abordare, GPU-ul pierde timp cu operatii copiere in si
din RAM, ceea ce nu se intampla in cadrul celei de-a doua abordari. Data fiind
complexitatea crescuta a functionalitatii functiei `cudaMallocManaged`,
aceasta dureaza ceva mai mult decat `cudaMalloc`, dar imbunatatirea esentiala
este ca folosind-o, procesorul trebuie sa execute doar kernelul, nu si altceva,
ceea ce ii creste performanta (daca am masura-o in _GLFLOPS_ de exemplu).

## Task 2
Ruland varianta pratic secventiala in care un singur thread calculeaza tot,
rezulta urmatorii timpi:, executia kernelului durand 9s.
```
[teodor_stefan.dutu@hpsl-wn03 Lab9]$ nvprof ./task2_no_atomic
==38875== NVPROF is profiling process 38875, command: ./task2_no_atomic
SUM: 535968646
MAX: 1023
MIN: 0
==38875== Profiling application: ./task2_no_atomic
==38875== Profiling result:
            Type  Time(%)      Time     Calls       Avg       Min       Max  Name
 GPU activities:  100.00%  9.01647s        10  901.65ms  901.64ms  901.66ms  kernel_no_atomics(int*, int*)
      API calls:   94.60%  9.01670s        10  901.67ms  901.65ms  901.73ms  cudaDeviceSynchronize
                    5.35%  509.77ms         2  254.89ms  237.68us  509.53ms  cudaMallocManaged
                    0.02%  1.8611ms        10  186.11us  62.276us  1.1524ms  cudaLaunch
                    0.02%  1.5233ms       188  8.1020us     266ns  311.35us  cuDeviceGetAttribute
                    0.01%  890.90us         2  445.45us  443.62us  447.27us  cuDeviceTotalMem
                    0.00%  339.15us        10  33.914us     709ns  321.07us  cudaConfigureCall
                    0.00%  139.52us         2  69.758us  69.440us  70.077us  cuDeviceGetName
                    0.00%  120.53us         1  120.53us  120.53us  120.53us  cudaFree
                    0.00%  16.008us        20     800ns     128ns  7.9020us  cudaSetupArgument
                    0.00%  6.8150us         4  1.7030us     447ns  4.9350us  cuDeviceGet
                    0.00%  4.6880us         3  1.5620us     561ns  2.9470us  cuDeviceGetCount
```

Varianta partiala (in care doar primul thread al fiecarui bloc face operatii)
are performante mult mai bune (durata a kernelului de ~4ms):
```
[teodor_stefan.dutu@hpsl-wn03 Lab9]$ nvprof ./task2_partial_atomic 
==38888== NVPROF is profiling process 38888, command: ./task2_partial_atomic
SUM: 535968646
MAX: 1023
MIN: 0
==38888== Profiling application: ./task2_partial_atomic
==38888== Profiling result:
            Type  Time(%)      Time     Calls       Avg       Min       Max  Name
 GPU activities:  100.00%  4.0338ms        10  403.38us  402.86us  404.65us  kernel_partial_atomics(int*, int*)
      API calls:   98.15%  465.11ms         2  232.56ms  270.46us  464.84ms  cudaMallocManaged
                    0.87%  4.1057ms        10  410.57us  408.43us  415.77us  cudaDeviceSynchronize
                    0.44%  2.0699ms        10  206.99us  91.022us  1.2220ms  cudaLaunch
                    0.27%  1.2615ms       188  6.7090us     262ns  252.81us  cuDeviceGetAttribute
                    0.21%  1.0043ms         2  502.15us  329.17us  675.12us  cuDeviceTotalMem
                    0.03%  134.93us         2  67.466us  54.122us  80.811us  cuDeviceGetName
                    0.03%  133.66us         1  133.66us  133.66us  133.66us  cudaFree
                    0.00%  17.359us        20     867ns     217ns  9.2990us  cudaSetupArgument
                    0.00%  13.503us        10  1.3500us  1.0200us  3.8680us  cudaConfigureCall
                    0.00%  4.6320us         4  1.1580us     335ns  2.9950us  cuDeviceGet
                    0.00%  3.7570us         3  1.2520us     269ns  2.6900us  cuDeviceGetCount
```
Surprinzatoare sunt performantele rularii complet paralele (fiecare thread se
ocupa de operatii cu un singur numar). Surpriza provine din faptul ca operatiile
(adunare, maxim, minim) se fac practic fiecare secvential din cauza
sincronizarii, ceea ce ar trebui sa duca la timpi mai mari decat cei ai
variantei partiale. M-am gandit la 4 posibile explicatii:
1. In cadrul kernelului partial, se fac mai multe operatii decat in cadrul celui
complet paralel, ceea ce creeaza un oarecare overhead
2. Placa grafica stie sa gestioneze eficient operatiile atomice si le
reordoneaza in functie de ce operatie poate fi facuta la un moment dat
3. Magie neagra
4. Combinatii intre cele de mai sus
```
[teodor_stefan.dutu@hpsl-wn03 Lab9]$ nvprof ./task2_full_atomic 
==41109== NVPROF is profiling process 41109, command: ./task2_full_atomic
SUM: 535968646
MAX: 1023
MIN: 0
==41109== Profiling application: ./task2_full_atomic
==41109== Profiling result:
            Type  Time(%)      Time     Calls       Avg       Min       Max  Name
 GPU activities:  100.00%  1.6638ms        10  166.38us  166.15us  167.62us  kernel_full_atomics(int*, int*)
      API calls:   98.52%  400.22ms         2  200.11ms  235.25us  399.99ms  cudaMallocManaged
                    0.43%  1.7330ms        10  173.30us  69.462us  1.0897ms  cudaLaunch
                    0.42%  1.7170ms        10  171.70us  170.33us  175.09us  cudaDeviceSynchronize
                    0.34%  1.3843ms       188  7.3630us     214ns  273.93us  cuDeviceGetAttribute
                    0.22%  895.68us         2  447.84us  445.06us  450.62us  cuDeviceTotalMem
                    0.03%  124.64us         2  62.319us  60.676us  63.962us  cuDeviceGetName
                    0.03%  112.89us         1  112.89us  112.89us  112.89us  cudaFree
                    0.00%  16.309us        20     815ns     239ns  8.8310us  cudaSetupArgument
                    0.00%  10.196us        10  1.0190us     736ns  3.0400us  cudaConfigureCall
                    0.00%  5.4890us         4  1.3720us     447ns  3.8670us  cuDeviceGet
                    0.00%  4.4480us         3  1.4820us     407ns  2.8320us  cuDeviceGetCount
```

## Task 3
Scrierea in baza de date functioneaza dupa urmatorul algoritm (destul de cretin):
1. se scrie valoarea la prima pozitie folosind `atomicExch`
2. daca valoarea veche nu este 0 (exista deja o valoare pe pozita respecitva),
aceasta se pune la loc, tot cu `atomicExch` si se reia algoritmul de la pasul 1

## Task 4
In `main` se creeaza `UM_ELEMS` masteri, cate unul pentru fiecare element din
`result`. Fiecare master va crea `data[threadIdx.x]` workeri, adica atatia cat
este elementul din `data` care ii revine masterului, astfel incat fiecarui
worker ii revine cate o pozitie din `data` de la 0 la `threadIdx.x`.

Fiecare worker adauga atomic (cu `atomicAdd`) valoarea din `data` de la indexul
sau la cea din `result` a masterului care l-a creat.
