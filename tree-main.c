#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <time.h>

#include "mylib.h"
#include "tree.h"

/*Variable declarations*/
char *spellcheck_file;
char *dot_file;
int spellcheck;
int print_depth;
int dot;
tree_t type;

/**
 * Prints a help notice when "-h" is passed as an argument.
 */
static void help_notice() {
    fprintf(stderr,"%s%s%s",
            "Usage: ./sample-tree [OPTION]... <STDIN>\n\n"

            "Perform various operations using a binary tree.  By default,\n"
            "words are read from stdin and added to the tree, before being\n"
            "printed out alongside their frequencies to stdout.\n\n",

            " -c FILENAME Check spelling of words in FILENAME using words\n"
            "             read from stdin as the dictionary.  Print timing\n"
            "             info & unknown words to stderr (ignore -d & -o)\n"
            " -d          Only print the tree depth (ignore -o)\n",
            " -f FILENAME Write DOT output to FILENAME (if -o given)\n"
            " -o          Output the tree in DOT form to file 'tree-view.dot'\n"
            " -r          Make the tree an RBT (the default is a BST)\n\n"

            " -h          Print this message\n");
}

/**
 * Processes the commandline arguments to determine options.
 * @param argc number of arguments
 * @param argv string of arguments
 */ 
static void options(int argc, char **argv){
    const char *optstring = "c:df:orh";
    char option;

    while ((option = getopt(argc, argv, optstring)) != EOF) {
        switch (option) {
            case 'c':
                spellcheck = 1;
                spellcheck_file = optarg;
                break;
            case 'd':
                print_depth = 1;
                break;
            case 'f':
                dot_file = optarg;
                break;
            case 'o':
                dot = 1;
                break;
            case 'r':
                type = RBT;
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
 *Main file,initialises and fills tree, performs spellcheck if selected.
 * @param argc the number of arguments from terminal.
 * @param argv string of arguments.
 * @return an exit-success notifier.
 */ 
int main(int argc, char **argv){
    /*Declare variables.*/
    FILE *file;
    tree t;
    char word[256];
    clock_t fill_start, fill_end, search_start, search_end;
    int unknown_words;

    /*Set default flags and filenames.*/
    dot_file = "tree-view.dot";
    unknown_words = 0;
    spellcheck = 0;
    print_depth = 0;
    dot = 0;
    type = BST;

    options(argc, argv);
    t = tree_new(type);

    fill_start = clock();
    while (getword(word, sizeof word, stdin) != EOF){
        t = tree_insert(t, word);
    }
    fill_end = clock();
    set_colour(t);

    if(spellcheck>0){
        file = fopen(spellcheck_file,"r");
        if(file != NULL){
            search_start = clock();
            while (getword(word, sizeof word, file) != EOF) {
                if(tree_search(t,word) == 0){
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
        print_depth = 0;
        dot = 0;
    }

    if(print_depth > 0){
        printf("%d\n",tree_depth(t));
        dot = 0;
    }

    if(dot>0){
        file = fopen(dot_file, "a+");
        tree_output_dot(t, file, dot_file);
    }

    tree_free(t);

    return EXIT_SUCCESS;
}
