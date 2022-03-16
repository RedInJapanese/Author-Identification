#include "parser.h"
#include "text.h"
#include "ht.h"
#include "bf.h"
#include "box.h"
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <inttypes.h>
#include <math.h>
//lines 23 and 24 derived from Professor Long: https://discord.com/channels/926211705766305842/926211706382868581/950157148782993538
//line 28-29,31, 57 derived from the assignment 7 documemt(regex code sample)
//lines 97-110, 116-130, 135-147 derived from Eugene Chou(TA): Section/Office Hours
#define WORD "[a-zA-Z]+([-']?[a-zA-Z]+)*" //regex macro
struct Text {
    HashTable *ht;
    BloomFilter *bf;
    uint32_t word_count;
}; //declaration of text ADT that contains a hash table, bloom filter, and word count

Text *text_create(FILE *infile, Text *noise) { //constructor function for text ADT
    Text *t = (Text *) malloc(sizeof(Text));
    t->ht = ht_create(1 << 19);
    t->bf = bf_create(1 << 21); //initializes the bloom filter and hash table to 2^19 and 2^21
    t->word_count = 0;
    regex_t re;
    regcomp(&re, WORD, REG_EXTENDED); //initialization of regex expression
    if (t->ht && t->bf) { //checks to see if the memory allocation was sucessfull
        char *word = NULL;
        while (
            (word = next_word(infile, &re))
            != NULL) { //iterates through the input file and uses word to get the new word. nex_word method provided by parser.c
            //t->word_count++;
            unsigned long i = 0; //iterator to go through all the elements in word
            size_t length = strlen(word); //gets the length of the word inputted
            //char *lower = (char *) calloc(length, sizeof(char));
            while (i < length) { //while loop for converting the word taken from infile to lowercase
                word[i] = tolower(word[i]);
                i++;
            }
            if (noise) { //checks to make sure the noise parameter wasn't NULL
                if (text_contains(noise, word)
                    == false) { //checks to make sure that the inputted text isnt in the noise text
                    t->word_count++;
                    ht_insert(t->ht, word);
                    bf_insert(
                        t->bf, word); //inserts the word in the hash table and bloom filter if it is
                }
            } else { //if the noise argument was passed in as NULL, it means that the noise is being created
                if (t->word_count < limit) {
                    t->word_count++;
                    ht_insert(t->ht, word);
                    bf_insert(t->bf,
                        word); //insert all the noise wordes into the hash table and bloom filter
                }
            }
            //free(lower);
            //lower = NULL;
        }
        regfree(&re); //free the regex_t
        return t;
    }
    return NULL; //return NULL by default
}

void text_delete(Text **text) { //destructor for the text
    bf_delete(&(*text)->bf);
    ht_delete(&(*text)->ht); //frees the hash table and bloom filter before deleting the text ADT
    free(*text);
    *text = NULL;
}

bool text_contains(Text *text, char *word) { //checks if the text is in the hash table
    if (bf_probe(text->bf, word)) {
        if (ht_lookup(text->ht, word)) {
            return true; //returns true if the ht_lookup call returns a pointer
        }
    }
    return false;
}

double text_frequency(Text *text, char *word) { //function to get the frequency of a word in a text
    Node *n = ht_lookup(
        text->ht, word); //checks if the text exists as long as it doesn't return a NULL pointer
    if (n) {
        return n->count; //returns the node count
    }
    return 0;
}

double text_dist(
    Text *text1, Text *text2, Metric metric) { //function for calculating one of the three distances
    Node *n = NULL;
    HashTableIterator *ht1 = hti_create(text1->ht);
    HashTableIterator *ht2 = hti_create(
        text2
            ->ht); //creates two hash table iterators for both texts in order to get the frequencies of each word between both texts
    double sum = 0;
    double t1, t2;
    double sub = 0;
    //double product = 0; //variables for storing numbers during the distance calculations
    //puts("dist1");
    switch (metric) { //switch case to see what the user inputted as their distance metric
    case MANHATTAN:
        //puts("starting manhattan");
        while (
            (n = ht_iter(ht1))
            != NULL) { //iterates through text1(the text sent through stdin) in order to normalize the words between text1 and 2
            //printf("t1 freq: %f\n",  text_frequency(text1, n->word));
            //puts("manhattan");
            t1 = text_frequency(text1, n->word) / text1->word_count;
            t2 = text_frequency(text2, n->word) / text2->word_count;
            sum += fabs(
                t1
                - t2); //takes the absolute value of the two normalized words and adds it to the sum
        }
        while (
            (n = ht_iter(ht2))
            != NULL) { //iterates through text2 in order to get words in text2 that arent in text1
            if (text_contains(text1, n->word) == false) {
                //t1 = text_frequency(text1, n->word) / text1->word_count;
                t2 = text_frequency(text2, n->word) / text2->word_count;
                sum += fabs(
                    t2); //adds only text 2 words, as the normalized values for text1 would just be 0
            }
        }
        //puts("ending manhattan");
        break;
    case EUCLIDEAN: //eucludidean distance metric

        while ((n = ht_iter(ht1)) != NULL) { //iterates through text1
            t1 = text_frequency(text1, n->word) / text1->word_count;
            t2 = text_frequency(text2, n->word)
                 / text2->word_count; //takes the normalized values of both text1 and text2
            sub = t1 - t2;
            sub = sub * sub; //squares the differentce between text1 and 2, then adds it to the sum
            sum += sub;
        }
        while ((n = ht_iter(ht2))
               != NULL) { //iterates in order to find the words that are exclusive to text2
            if (text_contains(text1, n->word) == false) {
                t2 = text_frequency(text2, n->word) / text2->word_count;
                sub = t2;
                sub = sub * sub;
                sum += sub; //only adds the square of text2 words
            }
        }
        sum = sqrt(sum);
        break;

    case COSINE: //cosine distance metric
        while ((n = ht_iter(ht1)) != NULL) { //iterates through text1
            t1 = text_frequency(text1, n->word) / text1->word_count;
            t2 = text_frequency(text2, n->word) / text2->word_count;
            //takes the product of the normalized values between text1 and text2 and adds their product to the sum
            sum += (t1 * t2);
        }
        while ((n = ht_iter(ht2)) != NULL) {
            if (text_contains(text1, n->word) == false) {
                t1 = text_frequency(text1, n->word) / text1->word_count;
                t2 = text_frequency(text2, n->word)
                     / text2
                           ->word_count; //iterates through the normalized words exclusive to text2 and adds them to the su
                sum += (t1 * t2);
            }
        }
        sum = 1 - sum;
        break;
    }
    hti_delete(&ht1);
    hti_delete(&ht2); //frees the two hash table iterators
    n = NULL;
    return sum;
}

void text_print(Text *text) { //debugging function for text
    //bf_print(text->bf);
    ht_print(text->ht);
}
