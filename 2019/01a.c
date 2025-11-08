///////////////////////////////////////////////////////////////////////////////
////
////  Advent of Code 2019
////  Day 1: The Tyranny of the Rocket Equation, part one
////
////  E. Dronkert
////  https://github.com/ednl/aoc2019
////
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>   // fopen, fgets, printf
#include <stdlib.h>  // atoi

#define BUFLEN 16

const char *inp = "../aocinput/2019-01-input.txt";

int main(void)
{
    FILE *fp;
    char line[BUFLEN];
    int i, a = 0;

    if ((fp = fopen(inp, "r")) != NULL)
        while (fgets(line, sizeof line, fp) != NULL)
            if ((i = atoi(line) / 3 - 2) > 0)
                a += i;

    printf("Fuel: %u\n", a);
    return 0;
}
