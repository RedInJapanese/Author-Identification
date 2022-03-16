#include "node.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//implementation of lines: 7-13, 15-20, 22-24 from Akash Basu(self) from node.c in assignment 6
Node *node_create(char *word) { //constructor of the node
    Node *x = (Node *) malloc(sizeof(Node));
    x->word = strdup(
        word); //duplicates the word using strdup since using '=' would just have the word point to the passed argument
    free(strdup(word)); //frees strdup since it returns a pointer
    x->count = 0;
    return x; //returns the node
}

void node_delete(Node **n) { //destructor for the node
    free((*n)->word);
    (*n)->word = NULL;
    free(*n);
    *n = NULL;
}

void node_print(
    Node *n) { //printing function to debug the nodes. Can be implemented in various ways
    printf("\nword: %s count: %u \n", n->word, n->count);
}
