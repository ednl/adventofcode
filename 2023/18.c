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
 *     cc -std=c17 -Wall -Wextra -pedantic 18.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 18.c
 * Get minimum runtime from timer output in bash:
 *     m=9999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz)                  :  62 µs
 *     Apple M1 Mac Mini 2020 (3.2 GHz)               : 111 µs
 *     Apple iMac 2013 (Core i5 Haswell 4570 3.2 GHz) : 219 µs
 *     Raspberry Pi 5 (2.4 GHz)                       : 229 µs
 *     Raspberry Pi 4 (1.8 GHz)                       : 481 µs
 */

#include <stdio.h>     // fopen, fclose, fscanf, printf
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRId64
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

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
    Dir dir;  // 0..3 = R,D,L,U
    int len;  // must hold 20-bit number for part 2
} Dig;

// Two different trenches for parts 1 & 2
static Dig trench[2][N];

static Dir char2dir(const int c)
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
#ifdef TIMER
    starttimer();
#endif

    FILE *f = fopen(NAME, "r");
    if (!f) { fputs("File not found.\n", stderr); return 1; }
    for (int i = 0, dir, len, hex; i < N && fscanf(f, "%c %2d (#%6x)\n", &dir, &len, &hex) == 3; ++i) {
        trench[0][i] = (Dig){char2dir(dir), len};  // digging instructions for part 1
        trench[1][i] = (Dig){hex & 3, hex >> 4};   // last hex digit: 0=R,1=D,2=L,3=U
    }
    fclose(f);

#if EXAMPLE || defined(DEBUG)
    const char *dir2char = "RDLU";
    for (int i = 0; i < N; ++i) {
        const Dig *const t0 = &trench[0][i];
        const Dig *const t1 = &trench[1][i];
        printf("%c %d | %c %6d\n", dir2char[t0->dir], t0->len, dir2char[t1->dir], t1->len);
    }
    printf("\n");
#endif

    const Dig *t = &trench[0][0];  // start at first trench dig instruction from part 1
    for (int part = 1; part <= 2; ++part) {
        int64_t a = 0, b = 0, y = 0;  // area, border, y-position
        for (int i = 0; i < N; ++i, ++t) {  // trench array must be exactly the right size to seamlessly go from part 1 to 2
            // Shoelace: A = 1/2 . sum((y[i] + y[i+1]).(x[i] - x[i+1]))
            // https://en.wikipedia.org/wiki/Shoelace_formula
            // For two points on horizontal line: y[i] = y[i+1], so y[i] + y[i+1] = 2y
            // For two points on vertical line: x[i] = x[i+1], so x[i] - x[i+1] = 0
            // Bring the 1/2 in the sum: A = sum(y.dx) for horizontal lines only.
            switch (t->dir) {
                case RIGHT: a -= y * t->len; break;  // area shrinks using Shoelace
                case DOWN : y +=     t->len; break;  // adjust position by trench length down (only y needed)
                case LEFT : a += y * t->len; break;  // area grows using Shoelace
                case UP   : y -=     t->len; break;  // adjust position by trench length up (only y needed)
            }
            b += t->len;  // trench length of width 1 = border area
        }
        // Pick: i = A - b/2 + 1, but add border b
        // https://en.wikipedia.org/wiki/Pick%27s_theorem
        // A can be negative, depending on direction of Shoelace
        // example:    62    952408144115
        // input  : 46334 102000662718092
        printf("Part %d: %"PRId64"\n", part, (a >= 0 ? a : -a) + b/2 + 1);
    }

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
}
