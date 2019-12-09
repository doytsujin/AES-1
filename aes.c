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

#define IND(row, column) ((column * BLOCK_SIZE) + row)
#define MODULUS 0x11

static const uint8_t subBytesSBox[16*16] = {
    0x63, 0xca, 0xb7, 0x04, 0x09, 0x53, 0xd0, 0x51, 0xcd, 0x60, 0xe0, 0xe7, 0xba, 0x70, 0xe1, 0x8c,
    0x7c, 0x82, 0xfd, 0xc7, 0x83, 0xd1, 0xef, 0xa3, 0x0c, 0x81, 0x32, 0xc8, 0x78, 0x3e, 0xf8, 0xa1,
    0x77, 0xc9, 0x93, 0x23, 0x2c, 0x00, 0xaa, 0x40, 0x13, 0x4f, 0x3a, 0x37, 0x25, 0xb5, 0x98, 0x89,
    0x7b, 0x7d, 0x26, 0xc3, 0x1a, 0xed, 0xfb, 0x8f, 0xec, 0xdc, 0x0a, 0x6d, 0x2e, 0x66, 0x11, 0x0d,
    0xf2, 0xfa, 0x36, 0x18, 0x1b, 0x20, 0x43, 0x92, 0x5f, 0x22, 0x49, 0x8d, 0x1c, 0x48, 0x69, 0xbf,
    0x6b, 0x59, 0x3f, 0x96, 0x6e, 0xfc, 0x4d, 0x9d, 0x97, 0x2a, 0x06, 0xd5, 0xa6, 0x03, 0xd9, 0xe6,
    0x6f, 0x47, 0xf7, 0x05, 0x5a, 0xb1, 0x33, 0x38, 0x44, 0x90, 0x24, 0x4e, 0xb4, 0xf6, 0x8e, 0x42,
    0xc5, 0xf0, 0xcc, 0x9a, 0xa0, 0x5b, 0x85, 0xf5, 0x17, 0x88, 0x5c, 0xa9, 0xc6, 0x0e, 0x94, 0x68,
    0x30, 0xad, 0x34, 0x07, 0x52, 0x6a, 0x45, 0xbc, 0xc4, 0x46, 0xc2, 0x6c, 0xe8, 0x61, 0x9b, 0x41,
    0x01, 0xd4, 0xa5, 0x12, 0x3b, 0xcb, 0xf9, 0xb6, 0xa7, 0xee, 0xd3, 0x56, 0xdd, 0x35, 0x1e, 0x99,
    0x67, 0xa2, 0xe5, 0x80, 0xd6, 0xbe, 0x02, 0xda, 0x7e, 0xb8, 0xac, 0xf4, 0x74, 0x57, 0x87, 0x2d,
    0x2b, 0xaf, 0xf1, 0xe2, 0xb3, 0x39, 0x7f, 0x21, 0x3d, 0x14, 0x62, 0xea, 0x1f, 0xb9, 0xe9, 0x0f,
    0xfe, 0x9c, 0x71, 0xeb, 0x29, 0x4a, 0x50, 0x10, 0x64, 0xde, 0x91, 0x65, 0x4b, 0x86, 0xce, 0xb0,
    0xd7, 0xa4, 0xd8, 0x27, 0xe3, 0x4c, 0x3c, 0xff, 0x5d, 0x5e, 0x95, 0x7a, 0xbd, 0xc1, 0x55, 0x54,
    0xab, 0x72, 0x31, 0xb2, 0x2f, 0x58, 0x9f, 0xf3, 0x19, 0x0b, 0xe4, 0xae, 0x8b, 0x1d, 0x28, 0xbb,
    0x76, 0xc0, 0x15, 0x75, 0x84, 0xcf, 0xa8, 0xd2, 0x73, 0xdb, 0x79, 0x08, 0x8a, 0x9e, 0xdf, 0x16
};

void cipher(aes_state * state) {
    addRoundKey(state, 0);

    int i;
    for (i = 1; i < NUM_ROUNDS; i++) {
        subBytes(state->buffer);
        shiftRows(state->buffer);
        mixColumns(state->buffer);
        addRoundKey(state, i);
    }

    subBytes(state->buffer);
    shiftRows(state->buffer);
    addRoundKey(state, NUM_ROUNDS);
}

uint8_t mult(uint8_t n, uint8_t m) {
    uint16_t mul = (uint16_t)n;
    uint16_t res = 0;
    for(; m > 0; m >>= 1) {
        if (m & 0x1) {
            res |= mul;
        }
        mul <<= 1;
    }


    uint16_t shifted_modulus = MODULUS << 11;
    uint16_t test_bit = 0x8000;
    while (res > MODULUS) {
        if (test_bit & res) {
            res ^= shifted_modulus;
        }
        test_bit >>= 1;
        shifted_modulus >>= 1;
    }

    return (uint8_t)res;
}

void subBytes(uint32_t * buffer) {
    uint8_t * byte_buffer = (uint8_t *)buffer;
    int i;
    for (i = 0; i < 4 * BLOCK_SIZE; i++){
        byte_buffer[i] = subBytesSBox[byte_buffer[i]];
    }
}

void shiftRows(uint32_t * buffer) {
    uint8_t * byte_buffer = (uint8_t *)buffer;
    uint8_t temp_bytes[BLOCK_SIZE];

    int row, column;
    for (row = 1; row < 4; row++) {
        for (column = 0; column < BLOCK_SIZE; column++){
            temp_bytes[(column + (BLOCK_SIZE - row)) % BLOCK_SIZE] = byte_buffer[IND(row, column)];
        }
        for (column = 0; column < BLOCK_SIZE; column++) {
            byte_buffer[IND(row, column)] = temp_bytes[column];
        }
    }
}

void mixColumns(uint32_t * buffer) {
    uint8_t * byte_buffer = (uint8_t *)buffer;
    int column;
    for (column = 0; column < BLOCK_SIZE; column++) {
        byte_buffer[4 * column] = mult(0x02, byte_buffer[4 * column]) ^  mult(0x03, byte_buffer[(4 * column) + 1]) ^ byte_buffer[(4 * column) + 2] ^ byte_buffer[(4 * column) + 3];
        byte_buffer[(4 * column) + 1] = byte_buffer[4 * column] ^ mult(0x02, byte_buffer[(4 * column) + 1]) ^ mult(0x03, byte_buffer[(4 * column) + 2]) ^ byte_buffer[(4 * column) + 3];
        byte_buffer[(4 * column) + 2] = byte_buffer[4 * column] ^ byte_buffer[(4 * column) + 1] ^ mult(0x02, byte_buffer[(4 * column) + 2]) ^ mult(0x03, byte_buffer[(4 * column) + 3]);
        byte_buffer[(4 * column) + 3] = mult(0x03, byte_buffer[4 * column]) ^ byte_buffer[(4 * column) + 1] ^ byte_buffer[(4 * column) + 2] ^ mult(0x02, byte_buffer[(4 * column) + 3]);
    }
}

void addRoundKey(aes_state * state, int round) {
    int i;
    for (i = 0; i < BLOCK_SIZE; i++) {
        state->buffer[i] ^= state->key_schedule[(round * BLOCK_SIZE) + i];
    }
}