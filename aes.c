/* aes.c
 * Written by: Austin Bohannon
 * Copyright: 2019
 * License: MPL 2.0
 *
 * This file implements the functions defined in aes.h. See that file for more
 * information on this library.
 */

#include <stdint.h>
#include "aes.h"

void cipher(aes_state * state) {
    addRoundKey(state->buffer);

    int i;
    for (i = 0; i < NUM_ROUNDS; i++) {
        subBytes(state->buffer);
        shiftRows(state->buffer);
        mixColumns(state->buffer);
        addRoundKey(state->buffer);
    }

    subBytes(state->buffer);
    shiftRows(state->buffer);
    addRoundKey(state->buffer);
}

void subBytes(uint32_t * buffer) {
    return;
}

void shiftRows(uint32_t * buffer) {
    return;
}

void mixColumns(uint32_t * buffer) {
    return;
}

void addRoundKey(uint32_t * buffer) {
    return;
}
