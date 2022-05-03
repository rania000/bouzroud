#include <stdio.h>      /* for printf, fprintf*/
#include <stdlib.h>     /* for exit */
#include <getopt.h>     /* for getopt_long , offers the following variables : optarg, optind, optopt */
#include <stdbool.h>    /* for bool type */
#include <string.h>     /* for strlen, atoi */
#include <ctype.h>      /* for isdigit, toupper */

#include "options.h"

#define DEFAULT_INIT    63
#define DEFAULT_UPPER   false
#define DEFAULT_PUNC    false
#define DEFAULT_SUPP    false
#define DEFAULT_TOP     10


void getOptions(int argc, char** argv, bool* u, bool* p, bool* s, int* init, int* t, int* startIndexFiles, int* nbFiles)
{
    int c;                  // stores the option character
    int num = 0;            // stores the argument number
    int option_index = 0;   // index of long option structure

    // initialize option structure with default values
    *u = DEFAULT_UPPER;         // uppercasing
    *p = DEFAULT_PUNC;          // punctuation
    *s = DEFAULT_SUPP;          // same-numbers
    *init = DEFAULT_INIT;       // initial
    *t = DEFAULT_TOP;           // top

    static struct option long_options[] = {
        {"help",                    no_argument,       0, 'h' },
        {"uppercasing",             no_argument,       0, 'u' },
        {"punctuation-like-space",  no_argument,       0, 'p' },
        {"same-numbers",            no_argument,       0, 's' },
        {"initial",                 required_argument, 0, 'i' },
        {"top",                     required_argument, 0, 't' },
        {0,                         0                , 0,  0  }     // close the structure
    };

    while (1) {
    
        c = getopt_long(argc, argv, ":i:psut:", long_options, &option_index);

        // no more options to treat
        if (c == -1)
            break;

        switch (c) {
            case 'h':
                help();
                exit(EXIT_SUCCESS);
                break;
            case 'u':
                *u = true;
                break;

            case 'p':
                *p = true;
                break;

            case 's':
                *s = true;
                break;

            // if(c == 'i' || c == 't')
            case 'i': // i and t opts take argument that must be number
            case 't':
                num = isNumber(optarg); // return -1 if not number

                if(num == -1)
                {
                    fprintf(stderr, "%s: Invalid argument '-%c'.\n"
                        "Try `%s --help' for more information.\n", argv[0], c, argv[0]);
                    exit(EXIT_FAILURE);
                } 

                if(c == 'i')
                    *init = num;
                else
                    *t = num;

                break;

            case ':':   // argument not found

                fprintf(stderr, "%s: Missing argument '-%c'.\n"
                    "Try `%s --help' for more information.\n", argv[0], optopt, argv[0]);
                exit(EXIT_FAILURE);
                break;

            case '?':
                if(optopt == '?')
                {
                    help();
                    exit(EXIT_SUCCESS);
                }

                fprintf(stderr, "%s: Unrecognized option '-%c'.\n"
                    "Try `%s --help' for more information.\n", argv[0], optopt, argv[0]);
                exit(EXIT_FAILURE);
                break;
        }
    }

    // Verify if at least two non-option argument exists
    *nbFiles = argc - optind;

    if(*nbFiles < 2)
    {
        fprintf(stderr, "%s: At least 2 files are expected.\n"
            "Try `%s --help' for more information.\n", argv[0], argv[0]);
        exit(EXIT_FAILURE);
    }

    // the optind is a variable that is pointing on the following
    // non option argument in the argv array
    *startIndexFiles = optind;

}

bool fileExist(const char* fileName)
{
    FILE *file;
    if ((file = fopen(fileName, "r")) != NULL)
    {
        fclose(file);
        return true;
    }
    return false;
}

int isNumber(char *argument)
{
    size_t length = strlen(argument);

    for(size_t i=0; i < length ;i++)
    {
        if(isdigit(argument[i]) == false)
            return -1;
    }

    int num = atoi(argument); // convert string to integer
    return num;
}

void help()
{
    printf(
"Usage: ws [OPTION]... FILES\n\n"

"Print a list of words shared by text files.\n\n"

"Mandatory arguments to long opts are mandatory for short opts too.\n\n"

"  -i, --initial=VALUE    Set the maximal number of significant initial letters\n"
"                    for words to VALUE. 0 means without limitation. Default is\n"
"                    63.\n\n"

"  -p, --punctuation-like-space    Make the punctuation characters play the same\n"
"                    role as space characters in the meaning of words.\n\n"

"  -s, --same-numbers    Print more words than the limit in case of same numbers.\n\n"

"  -t, --top=VALUE   Set the maximal number of words to print to VALUE. 0 means\n"
"                    all the words. Default is 10.\n\n"

"  -u, --uppercasing    Convert each lowercase letter of words to the\n"
"                    corresponding uppercase letter.\n\n"

"  -?, --help        Print this help message and exit.\n\n"

"      --man         Print this help message following man-style and exit.\n\n"

"      --usage       Print a short usage message and exit.\n\n"

"      --version     Print version information.\n\n"

"The number of FILES that can be taken into account is between 2 and 32. The\n"
"counters of the total number of occurrences of the words have a maximum value of\n"
"9223372036854775807.\n"
);
}