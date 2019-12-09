/* test.c
 * Written by: Austin Bohannon
 * Copyright: 2019
 * License: MPL 2.0
 *
 * Runs tests for the aes library. The success or failure of each function will
 * be printed to the screen. The program returns 0 if there were no failures,
 * and 1 if any test cases failed.
 */

#include <stdint.h>
#include <stdio.h>
#include "aes.h"

int assert_arrays_equal(const uint32_t * array1, const uint32_t * array2, int length) {
    int i;
    for (i = 0; i < length; i++){
        if (array1[i] != array2[i]) {
            return 0;
        }
    }
    return 1;
}

int test_mult() {
    uint8_t a = 0x57;
    uint8_t b = 0x83;

    uint8_t c = 0xf1;
    uint8_t d = 0x03;

    return (mult(a, b) == 0xc1) && (mult(c, d) == 0x08);
}

int test_sub_word() {
    uint32_t a = 0xbee33d19;

    return (subWord(a) == 0xae1127d4) && (a == 0xbee33d19);
}

int test_rot_word() {
    uint32_t a = 0x09cf4f3c;

    return (rotWord(a) == 0xcf4f3c09) && (a == 0x09cf4f3c);
}

int test_sub_bytes() {
    uint32_t before[BLOCK_SIZE] = {
        0xbee33d19, 0x2be2f4a0, 0x2a8dc69a, 0x0848f8e9
    };
    const static uint32_t after[BLOCK_SIZE] = {
        0xae1127d4, 0xf198bfe0, 0xe55db4b8, 0x3052411e
    };
    subBytes(before);
    return assert_arrays_equal(before, after, BLOCK_SIZE);
}

int test_shift_rows() {
    uint32_t before[BLOCK_SIZE] = {
        0x0c080400, 0x0d090501, 0x0e0a0602, 0x0f0b0703
    };
    const static uint32_t after[BLOCK_SIZE] = {
        0x0f0a0500, 0x0c0b0601, 0x0d080702, 0x0e090403
    };
    shiftRows(before);
    return assert_arrays_equal(before, after, BLOCK_SIZE);
}

int test_mix_columns() {
    uint32_t before[BLOCK_SIZE] = {
        0x305dbfd4, 0xae52b4e0, 0xf11141b8, 0xe598271e
    };
    const static uint32_t after[BLOCK_SIZE] = {
        0xe5816604, 0x9a19cbe0, 0x7ad3f848, 0x4c260628
    };
    mixColumns(before);
    return assert_arrays_equal(before, after, BLOCK_SIZE);
}

int test_key_expansion() {
    aes_state state;
    const static uint32_t cipher_key[4] = {
        0x2b7e1516, 0x28aed2a6, 0xabf71588, 0x09cf4f3c
    };
    const static uint32_t key_schedule[44] = {
        0x2b7e1516, 0x28aed2a6, 0xabf71588, 0x09cf4f3c,
        0xa0fafe17, 0x88542cb1, 0x23a33939, 0x2a6c7605,
        0xf2c295f2, 0x7a96b943, 0x5935807a, 0x7359f67f,
        0x3d80477d, 0x4716fe3e, 0x1e237e44, 0x6d7a883b,
        0xef44a541, 0xa8525b7f, 0xb671253b, 0xdb0bad00,
        0xd4d1c6f8, 0x7c839d87, 0xcaf2b8bc, 0x11f915bc,
        0x6d88a37a, 0x110b3efd, 0xdbf98641, 0xca0093fd,
        0x4e54f70e, 0x5f5fc9f3, 0x84a64fb2, 0x4ea6dc4f,
        0xead27321, 0xb58dbad2, 0x312bf560, 0x7f8d292f,
        0xac7766f3, 0x19fadc21, 0x28d12941, 0x575c006e,
        0xd014f9a8, 0xc9ee2589, 0xe13f0cc8, 0xb6630ca6
    };

    keyExpansion(&state, cipher_key);
    return assert_arrays_equal(state.key_schedule, key_schedule, 44);
}

int main() {
    int num_failed = 0;
    int num_succeeded = 0;

    printf("test_mult(): ");
    if (test_mult()) {
        num_succeeded++;
        printf("SUCCEEDED!\n");
    } else {
        num_failed++;
        printf("FAILED!\n");
    }

    printf("test_sub_word(): ");
    if (test_sub_word()) {
        num_succeeded++;
        printf("SUCCEEDED!\n");
    } else {
        num_failed++;
        printf("FAILED!\n");
    }

    printf("test_rot_word(): ");
    if (test_rot_word()) {
        num_succeeded++;
        printf("SUCCEEDED!\n");
    } else {
        num_failed++;
        printf("FAILED!\n");
    }

    printf("test_sub_bytes(): ");
    if (test_sub_bytes()) {
        num_succeeded++;
        printf("SUCCEEDED!\n");
    } else {
        num_failed++;
        printf("FAILED!\n");
    }

    printf("test_shift_rows(): ");
    if (test_shift_rows()) {
        num_succeeded++;
        printf("SUCCEEDED!\n");
    } else {
        num_failed++;
        printf("FAILED!\n");
    }

    printf("test_mix_columns(): ");
    if (test_mix_columns()) {
        num_succeeded++;
        printf("SUCCEEDED!\n");
    } else {
        num_failed++;
        printf("FAILED!\n");
    }

    printf("test_key_expansion(): ");
    if (test_key_expansion()) {
        num_succeeded++;
        printf("SUCCEEDED!\n");
    } else {
        num_failed++;
        printf("FAILED!\n");
    }

    printf("\n%d tests failed\n", num_failed);
    printf("%d tests succeeded\n", num_succeeded);

    return !num_failed;
}