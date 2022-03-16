#include "box.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//Note: The box adt was implemented for use in the priority queue. It works in the same way that node.c works.
//Note: implementation for box.c derived from node.c
Box *box_create(char *word) { //constructor for the box
    Box *b = (Box *) malloc(sizeof(Box));
    b->word = strdup(word); //duplicates the word being passed in as an argument
    b->distance = 0;
    return b;
}

void box_delete(Box **box) { //destructor for the box
    free((*box)->word);
    (*box)->word = NULL;
    free(*box);
    *box = NULL;
}

void box_print(Box *b) { //print function for the box which is used in pq_print in pq.c
    printf("%s [%0.15f]\n", b->word, b->distance);
}
