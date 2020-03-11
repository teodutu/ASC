#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>     // provides int8_t, uint8_t, int16_t etc.
#include <stdlib.h>

#include "utils.h"

struct particle
{
    int8_t v_x, v_y, v_z;
};

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        printf("apelati cu %s <n>\n", argv[0]);
        return -1;
    }

    long n = atol(argv[1]);
    long n_aux;
    struct particle* vect;
    int8_t* aux;

    // TODO
    // alocati dinamic o matrice de n x n elemente de tip struct particle
    // verificati daca operatia a reusit
    vect = malloc(n * n * sizeof(*vect));
    DIE(vect == NULL, "malloc vect");

    // TODO
    // populati matricea alocata astfel:
    // *liniile pare contin particule cu toate componentele vitezei pozitive
    //   -> folositi modulo 128 pentru a limita rezultatului lui rand()
    // *liniile impare contin particule cu toate componentele vitezi negative
    //   -> folositi modulo 129 pentru a limita rezultatului lui rand()
    for (long i = 0; i != n; ++i)
    {
        for (long j = 0; j != n; ++j)
        {
            if (i & 1)
            {
                vect[n * i + j].v_x = (int8_t)(rand() % 129);
                vect[n * i + j].v_y = (int8_t)(rand() % 129);
                vect[n * i + j].v_z = (int8_t)(rand() % 129);
            } else
            {
                vect[n * i + j].v_x = (int8_t)(rand() % 128);
                vect[n * i + j].v_y = (int8_t)(rand() % 128);
                vect[n * i + j].v_z = (int8_t)(rand() % 128);
            }
        }
    }

    // TODO
    // scalati vitezele tuturor particulelor cu 0.5
    //   -> folositi un cast la int8_t* pentru a parcurge vitezele fara
    //      a fi nevoie sa accesati individual componentele v_x, v_y, si v_z
    n_aux = (n << 1) + n;
    aux = (int8_t*)vect;

    for (long i = 0; i != n; ++i)
    {
        for (long j = 0; j != n_aux; ++j)
        {
            aux[n_aux * i + j] >>= 1;
        }
    }

    // compute max particle speed
    float max_speed = 0.0f;
    for(long i = 0; i < n * n; ++i)
    {
        float speed = sqrt(vect[i].v_x * vect[i].v_x +
                           vect[i].v_y * vect[i].v_y +
                           vect[i].v_z * vect[i].v_z);
        if(max_speed < speed) max_speed = speed;
    }

    // print result
    printf("viteza maxima este: %f\n", max_speed);

    free(vect);

    return 0;
}

