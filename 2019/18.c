///////////////////////////////////////////////////////////////////////////////
////
////  Advent of Code 2019
////  Day 17: Set and Forget, part one & two
////
////  E. Dronkert
////  https://github.com/ednl/aoc2019
////
///////////////////////////////////////////////////////////////////////////////

////////// Includes & Defines /////////////////////////////////////////////////

#ifdef __STDC_ALLOC_LIB__
#define __STDC_WANT_LIB_EXT2__ 1
#else
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdio.h>   // fopen, fgetc, getdelim, printf
#include <stdlib.h>  // atoi, atol
#include <ctype.h>   // isdigit
#include <time.h>    // time for srand
#include <string.h>  // strcpy

#define DEBUG
#define FNLEN 16

#define DIM  81
#define KEYS 26
#define WALL  0
#define PATH  1

////////// Typedefs & Constants ///////////////////////////////////////////////

typedef struct Pos {
    int i, x, y;
} POS, *PPOS;

////////// Globals ////////////////////////////////////////////////////////////

char inputfile[FNLEN];
int maze[DIM * DIM];
POS key[KEYS];
POS door[KEYS];

////////// Function Declarations //////////////////////////////////////////////

////////// Function Definitions ///////////////////////////////////////////////

int aoc_thispuzzle(char **exe)
{
    if (!exe || !*exe)
        return 0;

    char *pc;
    int c;

    pc = *exe;
    while ((c = *pc) && !isdigit(c))
        ++pc;
    return atoi(pc);
}

void aoc_setfilename(int num)
{
    static const char *test = "../aocinput/2019-18-example.txt";
    static const char *fmt = "%02d.txt";

    if (num)
        snprintf(inputfile, FNLEN, fmt, num);
    else
        strcpy(inputfile, test);
}

int xy2i(int x, int y)
{
    return y * DIM + x;
}

POS i2pos(int i)
{
    return (POS){ i, i % DIM, i / DIM };
}

int readmaze(void)
{
    FILE *fp;
    int c, i = 0;

    if ((fp = fopen(inputfile, "r")) != NULL)
    {
        while ((c = fgetc(fp)) != EOF)
            if (c == ',')
                ++i;
        fclose(fp);
    }
    return i;
    /*
    FILE *fp;        // file pointer
    char *s = NULL;  // dynamically allocated buffer
    size_t t = 0;    // size of buffer
    int i = 0;       // values read

    if (vm_cache != NULL && vm_cachesize > 0 &&
        (fp = fopen(vm_filename, "r")) != NULL)
    {
        while (i < vm_cachesize && getdelim(&s, &t, ',', fp) > 0)
            vm_cache[i++] = atol(s);
        free(s);
        fclose(fp);
    }
    return i;
    */
}

////////// Main ///////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    aoc_setfilename(aoc_thispuzzle(argv));
    return 0;
}
