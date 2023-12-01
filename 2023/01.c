/**
 * Advent of Code 2023
 * Day 1: ?
 * https://adventofcode.com/2023/day/1
 * By: E. Dronkert https://github.com/ednl
 */

#include <stdio.h>   // fopen, fclose, getline, printf
#include <stdlib.h>  // free
#include <ctype.h>   // isdigit

static const char* name[] = {"zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine"};
static const int namelen[] = {4, 3, 3, 5, 4, 4, 3, 5, 5, 4};

int main(void)
{
    int a = 0, b = 0;
    char *buf = NULL;
    size_t bufsz;
    ssize_t len;
    FILE *f = fopen("../aocinput/2023-01-input.txt", "r");
    while ((len = getline(&buf, &bufsz, f)) > 1) {
        ssize_t i = 0, j = len - 1;
        while (i < len && !isdigit(buf[i]))
            ++i;
        while (j > 0 && !isdigit(buf[j]))
            --j;
        if (i <= j) {
            // printf("%c%c %s", buf[i], buf[j], buf);
            a += (buf[i] - '0') * 10 + buf[j] - '0';
        }
    }
    fclose(f);
    free(buf);
    printf("%d\n%d\n", a, b);  // 54630 ?

    return 0;
}
