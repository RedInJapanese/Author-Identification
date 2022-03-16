#include "bv.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//line 15 inspired by Professor Long in the code comments repository(bv8.h): https://git.ucsc.edu/cse13s/code-comments/-/blob/master/bv8.h
//line 34 inspired by Professor Long in the code comments repository(bv8.h): https://git.ucsc.edu/cse13s/code-comments/-/blob/master/bv8.h
//line 44 inspired by Professor Long in the code comments repository(bv8.h): https://git.ucsc.edu/cse13s/code-comments/-/blob/master/bv8.h
//line 54 inspired by Professor Long in the code comments repository(bv8.h): https://git.ucsc.edu/cse13s/code-comments/-/blob/master/bv8.h
//lines 61-70 derived from Akash Basu(self) in code.c from assignment 6
struct BitVector {
    uint32_t length;
    uint8_t *vector;
};

BitVector *bv_create(uint32_t length) { //bit vector constructor
    BitVector *bv = (BitVector *) malloc(sizeof(BitVector));
    bv->length = length;
    uint32_t bytes
        = length / 8
          + (length % 8
                  ? 1
                  : 0); //uses ceiling division in order to convert the length parameter passed in as bits to bytes. This is because the smallest granularity of data in C is bytes.
    bv->vector = (uint8_t *) calloc(bytes, sizeof(uint8_t)); //initialization of the bit vector
    return bv;
}

void bv_delete(BitVector **bv) { //destructor of the bit vector
    free((*bv)->vector);
    (*bv)->vector = NULL; //frees the array
    free(*bv);
    *bv = NULL; //sets the adt to NULL
}

uint32_t bv_length(BitVector *bv) { //returns length of the bit vector
    return bv->length;
}

bool bv_set_bit(
    BitVector *bv, uint32_t i) { // function for setting a bit in a bit vector to 1 using bitwise OR
    if (i < bv->length && i >= 0) { //checks if the index is in bounds
        bv->vector[i / 8] |= (0x1 << i % 8);
        return true; //returns true if setting the bit was successfull
    } else {
        return false;
    }
}

bool bv_clr_bit(
    BitVector *bv, uint32_t i) { //function for clearing a bit using bitwise AND and inverse
    if (i < bv->length && i >= 0) {
        bv->vector[i / 8]
            &= ~(0x1 << i % 8); //uses /8 and mod in order to access the bit at a specific partition
        return true; //returns true if successfull
    } else {
        return false;
    }
}

bool bv_get_bit(BitVector *bv,
    uint32_t i) { //function that uses bitwise AND in order to get a bit at a specific
    if (i < bv->length && i >= 0) {
        return (bv->vector[i / 8] >> i % 8)
               & 0x1; //returns the bit as a boolean since true and false can be 1 or 0 in C
    } else {
        return false;
    }
}

void bv_print(
    BitVector *bv) { //print function for the bit vector. Can be implemented in various ways
    for (uint32_t i = 0; i < bv->length; i++) {
        if (bv_get_bit(bv, i)) {
            printf("1");
        } else {
            printf("0");
        }
    }
    printf("\n");
}
