#include "pq.h"
#include "box.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//implementation of lines 20-27 derived from Akash Basu(self) in pq.c from assignment 6
//implementation of lines 40-42 derived from Akash Basu(self) in pq.c from assignment 6
//implementation of lines 45-47 derived from Akash Basu(self) in pq.c from assignment 6
//implementation of lines 50-52 derived from Akash Basu(self) in pq.c from assignment 6
//implementation of lines 55-64 derived from Akash Basu(self) in pq.c from assignment 6
//implementation of lines 67-84 derived from Akash Basu(self) in pq.c from assignment 6
//implementation of lines 87-98 derived from Akash Basu(self) in pq.c from assignment 6
//implementation of lines 40-42 derived from Akash Basu(self) in pq.c from assignment 6
//implementation of lines 101-117 derived from Akash Basu(self) in pq.c from assignment 6
void insertion_sort(PriorityQueue *q,
    Box *b); //declaration of insertion sort and left shift functions for enqueueing and dequeuing
void l_shift(PriorityQueue *q);
//void k_print(PriorityQueue *q, uint32_t k);

typedef struct PriorityQueue { //declaration of priority queue struct
    Box *head;
    Box *tail;
    uint32_t size;
    uint32_t capacity;
    Box **boxes;
} PriorityQueue;

PriorityQueue *pq_create(uint32_t capacity) { //constructor for the priority queue
    PriorityQueue *q = (PriorityQueue *) malloc(sizeof(PriorityQueue));
    q->boxes = (Box **) calloc(capacity, sizeof(Box *)); //initializes an array of box pointers
    q->head = q->tail = NULL; //sets the head and tail to NULL
    q->capacity = capacity; //sets capacity to the passed in argument
    q->size = 0;
    return q;
}

void pq_delete(PriorityQueue **
        q) { //destructor of the priority queue function that deletes the elements inside the box array
    for (uint32_t i = 0; i < (*q)->size; i++) {
        box_delete(&(*q)->boxes[i]);
    }
    free((*q)->boxes);
    (*q)->boxes = NULL;
    free(*q);
    *q = NULL;
}

bool pq_empty(PriorityQueue *q) { //function to check if the queue is empty
    return q->size == 0;
}

bool pq_full(PriorityQueue *q) { //function to check if the queue is full
    return q->size == q->capacity;
}

uint32_t pq_size(PriorityQueue *q) { //function to return the size of the queue
    return q->size;
}

bool enqueue(PriorityQueue *q, char *author,
    double dist) { //function to enqueue and sort elements onto the queue
    if (pq_full(q) == true) { //first checks if the queue is full or not
        return false;
    }
    Box *b = box_create(author); //creates a new box and passes it into insertion_sort
    b->distance = dist; //sets the distance to what was passed in
    insertion_sort(q, b);
    q->size++; //increments size
    return true;
}

bool dequeue(
    PriorityQueue *q, char **author, double *dist) { //function to dequeue elements from the queue
    if (pq_empty(q) == true) { //checks if the queue is empty or not
        return false;
    }
    if (q->size == 1) { //conditional to check if this is the first box
        *author = q->head->word;
        *dist = q->head->distance;
        q->head = NULL;
        q->tail = NULL;
        q->size--;
    }
    if (q->size > 1) { //conditional for if theres more than one box in the queue
        *author = q->head->word;
        *dist = q->head->distance;
        l_shift(q);
    }
    return true;
}

void insertion_sort(PriorityQueue *q, Box *b) {
    uint64_t j = q->size;
    q->boxes[j] = b;
    Box *temp = q->boxes[j]; //sets temporary variables
    while (
        j > 0
        && temp->distance
               < q->boxes[j - 1]
                     ->distance) { //checks if the the current element is less than its predecessor
        q->boxes[j] = q->boxes[j - 1]; //swaps elements
        q->boxes[j - 1] = temp;
        j--;
    }
    q->head = q->boxes[0]; //resets the head and tail
    q->tail = q->boxes[q->size];
}

void l_shift(PriorityQueue *q) { //function to shift elements down whenever a box is dequeued
    uint64_t y = 1;

    for (uint64_t k = 0; k < pq_size(q);
         k++) { //iteratres through the entire queue and shifts everything down by 1 element
        q->boxes[k] = q->boxes[y];
        y++;
    }
    q->size--;
    q->head = q->boxes[0];
    q->tail = q->boxes[q->size - 1]; //resets the size, head, and tail
}
void pq_print(PriorityQueue *q) { //printing function for debugging
    static uint32_t index = 0;
    printf("%u) ", index + 1);
    box_print(q->boxes[index]);
    index++;
}
