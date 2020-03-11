// TODO
// alocati dinamic un vector vect de n elemente de tip struct particle
// verificati daca operatia a reusit
// cate elemente puteti aloca maxim (aproximativ)?
//   -> porniti de la 1.000.000 elemente; inmultiti cu 10

#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>     // provides int8_t, uint8_t, int16_t etc.
#include <stdlib.h>

#define NUM_PARTICLES           (3900000000) // un ordin de marime de ~12.5GB

struct particle
{
    uint8_t v_x, v_y, v_z;
};

int main(int argc, char* argv[])
{
    struct particle *vect = malloc(NUM_PARTICLES * sizeof(*vect));
    if (vect == NULL)
    {
        printf("unable to allocate %llu particles\n", NUM_PARTICLES);
        return -1;
    }

    long n = NUM_PARTICLES;

    // generate some particles
    for(long i = 0; i < n; i += (5 * (1 << 20)))
    {
        vect[i].v_x = (uint8_t)rand();
        vect[i].v_y = (uint8_t)rand();
        vect[i].v_z = (uint8_t)rand();
    }

    // compute max particle speed
    float max_speed = 0.0f;
    for(long i = 0; i < n; i += (5 * (1 << 20)))
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

