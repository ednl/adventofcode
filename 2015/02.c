/**
 * Advent of Code 2015
 * Day 2: I Was Told There Would Be No Math
 * https://adventofcode.com/2015/day/2
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -O3 -march=native -Wall -Wextra 02.c
 *    gcc   -std=gnu17 -O3 -march=native -Wall -Wextra 02.c
 */

#include <stdio.h>

static inline void swap(unsigned *restrict const a, unsigned *restrict const b)
{
    const unsigned tmp = *a;
    *a = *b;
    *b = tmp;
}

int main(void)
{
    FILE *f = fopen("../aocinput/2015-02-input.txt", "r");
    if (!f) return 1;
    unsigned l, w, h, paper = 0, ribbon = 0;
    while (fscanf(f, "%ux%ux%u", &l, &w, &h) == 3) {
        // Bubble sort l,w,h ascending
        if (l > w) swap(&l, &w);
        if (l > h) swap(&l, &h);
        if (w > h) swap(&w, &h);
        const unsigned A = l * w, B = (l + w) << 1;
        paper  += A * 3 + B * h;  // 2*l*w + 2*l*h + 2*w*h + l*w
        ribbon += A * h + B;      // l*w*h + 2*l + 2*w
    }
    fclose(f);
    printf("%u %u\n", paper, ribbon);  // 1598415 3812909
    return 0;
}
