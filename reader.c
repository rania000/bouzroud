#include <stdio.h>      /* for printf, fprintf*/
#include <stdlib.h>     /* for exit */
#include <stdbool.h>    /* for bool type */
#include <ctype.h>      /* for isdigit, ispunc, isspace, toupper */

#include "reader.h"


int readWord(FILE* file, char* word, int* init, bool* u, bool* p)
{
    char c = 0;
    int i = 0;

    for(i = 0; i < *init ; i++)
    {
        c = (char)fgetc(file);

        if(isspace(c) || c == EOF) // isspace is comparing with \n \t \r ' '
        {
            break;
        }
        
        if(*p == true)
        {
            if(ispunct(c))
            {
                break;
            }
        }

        if(*u == true)
        {
            c = (char)toupper(c);
        }

        word[i] = c;
    }

    word[i] = '\0';     // close word array 

    // ignore the rest of characters, till reaching space or EOF
    while(isspace(c) == false && c != EOF)
    {
        c = (char)fgetc(file);
    }

    // reahing the end of file
    if(c == EOF)
        return -1;

    return 1;
}