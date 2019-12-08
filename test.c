/* test.c
 * Written by: Austin Bohannon
 * Copyright: 2019
 * License: MPL 2.0
 *
 * Runs tests for the aes library.
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

int main() {
    int num_failed = 0;
    int num_succeeded = 0;

    if (test_shift_rows()) {
        num_succeeded++;
        printf("test_shift_rows(): SUCCEEDED!\n");
    } else {
        num_failed++;
        printf("test_shift_rows(): FAILED!");
    }

    printf("\n%d tests failed\n", num_failed);
    printf("%d tests succeeded\n", num_succeeded);
}