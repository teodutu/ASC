// TODO
// alocati global un vector vect de n elemente de tip struct particle
// cate elemente puteti aloca maxim (aproximativ)?
//   -> porniti de la 1.000.000 elemente; inmultiti cu 10

#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>     // provides int8_t, uint8_t, int16_t etc.
#include <stdlib.h>

#define NUM_PARTICLES           (10000000)  // cam cat imaginea maxima a unui proces in memorie

struct particle
{
    uint8_t v_x, v_y, v_z;
};

// sizeof(struct particle) = 3;
struct particle vect[NUM_PARTICLES];

int main(int argc, char* argv[])
{
    long n = NUM_PARTICLES;//atol(argv[1]);

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

    return 0;
}
