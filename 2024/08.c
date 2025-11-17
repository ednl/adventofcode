/**
 * Advent of Code 2024
 * Day 8: Resonant Collinearity
 * https://adventofcode.com/2024/day/8
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 08.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 08.c
 * Get minimum runtime from timer output in bash:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz)                    :  4.67 µs
 *     Mac Mini 2020 (M1 3.2 GHz)                       :  8    µs
 *     Raspberry Pi 5 (2.4 GHz)                         : 19    µs
 *     Macbook Air 2013 (Core i5 Haswell 4250U 1.3 GHz) : 27    µs
 *     Raspberry Pi 4 (1.8 GHz)                         : 63    µs
 */

#include <stdio.h>
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define EXAMPLE 0
#if EXAMPLE
    #define FNAME "../aocinput/2024-08-example.txt"
    #define N 12  // rows and cols of square grid in example file
#else
    #define FNAME "../aocinput/2024-08-input.txt"
    #define N 50  // rows and cols of square grid in input file
#endif
#define FSIZE (N * (N + 1))  // file size in bytes (rows +newline)

#define M 4  // max antenna count per frequency (4 for my input)
#define NUM ('9' - '0' + 1)  // different numbers = 10
#define LET ('Z' - 'A' + 1)  // different letters = 26
#define ALF (NUM + LET)  // different alphanumeric chars = 36
#define FRQ (ALF + LET)  // different frequencies (bandwidth) = 62
#define SPC '.'  // free space

typedef struct vec {
    int x, y;
} Vec;

typedef struct antenna {
    int count;
    Vec pos[M];
} Antenna;

static char map[N][N + 1];      // antenna map (input file)
static Antenna antenna[FRQ];    // antenna positions grouped by frequency
static bool antinode[2][N][N];  // two antinode maps for parts 1 & 2

// Hash antenna name to frequency (=index 0..61)
static int freq(const char name)
{
    if (name <= '9') return name - '0';
    if (name <= 'Z') return name - 'A' + NUM;
    return name - 'a' + ALF;
}

// Position within bounds of map?
static bool ismap(const Vec a)
{
    return a.x >= 0 && a.x < N && a.y >= 0 && a.y < N;
}

// Vector sum a+b
static Vec add(const Vec a, const Vec b)
{
    return (Vec){a.x + b.x, a.y + b.y};
}

// Vector difference a-b (=from b to a)
static Vec sub(const Vec a, const Vec b)
{
    return (Vec){a.x - b.x, a.y - b.y};
}

// Count antinodes on one map of N*N bools
static int sum(const void *const a)
{
    int sum = 0;
    for (int i = 0; i < N * N; ++i)
        sum += ((const bool *const)a)[i];
    return sum;
}

int main(void)
{
    // Read input file as one chunk
    FILE *f = fopen(FNAME, "rb");
    if (!f) { fputs("File not found.\n", stderr); return 1; }
    fread(map, FSIZE, 1, f);
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    // Group antenna positions by frequency
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            if (map[i][j] != SPC) {
                Antenna *const a = &antenna[freq(map[i][j])];
                a->pos[a->count++] = (Vec){j, i};
                antinode[1][i][j] = true;  // antennas are also antinodes in part 2
            }

    for (int i = 0; i < FRQ; ++i) {
        const Antenna *const a = &antenna[i];
        for (int j = 1; j < a->count; ++j)  // for every antenna j in [1..count)
            for (int k = 0; k < j; ++k) {  // consider every unique pair with k in [0..j)
                const Vec diff = sub(a->pos[j], a->pos[k]);  // vector from [k] to [j]
                // One Direction
                Vec pos = add(a->pos[j], diff);  // one step from [j] away from [k]
                if (ismap(pos)) {
                    antinode[0][pos.y][pos.x] = true;  // part 1
                    do {
                        antinode[1][pos.y][pos.x] = true;  // part 2
                        pos = add(pos, diff);  // more steps in same direction
                    } while (ismap(pos));  // until we fall off the map
                }
                // Backstreet Boys (I mean, opposite direction)
                pos = sub(a->pos[k], diff);  // one step from [k] away from [j]
                if (ismap(pos)) {
                    antinode[0][pos.y][pos.x] = true;  // part 1
                    do {
                        antinode[1][pos.y][pos.x] = true;  // part 2
                        pos = sub(pos, diff);  // more steps in same direction
                    } while (ismap(pos));  // until we fall off the map
                }

            }
    }
    printf("%d %d\n", sum(antinode[0]), sum(antinode[1]));  // example: 14 34, input: 244 912

#ifdef TIMER
    printf("Time: %.0f ns\n", stoptimer_ns());
#endif
    return 0;
}
