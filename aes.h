/* aes.h
 * Written by: Austin Bohannon
 * Copyright: 2019
 * License: MPL 2.0
 *
 * Header file for the aes library. This implements AES as per the FIPS 197
 * specification. The constants defined below can be changed per that spec to
 * implement either AES-192 or AES-256; it currently implements AES-128 by
 * default.
 *
 * This library is a minimal implementation of the spec, with no helper
 * functions. The caller needs to manage parsing, padding, and loading data
 * into aes_state structs.
 *
 * This code conforms to ANSI C, and has no memory leaks according to Valgrind.
 */

#ifndef AES_H
#define AES_H

#include <stdint.h>

/* Constants for AES-128. Change these to implement AES-192 or AES-256 */
/* The number of 32-bit words in the block */
#define BLOCK_SIZE 4
/* The number of 32-bit words in the key */
#define KEY_LENGTH 4
/* The number of rounds to perform */
#define NUM_ROUNDS 10

typedef struct {
    uint32_t buffer[BLOCK_SIZE];
    uint32_t key_schedule[BLOCK_SIZE * (NUM_ROUNDS + 1)];
} aes_state;

void cipher(aes_state * state);

/* Should be private, but public for testing */
uint8_t mult(uint8_t n, uint8_t m);
void subBytes(uint32_t * buffer);
void shiftRows(uint32_t * buffer);
void mixColumns(uint32_t * buffer);
void addRoundKey(uint32_t * buffer);

#endif /* AES_H */