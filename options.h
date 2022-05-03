#ifndef OPTIONS
#define OPTIONS

#include <stdbool.h>    /* for bool type */

void help();

bool fileExist(const char* fileName);

int isNumber(char *optarg);

void getOptions(int argc, char** argv, bool* u, bool* p, bool* s, int *init, int * t, int* startIndexFiles, int* nbFiles);

#endif