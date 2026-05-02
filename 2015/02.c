/**
 * Advent of Code 2015
 * Day 2: I Was Told There Would Be No Math
 * https://adventofcode.com/2015/day/2
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 02.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 02.c
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements, includes reading from disk:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  59 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :  93 µs
 *     Raspberry Pi 5 (2.4 GHz)      :   ? µs
 */

#include <stdio.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

static void swap(unsigned *restrict const a, unsigned *restrict const b)
{
    const unsigned tmp = *a;
    *a = *b;
    *b = tmp;
}

int main(void)
{
    FILE *f = fopen("../aocinput/2015-02-input.txt", "r");
    if (!f) return 1;

#ifdef TIMER
    starttimer();
#endif

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

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
}
