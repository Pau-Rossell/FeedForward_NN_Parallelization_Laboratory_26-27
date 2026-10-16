/*
 * randomizer.c
 *
 * Created: 2-11-2024
 * by: Christian Germer
 *
 *
 * funcions de aleatorietat de la xarxa Neuronal
 */

#include "randomizer.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//-----------RANDOM FUNCTIONS------------
int rando(void) {
    uint32_t next = UINT32_C(214013) * (uint32_t)seed
                  + UINT32_C(2531011);

    seed = next <= INT32_MAX
        ? (int)next
        : (int)((int64_t)next - INT64_C(4294967296));

    int upper = (int)(next >> 16);
    return upper < 32768 ? upper : upper - 65536;
}

float random_between_two(float min, float max) {
    return ((max - min) * ((float)rand() / RAND_MAX)) + min;
}
