/**
 * Advent of Code 2023
 * Day 11: Cosmic Expansion
 * https://adventofcode.com/2023/day/11
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -Ofast -march-native -Wall 11alt.c ../startstoptimer.c
 *    gcc   -std=gnu17 -Ofast -march-native -Wall 11alt.c ../startstoptimer.c
 * Get minimum runtime:
 *     m=50000;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * Minimum runtime:
 *     Apple M1 Mac Mini      : 36 us
 *     Raspberry Pi 5 2.4 GHz : 70 us
 */

#include <stdio.h>     // fopen, fclose, fgets, printf
#include <stdlib.h>    // malloc, free
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRId64
#include "../startstoptimer.h"  // timer

#define EXAMPLE 0
#if EXAMPLE
#define NAME "../aocinput/2023-11-example.txt"
#define N 10   // rows and columns in image map
#define M 100  // empty space expansion factor
#else
#define NAME "../aocinput/2023-11-input.txt"
#define N 140      // rows and columns in image map
#define M 1000000  // empty space expansion factor
#endif

static char image[N][N + 2];
static int xcount[N], ycount[N], xshift[N], yshift[N];

// Nifty algorithm by /u/NikitaSkybytskyi
// https://www.reddit.com/r/adventofcode/comments/18fqxuq/an_on_algorithm_for_day_11/kcvwlev/
static int64_t dist(int* pos, int* shift, int n, int64_t f)
{
    int64_t sum = 0, partial = 0;
    for (int i = 0; i < n; ++i) {
        const int64_t exp = (f - 1) * shift[pos[i]] + pos[i];
        sum += i * exp - partial;
        partial += exp;
    }
    return sum;
}

int main(void)
{
    starttimer();
    FILE* f = fopen(NAME, "r");
    if (!f)
        return 1;
    for (int i = 0; i < N; ++i)
        fgets(image[i], sizeof *image, f);
    fclose(f);

    int galaxies = 0;
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            if (image[i][j] == '#') {
                ++galaxies;
                ++xcount[j];
                ++ycount[i];
            }

    int* xpos = malloc((size_t)galaxies * sizeof *xpos);
    int* ypos = malloc((size_t)galaxies * sizeof *ypos);
    for (int i = 0, k = 0; i < N; ++i)
        for (int j = 0; j < xcount[i]; ++j)
            xpos[k++] = i;
    for (int i = 0, k = 0; i < N; ++i)
        for (int j = 0; j < ycount[i]; ++j)
            ypos[k++] = i;

    xshift[0] = !xcount[0];
    yshift[0] = !ycount[0];
    for (int i = 1; i < N; ++i) {
        xshift[i] = xshift[i - 1] + !xcount[i];
        yshift[i] = yshift[i - 1] + !ycount[i];
    }

    printf("Part 1: %"PRId64"\n", dist(xpos, xshift, galaxies, 2) + dist(ypos, yshift, galaxies, 2));
    printf("Part 2: %"PRId64"\n", dist(xpos, xshift, galaxies, M) + dist(ypos, yshift, galaxies, M));

    free(xpos);
    free(ypos);
    printf("Time: %.0f us\n", stoptimer_us());
    return 0;
}
