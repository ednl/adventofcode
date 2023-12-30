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
 *    clang -std=gnu17 -Ofast -march=native -Wall -Wextra 18.c ../startstoptimer.c
 *    gcc   -std=gnu17 -Ofast -march=native -Wall -Wextra 18.c ../startstoptimer.c
 * Get minimum runtime:
 *     m=999999;for((i=0;i<5000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * Minimum runtime:
 *     Apple M1 Mac Mini 2020 (3.2 GHz)               : 111 µs
 *     Apple iMac 2013 (Core i5 Haswell 4570 3.2 GHz) : 219 µs
 *     Raspberry Pi 5 (2.4 GHz)                       : 229 µs
 *     Raspberry Pi 4 (1.8 GHz)                       : 481 µs
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
    RIGHT=0, DOWN, LEFT, UP
} Dir;

typedef struct dig {
    Dir dir;  // R=0,D,L,U
    int len;  // must hold 20-bit number for part 2
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
    if (!f) { fputs("File not found.\n", stderr); return 1; }

    char c;
    for (int i = 0, k, x; i < N && fscanf(f, " %c %2d (#%6x)", &c, &k, &x) == 3; ++i) {
        trench[0][i] = (Dig){char2dir(c), k};  // digging instructions for part 1
        trench[1][i] = (Dig){x & 3, x >> 4};   // last hex digit of x: 0=R,1=D,2=L,3=U
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

    const Dig* t = &trench[0][0];
    for (int part = 1; part <= 2; ++part) {
        int64_t x = 0, y = 0, a = 0, b = 0;  // position, area, border
        for (int i = 0; i < N; ++i, ++t) {
            // Shoelace: A = 1/2 . sum((y[i] + y[i+1]).(x[i] - x[i+1]))
            // For two points on horizontal line: y[i] = y[i+1], so y[i] + y[i+1] = 2y
            // For two points on vertical line: x[i] = x[i+1], so x[i] - x[i+1] = 0
            // Bring the 1/2 in the sum: A = sum(y.dx) for horizontal lines only
            switch (t->dir) {
                case RIGHT: x += t->len; a -= y * t->len; break;
                case DOWN : y += t->len; break;
                case LEFT : x -= t->len; a += y * t->len; break;
                case UP   : y -= t->len; break;
            }
            b += t->len;
        }
        // Pick: i = A - b/2 + 1, but add border b
        // example:    62    952408144115
        // input  : 46334 102000662718092
        printf("Part %d: %"PRId64"\n", part, (a > 0 ? a : -a) + b/2 + 1);
    }

    printf("Time: %.0f us\n", stoptimer_us());
    return 0;
}
