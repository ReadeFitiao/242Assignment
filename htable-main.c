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

/**
 * Displays help notice.
 */
static void help_notice(){
    fprintf(stderr,"%s%s%s",
            "Usage: ./sample-htable [OPTION]... <STDIN>\n\n"

            "Perform various operations using a hash table.  By default,\n"
            "words are read from stdin and added to the hash table, before\n"
            "being printed out alongside their frequencies to stdout.\n\n",

            " -c FILENAME  Check spelling of words in FILENAME using words\n"
            "              from stdin as dictionary.  Print unknown words to\n"
            "              stdout, timing info & count to stderr (ignore -p)\n"
            " -d           Use double hashing (linear probing is the default)\n"
            " -e           Display entire contents of hash table on stderr\n",
            " -p           Print stats info instead of frequencies & words\n"
            " -s SNAPSHOTS Show SNAPSHOTS stats snapshots (if -p is used)\n"
            " -t TABLESIZE Use the first prime >= TABLESIZE as htable size\n\n"

            " -h           Display this messagen\n");
}

/**
 * Processes the commandline arguments to determine options.
 * @param argc the number of arguments.
 * @param **arcv the string of arguments.
 */
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
                snapshots = 10;
                break;
            case 's':
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

/**
 *Finds if a number is prime.
 * @param n the number to be tested.
 * @return true if prime fales if not.
 */
int isPrime(int n){
    int i,j=0;
    for(i=1; i<=n; i++) {
        if(n%i == 0)
            j++;
    }
    return(j == 2);
}

/**
 *Main method, initilises, fills and proforms
 *options selected on htable.
 * @param argc the number of arguments.
 * @param argv the string of arguments.
 * @return an exit-success notifier.
 */
int main(int argc, char **argv){
    /*Declare variables.*/
    FILE *file;
    htable h;
    char word[256];
    clock_t fill_start, fill_end, search_start, search_end;
    int unknown_words;

    /*Set default flags and values*/
    unknown_words = 0;
    spellcheck = 0;
    print_table = 0;
    print_stats = 0;
    table_size = 113;
    method = LINEAR_P;

    /*Get flags and values from user*/
    options(argc, argv);

    /*If the user has set a table size make it prime*/
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
