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

    printf("\n%d tests failed\n", num_failed);
    printf("%d tests succeeded\n", num_succeeded);

    return !num_failed;
}