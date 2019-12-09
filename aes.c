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
#define MODULUS 0x011b

static const uint32_t rcon[10] = {
    0x01000000, 0x02000000, 0x04000000, 0x08000000,
    0x10000000, 0x20000000, 0x40000000, 0x80000000,
    0x1b000000, 0x36000000
};

static const uint8_t subBytesSBox[16*16] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
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

void keyExpansion(aes_state * state, const uint32_t * cipher_key) {
    int i;
    for (i = 0; i < KEY_LENGTH; i++) {
        state->key_schedule[i] = cipher_key[i];
    }

    for (; i < BLOCK_SIZE * (NUM_ROUNDS + 1); i++) {
        uint32_t temp = state->key_schedule[i - 1];
        if (i % KEY_LENGTH == 0) {
            temp = subWord(rotWord(temp)) ^ rcon[(i - KEY_LENGTH) / KEY_LENGTH];
        }
#if KEY_LENGTH==8
        else if (i % KEY_LENGTH == 4) {
            temp = subWord(temp);
        }
#endif
        state->key_schedule[i] = state->key_schedule[i - KEY_LENGTH] ^ temp;
    }
}

uint8_t mult(uint8_t n, uint8_t m) {
    /* polynomial multiplication */
    uint16_t mul = (uint16_t)n;
    uint16_t res = 0;
    for(; m > 0; m >>= 1) {
        if (m & 0x1) {
            res ^= mul;
        }
        mul <<= 1;
    }

    /* polynomial division, returning the remainder */
    uint16_t shifted_modulus = MODULUS << 7;
    uint16_t test_bit = 0x8000;
    while (res >= 0x0100) {
        if (test_bit & res) {
            res ^= shifted_modulus;
        }
        test_bit >>= 1;
        shifted_modulus >>= 1;
    }

    return (uint8_t)res;
}

uint32_t subWord(uint32_t w) {
    uint8_t * byte_buffer = (uint8_t *)&w;
    int i;
    for (i = 0; i < 4; i++){
        byte_buffer[i] = subBytesSBox[byte_buffer[i]];
    }

    return w;
}

uint32_t rotWord(uint32_t w) {
    uint8_t * byte_buffer = (uint8_t *)&w;

    uint8_t last_byte = byte_buffer[3];
    byte_buffer[3] = byte_buffer[2];
    byte_buffer[2] = byte_buffer[1];
    byte_buffer[1] = byte_buffer[0];
    byte_buffer[0] = last_byte;

    return w;
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
    uint8_t temp_bytes[4];

    int column;
    for (column = 0; column < BLOCK_SIZE; column++) {
        temp_bytes[0] = mult(0x02, byte_buffer[4 * column]) ^  mult(0x03, byte_buffer[(4 * column) + 1]) ^ byte_buffer[(4 * column) + 2] ^ byte_buffer[(4 * column) + 3];
        temp_bytes[1] = byte_buffer[4 * column] ^ mult(0x02, byte_buffer[(4 * column) + 1]) ^ mult(0x03, byte_buffer[(4 * column) + 2]) ^ byte_buffer[(4 * column) + 3];
        temp_bytes[2] = byte_buffer[4 * column] ^ byte_buffer[(4 * column) + 1] ^ mult(0x02, byte_buffer[(4 * column) + 2]) ^ mult(0x03, byte_buffer[(4 * column) + 3]);
        temp_bytes[3] = mult(0x03, byte_buffer[4 * column]) ^ byte_buffer[(4 * column) + 1] ^ byte_buffer[(4 * column) + 2] ^ mult(0x02, byte_buffer[(4 * column) + 3]);

        byte_buffer[4 * column] = temp_bytes[0];
        byte_buffer[4 * column + 1] = temp_bytes[1];
        byte_buffer[4 * column + 2] = temp_bytes[2];
        byte_buffer[4 * column + 3] = temp_bytes[3];
    }
}

void addRoundKey(aes_state * state, int round) {
    int i;
    for (i = 0; i < BLOCK_SIZE; i++) {
        state->buffer[i] ^= state->key_schedule[(round * BLOCK_SIZE) + i];
    }
}