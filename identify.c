#include "pq.h"
#include "box.h"
#include "text.h"
#include "bf.h"
#include "bv.h"
#include "ht.h"
#include "node.h"
#include "parser.h"
#include "speck.h"
#include "metric.h"
#include "salts.h"

#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <inttypes.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

#define MAX     4096
#define OPTIONS "d:n:k:l:emcvh"
//line 108 derived from gecko10000(student): https://discord.com/channels/926211705766305842/926211706382868581/947717984120610826
void usage(char *exec) { //usage options that are printed out through standard error
    fprintf(stderr,
        "SYNOPSIS\n"
        "Identifies the author of some anonymous text passed through stdin.\n"
        "Basically computes k-nearest neighbors, where k = 1 by default.\n"
        "The number of noise words that are removed is tunable.\n"
        "Uses either Euclidean, Manhattan, or cosine distance as metric.\n"
        "\nUSAGE\n"
        "	%s [OPTIONS]\n"
        "\nOPTIONS\n"
        "-d			Database of authors and texts [default: lib.db]\n"
        "-k matches		Set number of top matches to display [default: 5]\n"
        "-n noisefile		Set file of words to ignore [default: noise.txt]\n"
        "-l limit		Set noise word limit[default: 100]\n"
        "-e			Set distance metric as Euclidean distance [default]\n"
        "-m			Set distance metric as Manhattan distance\n"
        "-c			Set distance metric as cosine distance\n"
        "-v			Enable verbose printing of program run\n"
        "-h			Display program help and usage\n",
        exec);
}

int main(int argc, char **argv) {
    int opt = 0;
    char *nfile = "noise.txt"; //default noise filter is noise.txt
    FILE *noisefile = NULL; //noisefile that will open the path to the noise filter file

    char *database = "lib.db"; //default database is lib.db
    FILE *db = NULL; //database file that will open the specified database

    char *author = (char *) calloc(MAX, sizeof(char));
    char *path = (char *) calloc(
        MAX, sizeof(char)); //arrays for getting the author and path from the database

    int m, e, c; //variables to specify which distance metric to calculate
    m = 0;
    e = 1; //euclidean distance is the default
    c = 0;
    double sum;
    sum = 0;

    bool verbose = false; //boolean used to specify verbose output

    limit = 100; //noise limit(default is 100)
    uint32_t k = 5; //top distances(default is 5)

    FILE *auth = NULL;
    Text *comp = NULL;
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) { //switch case for the different command line options
        case 'd': database = optarg; break;
        case 'k': k = (uint32_t) strtoul(optarg, NULL, 10); break;
        case 'n': nfile = optarg; break;
        case 'l': limit = (uint32_t) strtoul(optarg, NULL, 10); break;
        case 'e': e++; break;
        case 'm': m++; break;
        case 'c': c++; break;
        case 'v': verbose = true; break;
        case 'h': usage(argv[0]); break;
        default: usage(argv[0]); break;
        }
    }
    if (!(noisefile = fopen(nfile, "r"))) { //error handling for the noisefile
        puts("Error opening noise file.");
        exit(0);
    }
    if (!fopen(database, "r")) { //error handling for the database
        puts("Error opening database file.");
        fclose(fopen(database, "r"));
        exit(0);
    }
    db = fopen(database, "r"); //opens the specified database

    uint32_t n = 0;
    fscanf(db, "%u \n", &n); //used to get the size of the priority queue

    Text *noise = text_create(noisefile, NULL); //creation of the noise text

    Text *anon = text_create(stdin, noise); //takes the anonymous text from stdin

    PriorityQueue *q = pq_create(n); //declaration of the priority queue

    while (!feof(db)) { //while loop to check if the databas has reached the EOF mark
        fgets(author, MAX, db);
        fscanf(db, "%s\n", path); //takes the author name and the path

        author[strlen(author) - 1]
            = '\0'; //removes the newline character from the end of the author name
        auth = fopen(path, "r");
        if (!auth) { //error handling for in case the author path doesn't actually exist
            puts("Error opening path.");
            exit(0);
            break;
        }
        comp = text_create(auth, noise); //creates the author text
        //conditionals for whichever distance metric the user picks(default is euclidean)
        if (c > 0) {
            m = 0;
            e = 0;
            sum = text_dist(anon, comp,
                COSINE); //gets the sum for the cosine distance. frees the author text and enqueues the sum
            text_delete(&comp);
            enqueue(q, author, sum);
        }
        if (m > 0) {
            e = 0;
            c = 0;
            sum = text_dist(anon, comp,
                MANHATTAN); //gets the sum for the manhattaan distance. frees the author text and enqueues the sum
            text_delete(&comp);
            enqueue(q, author, sum);
        }
        if (e > 0) {
            m = 0;
            c = 0;
            sum = text_dist(anon, comp,
                EUCLIDEAN); //gets the sum for the euclidean distance. frees the author text and enqueues the sum
            text_delete(&comp);
            enqueue(q, author, sum);
        }
        if (verbose) {
            printf("Processed %s [%f] (%s) \n", path, sum,
                author); //prints the texts that have been processed if verbose was set to true in the command line
        }
    }
    if (k > pq_size(q)) { //conditional to check if k is greater than the priority queue size
        k = pq_size(q);
    }
    if (m > 0) { //conditoinals to print the first line of the top k authors
        printf("Top %u, metric: Manhattan distance, noise limit: %u\n", k, limit);
    }
    if (e > 0) {
        printf("Top %u, metric: Euclidean distance, noise limit: %u\n", k, limit);
    }
    if (c > 0) {
        printf("Top %u, metric: Cosine distance, noise limit: %u\n", k, limit);
    }
    text_delete(&anon);
    text_delete(&noise);
    fclose(auth);
    fclose(db);
    fclose(noisefile);
    free(path);
    path = NULL;
    free(author);
    author = NULL; //deletes the necessary texts, closes all opened files, and frees all pointers
    for (uint32_t i = 0; i < k; i++) { //prints the top k likely authors using pq_print
        pq_print(q);
    }
    pq_delete(&q); //deletes the priority queue
}
