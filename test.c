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

    return (rotWord(a) == 0x3c09cf4f) && (a == 0x09cf4f3c);
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
        0x16157e2b, 0xa6d2ae28, 0x8815f7ab, 0x3c4fcf09
    };
    const static uint32_t key_schedule[44] = {
        0x16157e2b, 0xa6d2ae28, 0x8815f7ab, 0x3c4fcf09,
        0x17fefaa0, 0xb12c5488, 0x3939a323, 0x05766c2a,
        0xf295c2f2, 0x43b9967a, 0x7a803559, 0x7ff65973,
        0x7d47803d, 0x3efe1647, 0x447e231e, 0x3b887a6d,
        0x41a544ef, 0x7f5b52a8, 0x3b2571b6, 0x00ad0bdb,
        0xf8c6d1d4, 0x879d837c, 0xbcb8f2ca, 0xbc15f911,
        0x7aa3886d, 0xfd3e0b11, 0x4186f9db, 0xfd9300ca,
        0x0ef7544e, 0xf3c95f5f, 0xb24fa684, 0x4fdca64e,
        0x2173d2ea, 0xd2ba8db5, 0x60f52b31, 0x2f298d7f,
        0xf36677ac, 0x21dcfa19, 0x4129d128, 0x6e005c57,
        0xa8f914d0, 0x8925eec9, 0xc80c3fe1, 0xa60c63b6
    };

    keyExpansion(&state, cipher_key);
    return assert_arrays_equal(state.key_schedule, key_schedule, 44);
}

int test_cipher() {
    aes_state state;
    const static uint32_t buffer[4] = {
        0xa8f64332, 0x8d305a88, 0xa2983131, 0x340737e0
    };
    int i;
    for (i = 0; i < 4; i++)
        state.buffer[i] = buffer[i];
    const static uint32_t cipher_key[4] = {
        0x16157e2b, 0xa6d2ae28, 0x8815f7ab, 0x3c4fcf09
    };
    const static uint32_t after[4] = {
        0x1d842539, 0xfb09dc02, 0x978511dc, 0x320b6a19
    };

    keyExpansion(&state, cipher_key);
    cipher(&state);

    return assert_arrays_equal(state.buffer, after, BLOCK_SIZE);
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

    printf("test_cipher(): ");
    if (test_cipher()) {
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