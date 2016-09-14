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

/*Function declarations*/
int tree_depth(tree t);
void tree_output_dot(tree t, FILE *out);

/**
 * Prints a help notice when "-h" is passed as an argument.
 */
static void help_notice() {
    fprintf(stderr,"\n\n%s%s%s",
            "When running this BST/RBT program from the command line, we have\n"
            "a number of options regarding output and output location.\n"
            "Appending certain tags to our command line argument allows us\n"
            "to make the following changes: \n\n"
            "-c [filename]     Check the spelling of words in filename\n"
            "                  using words read from stdin as the\n"
            "                  dictionary. Print all unknown words to\n",
            "                  stdout. Print timing information and\n"
            "                  unknown word count to stderr. When this\n"
            "                  option is given then the -d/-o options\n"
            "                  is ignored.\n\n"
            "-d                Print the depth of the tree to stdout\n"
            "                  and don’t do anything else.\n\n"
            "-f [filename]     Write the 'dot' output to filename instead of\n"
            "                  the  default filename i -o is given.\n\n"
            "-o                Output a representation of the tree in 'dot'\n",
            "                  form to the file 'tree-view.dot'\n\n"
            "-r                Make the tree an RBT instead of the default\n"
            "                  BST.\n\n"
            "-h                Print this help message describing how to use\n "
            "                  the program.\n\n");
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
    }

    if(dot>0){
        file = fopen(dot_file, "a+");
        tree_output_dot(t, file);
    }

    tree_free(t);

    return EXIT_SUCCESS;
}


