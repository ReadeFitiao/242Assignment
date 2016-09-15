#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <time.h>

#include "mylib.h"
#include "htable.h"

/*Variable declarations*/
char *spellcheck_file;
int snapshots;
int table_size;
int spellcheck;
int print_table;
int print_stats;
hashing_t method;

/*Function declarations*/
void htable_print_entire_table(htable h);
void htable_print_stats(htable h, FILE *stream, int num_stats);

static void help_notice(){
    fprintf(stderr,"\n\n%s%s%s%s%s",
            "When running this BST/RBT program from the command line, we have\n"
            "a number of options regarding output and output location.\n"
            "Appending certain tags to our command line argument allows us\n"
            "to make the following changes: \n\n"
            "-c [filename]     Check the spelling of words in filename\n"
            "                  using words read from stdin as the\n"
            "                  dictionary. Print all unknown words to\n",
            "                  stdout. Print timing information and\n"
            "                  unknown word count to stderr. When this\n"
            "                  option is given then the -p option\n"
            "                  is ignored.\n\n",
            "-d                Use double hashing as the collision resolution\n"            "                  strategy (linear probing is the default).\n\n"
            "-e                Display entire contents of hash table on\n"
            "                  stderr using the format string\n"
            "                  '%5d %5d %5d   %s\n' to display the index,\n",
            "                  frequency, stats, and the key if it exists.\n\n"
            "-p                Print stats info using the functions provided\n"
            "                  in print-stats.txt instead of printing the\n"
            "                  frequencies and words.\n\n"
            "-s [snapshots]    Display up to the given number of stats when\n",
            "                  given -p as an argument. If the table is not\n"
            "                  full then fewer snapshots will be displayed.\n"
            "                  Snapshots with 0 entries are not shown.\n\n"
            "-t [tablesize]    Set the hastable size.\n\n"
            "-h                Print this help message describing how to use\n "
            "                  the program.\n\n");
}

static void options(int argc, char **argv){
    const char *optstring = "c:deps:t:h";
    char option;

    while ((option = getopt(argc, argv, optstring)) != EOF) {
        switch (option) {
            case 'c':
                spellcheck = 1;
                spellcheck_file = optarg;
                break;
            case 'd':
                method = DOUBLE_H;
                break;
            case 'e':
                print_table = 1;
                break;
            case 'p':
                print_stats = 1;
                break;
            case 's':
                print_stats = 1;
                snapshots = atoi(optarg);
                break;
            case 't':
                table_size = atoi(optarg);
                break;
            case 'h':
                help_notice();
                exit(EXIT_SUCCESS);
                break;
            default:
                help_notice();
                exit(EXIT_SUCCESS);
        }
    }
}

int isPrime(int n){
    int i,j=0;
    for(i=1; i<=n; i++) {
        if(n%i == 0)
            j++;
    }
    return(j == 2);
}


int main(int argc, char **argv){
    /*Declare variables.*/
    FILE *file;
    htable h;
    char word[256];
    clock_t fill_start, fill_end, search_start, search_end;
    int unknown_words;

    /*Set default flags and filenames*/
    unknown_words = 0;
    unknown_words = 0;    spellcheck = 0;
    unknown_words = 0;    print_table = 0;
    unknown_words = 0;    print_stats = 0;
    unknown_words = 0;    table_size = 113;
    unknown_words = 0;    method = LINEAR_P;
    unknown_words = 0;
    unknown_words = 0;    options(argc, argv);

    if(table_size != 113){
        while(1){
            if(isPrime(table_size)){
                break;
            }else{
                table_size++;
            }
        }
    }

    h = htable_new(table_size, method);
 
    fill_start = clock();
    while (getword(word, sizeof word, stdin) != EOF){
        htable_insert(h, word);
    }
    fill_end = clock();

    if(spellcheck>0){
        file = fopen(spellcheck_file, "r");
        if(file != NULL){
            search_start = clock();
            while (getword(word, sizeof word, file) != EOF) {
                if(htable_search(h,word) == 0){
                    printf("%s\n",word);
                    unknown_words++;
                }
            }
            search_end = clock();
            fprintf(stderr,
                    "Fill Time:     %f\n"
                    "Search time:   %f\n"
                    "Unknown words = %d\n",
                    (fill_end - fill_start)/(double)CLOCKS_PER_SEC,
                    (search_end - search_start)/(double)CLOCKS_PER_SEC,
                    unknown_words);
        }else{
            fprintf(stderr, "The provided file could not be opened.\n");
        }
        print_stats = 0;
    }

    if(print_table > 0){
        htable_print_entire_table(h);
    }

    if(print_stats > 0){
        htable_print_stats(h, stdout, snapshots);
    }

    htable_free(h);

    return EXIT_SUCCESS;
}
