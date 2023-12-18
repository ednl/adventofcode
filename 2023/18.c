/**
 * Advent of Code 2023
 * Day 18: Lavaduct Lagoon
 * https://adventofcode.com/2023/day/18
 * By: E. Dronkert https://github.com/ednl
 *
 * Theory:
 *     https://en.wikipedia.org/wiki/Shoelace_formula
 *     https://en.wikipedia.org/wiki/Pick%27s_theorem
 *
 * Compile:
 *    clang -std=gnu17 -Ofast -march=native -Wall 18.c ../startstoptimer.c
 *    gcc   -std=gnu17 -Ofast -march=native -Wall 18.c ../startstoptimer.c
 * Get minimum runtime:
 *     m=999999;for((i=0;i<5000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * Minimum runtime:
 *     Apple M1 Mac Mini 2020 (3.2 GHz)               :    ? us
 *     Raspberry Pi 5 (2.4 GHz)                       :    ? us
 *     Apple iMac 2013 (Core i5 Haswell 4570 3.2 GHz) :  223 us
 *     Raspberry Pi 4 (1.8 GHz)                       :    ? us
 */

#include <stdio.h>     // fopen, fclose, fscanf, printf
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRId64
#include "../startstoptimer.h"

#define EXAMPLE 0
#if EXAMPLE
#define NAME "../aocinput/2023-18-example.txt"
#define N 14  // number of trench digging instructions in example
#else
#define NAME "../aocinput/2023-18-input.txt"
#define N 766  // number of trench digging instructions in input
#endif

typedef enum dir {
    RIGHT, DOWN, LEFT, UP
} Dir;

typedef struct dig {
    Dir dir;
    int len;
} Dig;

static Dig trench[2][N];  // two different trenches for parts 1 & 2

static Dir char2dir(const char c)
{
    switch (c) {
        case 'R': return RIGHT;
        case 'D': return DOWN;
        case 'L': return LEFT;
        case 'U': return UP;
    }
    return 0;
}

int main(void)
{
    starttimer();
    FILE* f = fopen(NAME, "r");
    char c;
    int len, rgb;
    for (int i = 0; i < N && fscanf(f, " %c %2d (#%6x)", &c, &len, &rgb) == 3; ++i) {
        trench[0][i] = (Dig){char2dir(c), len};
        trench[1][i] = (Dig){rgb & 3, rgb >> 4};
    }
    fclose(f);

    #if EXAMPLE || defined(DEBUG)
    const char *dir2char = "RDLU";
    for (int i = 0; i < N; ++i) {
        const Dig* const t0 = &trench[0][i];
        const Dig* const t1 = &trench[1][i];
        printf("%c %d | %c %6d\n", dir2char[t0->dir], t0->len, dir2char[t1->dir], t1->len);
    }
    printf("\n");
    #endif

    for (int p = 0; p < 2; ++p) {  // parts 1 & 2
        int64_t x = 0, y = 0, a = 0, b = 0;  // position, area, border
        for (int i = 0; i < N; ++i) {
            const Dig* t = &trench[p][i];
            switch (t->dir) {
                case RIGHT: x += t->len; a -= y * t->len; break;  // pos. oriented, but y=-y
                case DOWN : y += t->len; break;
                case LEFT : x -= t->len; a += y * t->len; break;
                case UP   : y -= t->len; break;
            }
            b += t->len;
        }
        // example: 62 952408144115, input: 46334 102000662718092
        printf("Part %d: %"PRId64"\n", p + 1, (a > 0 ? a : -a) + b/2 + 1);
    }
    printf("Time: %.0f us\n", stoptimer_us());
    return 0;
}
