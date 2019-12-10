# Advanced Encryption Standard
*Writted by: Austin Bohannon*

## Introduction
This code implements AES from the [FIPS standard](https://csrc.nist.gov/csrc/media/publications/fips/197/final/documents/fips-197.pdf). It is written in compliant ANSI C and has no memory issues, according to Valgrind.

This code is **not** fit for production use. It has not been optimized. It has not been audited. The testing has been no more extensive than the tests listed in `test.c` and a quick Valgrind pass. This library was made purely for a class project. There is absolutely no warranty of any kind associated with this software, and using it is done at one's own risk.

The code as written implements AES-128, though it was made with the express intention of easily compiling for other versions. See the header in `aes.h` for information on using AES-192 and AES-256.

## Build
First, clone this repository. Then, this will build the library and its tests into a single executable:
```bash
gcc -ansi -Wall test.c aes.c aes.h -o test_aes.out
```

### Dependencies
This code base only depends on a C compiler and the C standard libraries.

## Run
To run the tests, run:
```bash
./test_aes.out
```

## License
This is licensed under the Mozilla Public License (MPL). See the `LICENSE` file for more information.
