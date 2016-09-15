#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "htable.h"
#include "mylib.h"

/**
   num_keys is the number of keys the htable is currnetly holding.
   capacity is how amny keys the htable can hold.
   *frequencies stores the frequencies of different keys.
   *stats stores the number of collisions before an empty space was found.
   **keys stores the keys
   method is either linear probing or double hashing.
**/
struct htablerec{
    int num_keys;
    int capacity;
    int *frequencies;
    int *stats;
    char **keys;
    hashing_t method;
};

/**
   Creates a new htable, frequencies array and stats array.
   Decides what collision method is used.
   Allocates memory and sets their values to null.
   @param capacity how amny keys the htable can store
   @param hash_type either linear probing or double hashing
   @return the created htable
**/
htable htable_new(int capacity, hashing_t hash_type){
    int i;
    htable newhtable = emalloc(sizeof *newhtable);
    newhtable->method = hash_type;
    newhtable->capacity = capacity;
    newhtable->num_keys = 0;
    newhtable->frequencies = emalloc(newhtable->capacity * sizeof
                                     newhtable->frequencies[0]);
    newhtable->stats = emalloc(newhtable->capacity * sizeof
                               newhtable->stats[0]);
    newhtable->keys = emalloc(newhtable->capacity * sizeof
                              newhtable->keys[0]);                                
    for(i=0;i<capacity;i++){
        newhtable->frequencies[i] = 0;
        newhtable->stats[i] = 0;
        newhtable->keys[i] = NULL;
    }
    return newhtable;
}

/**
   Frees all memory allocated to the htable.
   @param h the htable to be freed
**/
void htable_free(htable h){
    int i;	
    for(i=0;i<h->capacity;i++){
        free(h->keys[i]);
    }	
    free(h->frequencies);
    free(h->keys);
    free(h->stats);
    free(h);
}

/**
   Converts a given word to an integer.
   @param *word the word to be converted
   @return the int that has been made
**/
static unsigned int htable_word_to_int(char *word) {
    unsigned int result = 0;	
    while (*word != '\0') {
        result = (*word++ + 31 * result);
    }
    return result;
}

/**
   Figures out the step for double hashing.
   @param h the htable
   @param i_key the number converted from a string
   @return the step value
**/
static unsigned int htable_step(htable h, unsigned int i_key) {
    return 1 + (i_key % (h->capacity - 1));
}

/**
   Inserts a string into the htable.
   @param h the htable to be inserted into
   @param *str the string to be insertes
   @return the frequency of the string 0 if hatble is full
**/
int htable_insert(htable h, char *str){
    unsigned int strvalue = htable_word_to_int(str);
    unsigned int keyaddress = strvalue % h->capacity;
    unsigned int step = 1;                    	
    int i = 0;
    if(h->method == DOUBLE_H){
        step = htable_step(h, strvalue);
    }
    while(i<h->capacity){
        if(h->keys[keyaddress] == NULL){
            h->keys[keyaddress] = emalloc((strlen(str)+1)*sizeof str[0]);
            strcpy(h->keys[keyaddress], str);
            h->frequencies[keyaddress]++;
            h->stats[h->num_keys] = i;
            h->num_keys++;
            return h->frequencies[keyaddress];
        } else if(strcmp(str, h->keys[keyaddress])==0){
            h->frequencies[keyaddress]++;
            return h->frequencies[keyaddress];
        } else{
            keyaddress+=step;
            keyaddress%= h->capacity;
            i++;
        }
    }
    return 0;
}

/**
   Prints the htable index, frequencies, stats and keys.
   @param h the table to be printed
 **/
void htable_print_entire_table(htable h){
    int i;
    printf("  Pos  Freq  Stats  Word\n");
    printf("----------------------------------------\n");
        for(i=0;i<h->capacity;i++){
            if(h->frequencies[i] > 0){
                printf("%5d %5d %5d   %s\n", i, h->frequencies[i],
                       h->stats[i],h->keys[i]);
            }else{
                printf("%5d %5d %5d   %s\n", i, h->frequencies[i],
                       h->stats[i], "");
            }
        }
}

/**
   Searches the htable for a spicific string.
   @param h the htable to be searched
   @param *str the string to be searched for
   @return the fthe ammount of times the string has been stored
**/
int htable_search(htable h, char *str){
    unsigned int strvalue = htable_word_to_int(str);
    unsigned int keyaddress = strvalue % h->capacity;
    unsigned int step = 1;
    int i = 0;
    if(h->method == DOUBLE_H){
        step = htable_step(h, strvalue);
    }
    while(i<h->capacity && (h->frequencies[keyaddress] >0 &&
                            strcmp(h->keys[keyaddress],str)!=0)){
        i++;
        keyaddress=(keyaddress+step)% h->capacity;
    }
    if(i == h->capacity){
        return 0;
    }else{
        return h->frequencies[keyaddress];
    }
}

/**
 * Prints out a line of data from the hash table to reflect the state
 * the table was in when it was a certain percentage full.
 * Note: If the hashtable is less full than percent_full then no data
 * will be printed.
 *
 * @param h - the hash table.
 * @param stream - a stream to print the data to.
 * @param percent_full - the point at which to show the data from.
 */
static void print_stats_line(htable h, FILE *stream, int percent_full) {
    int current_entries = h->capacity * percent_full / 100;
    double average_collisions = 0.0;
    int at_home = 0;
    int max_collisions = 0;
    int i = 0;
    if (current_entries > 0 && current_entries <= h->num_keys) {
        for (i = 0; i < current_entries; i++) {
            if (h->stats[i] == 0) {
                at_home++;
            } 
            if (h->stats[i] > max_collisions) {
                max_collisions = h->stats[i];
            }
            average_collisions += h->stats[i];
        }    
        fprintf(stream, "%4d %10d %10.1f %10.2f %11d\n", percent_full, 
                current_entries, at_home * 100.0 / current_entries,
                average_collisions / current_entries, max_collisions);
    }
}

/**
 * Prints out a table showing what the following attributes were like
 * at regular intervals (as determined by num_stats) while the
 * hashtable was being built.
 *
 * @li Percent At Home - how many keys were placed without a collision
 * occurring.
 * @li Average Collisions - how many collisions have occurred on
 *  average while placing all of the keys so far.
 * @li Maximum Collisions - the most collisions that have occurred
 * while placing a key.
 *
 * @param h the hashtable to print statistics summary from.
 * @param stream the stream to send output to.
 * @param num_stats the maximum number of statistical snapshots to print.
 */
void htable_print_stats(htable h, FILE *stream, int num_stats) {
    int i;
    fprintf(stream, "\n%s\n\n", 
            h->method == LINEAR_P ? "Linear Probing" : "Double Hashing"); 
    fprintf(stream, "Percent   Current   Percent    Average      Maximum\n");
    fprintf(stream, " Full     Entries   At Home   Collisions   Collisions\n");
    fprintf(stream, "-----------------------------------------------------\n");
    for (i = 1; i <= num_stats; i++) {
        print_stats_line(h, stream, 100 * i / num_stats);
    }
    fprintf(stream, "-----------------------------------------------------\n\n");
}




