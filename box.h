#pragma once

#include <stdlib.h>
#include <stdint.h>

uint32_t limit; //noise limit

typedef struct Box Box;

struct Box { //declaration of box struct
    char *word;
    double distance;
};

//declaration of other functions
Box *box_create(char *word);

void box_delete(Box **box);

void box_print(Box *b);
