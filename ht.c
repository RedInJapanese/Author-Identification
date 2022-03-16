#include "ht.h"
#include "salts.h"
#include "speck.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
//implementation for lines 26-29 from Akash Basu(self) from universe.c in assignment 4
//implementation for lines 45-52 derived from Eugene Chou(TA) from the lecture on hash tables
//implementation for lines 63-71 derived from Euegene Chou(TA) from the lecture on hash tables
//Hash table constructor inspired by Akash Basu(self) from pq.c in assignment 6
struct HashTable { //declaration of hash table struct with salts, size, and array of node pointers
    uint64_t salt[2];
    uint32_t size;
    Node **slots;
};

HashTable *ht_create(uint32_t size) { //constructor function for the hash table
    HashTable *x = (HashTable *) malloc(sizeof(HashTable));
    x->salt[0] = SALT_HASHTABLE_LO; //sets the two elements in salt to the macros defined in salts.h
    x->salt[1] = SALT_HASHTABLE_HI;
    x->slots = (Node **) calloc(size, sizeof(Node *)); //initializes node pointer array
    x->size = size;
    return x; //returns hash table
}

void ht_delete(HashTable **ht) {
    for (uint32_t i = 0; i < (*ht)->size; i++) { //deletes every element in the node pointer array
        if ((*ht)->slots[i] != NULL) {
            node_delete(&(*ht)->slots[i]);
        }
    }
    free((*ht)->slots); //frees the node pointer array itself
    (*ht)->slots = NULL;
    free(*ht); //frees the hash table
    *ht = NULL;
}

uint32_t ht_size(HashTable *ht) { //returns the size of the hash table
    return ht->size;
}

Node *ht_lookup(HashTable *ht, char *word) {
    uint32_t index
        = hash(ht->salt, word)
          % ht->size; //gets the words index in the hash table by calling h(k) and modding it by the table size so that the index is not out of bounds
    uint32_t i = 0;
    while (i < ht->size) { //iterate through the entire hash table
        Node *check
            = ht->slots
                  [index]; //assigns a node to the current pointer at the index and checks if it was successful
        if (check
            && strcmp(check->word, word) == 0) { //uses strcmp to make sure the words are the same
            return check; //returns the node for the word if the conditional is true
        }
        if (check == NULL) {
            return NULL; //returns null if the preceding node is null
        }
        index = (index + 1) % ht->size; //increments the index
        i++;
    }
    return NULL; //returns NUL by default
}

Node *ht_insert(HashTable *ht, char *word) { //inserts a node into the hash table
    uint32_t index = hash(ht->salt, word) % ht->size; //gets the index number of the word
    uint32_t i = 0;
    while (i < ht->size) { //iterates through the entire hash table
        if (!ht->slots[index]) { //checks if the node doesnt already exist
            ht->slots[index] = node_create(word); //if it doesn't, then a node is created
            ht->slots[index]->count = 1;
            return ht->slots[index];
        } else if (ht->slots[index]) { //checks if the node exists or not
            if (ht_lookup(ht,
                    word)) { //if the htlookup says that the word exists, then only the counter for said word is incremented
                ht->slots[index]->count++;
                return ht->slots[index];
            }
        }
        index = (index + 1) % ht->size; //increments the index
        i++;
    }
    return NULL; //returns NULL by default
}

void ht_print(HashTable *ht) {
    for (
        uint32_t i = 0; i < ht->size;
        i++) { //printing function for the hash table in order to debug. Can be implemented in various ways
        if (ht->slots[i] != NULL) {
            node_print(ht->slots[i]);
        }
    }
    printf("size: %u \n", ht_size(ht));
}

struct HashTableIterator { //declaration of the hash table iterator struct
    HashTable *table;
    uint32_t slot;
};

HashTableIterator *hti_create(
    HashTable *ht) { //constructor for the iterator that initializes the ht pointer and index
    HashTableIterator *h = (HashTableIterator *) malloc(sizeof(HashTableIterator));
    h->table = ht;
    h->slot = 0;
    return h;
}

void hti_delete(HashTableIterator **hti) { //destructor of the hash table iterator
    free(*hti);
    *hti = NULL;
}

Node *ht_iter(HashTableIterator *hti) { //function to give the next word in the hash table
    while (hti->slot < hti->table->size) { //iterates through the entire hash table
        hti->slot++;
        if (hti->table->slots[hti->slot - 1]
            != NULL) { //checks to make sure the current node isnt NULL so as to return a word
            return hti->table->slots[hti->slot - 1];
        }
    }
    return NULL; //returns NULL by default
}
