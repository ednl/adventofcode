/**
 * Advent of Code 2018
 * Day 5: Alchemical Reduction
 * https://adventofcode.com/2018/day/5
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 05.c
 * Enable timer:
 *     cc -std=gnu17 -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 05.c
 * Get minimum runtime from timer output:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 279 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :   ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : 863 µs
*/

#include <stdio.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2018-05-input.txt"
#define FSIZE (50 * 1000)  // characters in input file, excludes newline
#define REDUCED (1 << 14)  // 16384, needed for my input: 11126

static char input[FSIZE];
static char stack1[REDUCED];
static char stack2[REDUCED];

// Reduce input (src) of size 'len' onto stack (dst)
// 'skip' is lowercase char a-z
static int reduce(char *dst, const char *src, const int len, const int skip)
{
    int n = 0;
    for (int i = 0; i < len; )
        if ((src[i] | 32) == skip)
            i++;
        else if (n > 0 && (dst[n - 1] ^ src[i]) == 32) {
            n--;
            i++;
        } else
            dst[n++] = src[i++];
    return n;
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    fread(input, sizeof input, 1, f);
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    // Part 1
    const int n = reduce(stack1, input, FSIZE, 0);
    printf("%d\n", n);  // 11118

    // Part 2
    int min = n;
    for (int skip = 'a', r; skip <= 'z'; ++skip)
        if ((r = reduce(stack2, stack1, n, skip)) < min)
            min = r;
    printf("%d\n", min);  // 6948

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
}
