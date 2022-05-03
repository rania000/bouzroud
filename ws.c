#include <stdio.h>      /* for printf, fprintf*/
#include <stdlib.h>     /* for exit */
#include <stdbool.h>    /* for bool type */
#include <string.h>     /* for strlen, atoi */

/** include external modules **/
#include "options.h"
#include "reader.h"
#include "hashtable.h"
#include "holdall.h"

/** global declaration **/
typedef struct 
{
    char* motif;
    long int *occur;
}wordInfo;


bool *u;         // uppercasing
bool *p;         // punctuation
bool *s;         // same-numbers
int *init;       // initial
int *t;          // top
int *startIndexFiles; // option index
int *nbFiles;    // number of files


/** Function prototype **/

//  str_hashfun : l'une des fonctions de pré-hachage conseillées par Kernighan
//    et Pike pour les chaines de caractères.
static size_t str_hashfun(const char *s);

//  scptr_display : affiche sur la sortie standard la chaine de caractères 
//    pointée par (info->motif), tabulation, l'entier *(info->occur), 
//    tabulation, le mot et la fin de ligne.
//    Renvoie zéro en cas de succès, une valeur non nulle en cas d'échec.
static int scptr_display(const char *s, const wordInfo *info);

//  rfree : libère la zone mémoire pointée par ptr et renvoie zéro.
static int rfree(void *ptr);


/** Main function **/

int main(int argc, char **argv)
{   
    // returned value for main function 
    int r = EXIT_SUCCESS;

    // allocate space for those variables
    u = (bool*)malloc(sizeof(bool));              // uppercasing
    p = (bool*)malloc(sizeof(bool));              // punctuation
    s = (bool*)malloc(sizeof(bool));              // same-numbers
    init = (int*)malloc(sizeof(int));              // initial
    t = (int*)malloc(sizeof(int));                 // top
    startIndexFiles = (int*)malloc(sizeof(int));   // option index
    nbFiles = (int*)malloc(sizeof(int));           // number of files


    /*** Parse options ***/
    getOptions(argc, argv, u, p, s, init, t, startIndexFiles, nbFiles); // will put options in opts structure


    /*** Search for words ***/
    // hashtable variable definition
    hashtable *ht = hashtable_empty((int (*)(const void *, const void *))strcmp,
    (size_t (*)(const void *))str_hashfun);
    holdall *has = holdall_empty();

    // declaration of variables
    char word[(*init)+1];                  // +1 for termination character
    int status = 0;
    FILE *file = NULL;

    // check if hashtable is created
    if (ht == NULL
        || has == NULL) {
        goto error_capacity;
    }


    // indexFiles is the starting position of filenames in argv
    for(int i = *startIndexFiles; i < argc; i++)
    {  
        // if filename is "-" read from standard input file (stdin)
        if(strcmp(argv[i], "-") == 0)
        {
            // stdin is the file attributed to keyboard
            file = stdin;

            // so as to reuse stdin after presing ctrl+d
            rewind(file);
        }
        else
        {
            // check wheter the file exist
            if(fileExist(argv[i]) == false)
            {
                fprintf(stderr, "%s: Can\'t open for reading file '%s'.\n", argv[0], argv[i]);
                exit(EXIT_FAILURE);
            }

            // open file in read mode
            file = fopen(argv[i], "r");
        }

        // read words from file or stdin
        while(1)
        {
            // if status = -1 : EOF or ctrl+D is found, else 1 
            status = readWord(file, word, init, u, p);       

            size_t len = strlen(word);

            if(len > 0)
            {
                // insert gotten word into hashtable
                // or if already found increment its occur
                // and mark its existance in file i-startIndexFiles

                wordInfo *info = (wordInfo*) hashtable_search(ht, word);

                if(info != NULL){
                    *(info->occur) += 1;
                    (info->motif)[i-*startIndexFiles] = 'x';
                } 
                else
                {
                    char *s = malloc(len + 1);
                    if (s == NULL) {
                        goto error_capacity;
                    }
                    strcpy(s, word);
                    if (holdall_put(has, s) != 0) {
                        free(s);
                        goto error_capacity;
                    }

                    info = (wordInfo *)malloc(sizeof *info);
                    if (info == NULL) {
                        goto error_capacity;
                    }

                    info->occur = (long int*)malloc(sizeof *info);
                    info->motif = (char*)malloc((size_t)(*nbFiles+1) * sizeof(char));
                    
                    if (info->occur == NULL || info->motif == NULL) {
                        goto error_capacity;
                    }

                    // clear the whole string with \0
                    memset(info->motif, '\0', (size_t)(*nbFiles+1));
                    // fill string with - character
                    memset(info->motif, '-', (size_t)(*nbFiles));
                    //initialize counter
                    *(info->occur) = 1;

                    if (hashtable_add(ht, s, info) == NULL) {
                        goto error_capacity;
                    }
                }
            }

            // end of file is reached
            if(status == -1)
            {
                break;
            }
        }
        
        // close file
        if(file != stdin)
        {
            fclose(file);
        }

    }

    // print all elements of hashtable
    if (holdall_apply_context(has,
    ht, (void *(*)(void *, void *))hashtable_search,
    (int (*)(void *, void *))scptr_display) != 0) 
    { 
        goto error_write;
    }

    // sorting the hashtable based on the three criteria
    // incompleted

    goto dispose;

    error_capacity:
        fprintf(stderr, "*** Error: Not enough memory.\n");
        goto error;

    error_write:
        fprintf(stderr, "*** Error: A write error occurs.\n");
        goto error;

    error:
        r = EXIT_FAILURE;
        goto dispose;

    dispose:
        hashtable_dispose(&ht);
        if (has != NULL) {
            holdall_apply(has, rfree);
        }
        holdall_dispose(&has);

  return r;
}

/** Definition of functions **/

size_t str_hashfun(const char *s) {
  size_t h = 0;
  for (const unsigned char *p = (const unsigned char *) s; *p != '\0'; ++p) {
    h = 37 * h + *p;
  }
  return h;
}

int scptr_display(const char *s, const wordInfo *info) {
    // print just shared words which have more than 
    // one 'x' in info->motif
    // *t to limit the printed words

    int count = 0;

    for(int i=0; i < *nbFiles; i++)
    {
        if( (info->motif)[i] == 'x')
        {
            count++;

            if(count >= 2)
                break;
        } 
    }

    if(count >= 2 && *t > 0)
    {
        *t = (*t) -1;
        return printf("%s\t%ld\t%s\n", info->motif, *(info->occur), s) < 0;
    }
    else
        return 0;
}

int rfree(void *ptr) {
  free(ptr);
  return 0;
}