/**
 * Advent of Code 2023
 * Day 1: ?
 * https://adventofcode.com/2023/day/1
 * By: E. Dronkert https://github.com/ednl
 */

#include <stdio.h>   // fopen, fclose, getline, printf
#include <stdlib.h>  // free

int main(void)
{
    int a = 0, b = 0;
    char *buf = NULL;
    size_t bufsz;
    FILE *f = fopen("../aocinput/2023-01-input.txt", "r");
    while (getline(&buf, &bufsz, f) > 1) {
        //TODO
    }
    fclose(f);
    free(buf);
    printf("%d\n%d\n", a, b);  // ? ?
    return 0;
}
