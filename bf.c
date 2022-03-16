#include "bf.h"
//#include "bv.h"
#include "speck.h"
#include "salts.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//lines 33-38 derived from Akash Basu(self) from code.c in assignment 6
struct BloomFilter { //bloom filter struct containing three different salts
    uint64_t primary[2];
    uint64_t secondary[2];
    uint64_t tertiary[2];
    BitVector *filter; //filter containing the bitvector
};

BloomFilter *bf_create(uint32_t size) { //constructor for the bloom filter
    BloomFilter *bf = (BloomFilter *) malloc(sizeof(BloomFilter));

    bf->primary[0] = SALT_PRIMARY_LO;
    bf->primary[1] = SALT_PRIMARY_HI;

    bf->secondary[0] = SALT_SECONDARY_LO;
    bf->secondary[1] = SALT_SECONDARY_HI;

    bf->tertiary[0] = SALT_TERTIARY_LO;
    bf->tertiary[1] = SALT_TERTIARY_HI;
    //initialization of the three salts
    bf->filter = bv_create(size);
    //initialization of the bit vector
    return bf;
}

void bf_delete(BloomFilter **bf) { //destructor for the bloom filter
    bv_delete(&(*bf)->filter); // deletes the bit vector
    (*bf)->filter = NULL;
    free(*bf); //deletes the ADT
    *bf = NULL;
}

uint32_t bf_size(BloomFilter *bf) { //returns the size of the bloom filter
    return bv_length(bf->filter);
}

void bf_insert(BloomFilter *bf, char *word) {
    uint32_t x = hash(bf->primary, word) % bf_size(bf);
    uint32_t y = hash(bf->secondary, word) % bf_size(bf);
    uint32_t z
        = hash(bf->tertiary, word)
          % bf_size(bf); //hashes three different u32s using the three salts initialized earlier

    bv_set_bit(bf->filter, x);
    bv_set_bit(bf->filter, y);
    bv_set_bit(bf->filter, z); //sets the bits for all three u32s
}

bool bf_probe(BloomFilter *bf, char *word) {
    uint32_t x = hash(bf->primary, word) % bf_size(bf);
    uint32_t y = hash(bf->secondary, word) % bf_size(bf);
    uint32_t z
        = hash(bf->tertiary, word)
          % bf_size(bf); //hashes three different u32s using the three salts initialized earlier

    if (bv_get_bit(bf->filter, x) && bv_get_bit(bf->filter, y)
        && bv_get_bit(bf->filter, z)) { //checks if all 3 of the u32s are in the bloom filter
        return true; //returns true if that's the case
    } else {
        return false;
    }
}

void bf_print(BloomFilter *bf) { //print function for debugging
    bv_print(bf->filter);
}
