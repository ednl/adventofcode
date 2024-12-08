/**
 * Advent of Code 2024
 * Day 8: Resonant Collinearity
 * https://adventofcode.com/2024/day/8
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -Wall -Wextra 08.c
 *    gcc   -std=gnu17 -Wall -Wextra 08.c
 * Enable timer:
 *    clang -DTIMER -O3 -march=native 08.c ../startstoptimer.c
 *    gcc   -DTIMER -O3 -march=native 08.c ../startstoptimer.c
 * Get minimum runtime from timer output:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Mac Mini 2020 (M1 3.2 GHz)                       :  8 µs
 *     Raspberry Pi 5 (2.4 GHz)                         :  ? µs
 *     Raspberry Pi 4 (1.8 GHz)                         :  ? µs
 *     Macbook Air 2013 (Core i5 Haswell 4250U 1.3 GHz) :  ? µs
 */

#include <stdio.h>
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define EXAMPLE 0
#if EXAMPLE
    #define FNAME "../aocinput/2024-08-example.txt"
    #define N 12
#else
    #define FNAME "../aocinput/2024-08-input.txt"
    #define N 50
#endif
#define FSIZE (N * (N + 1))

#define M 4  // max antenna count per frequency (4 for my input)
#define NUM ('9' - '0' + 1)  // different numbers = 10
#define LET ('Z' - 'A' + 1)  // different letters = 26
#define ALF (NUM + LET)  // different alphanumeric chars = 36
#define FRQ (ALF + LET)  // different frequencies (bandwidth) = 62
#define SPC '.'

typedef struct vec {
    int x, y;
} Vec;

typedef struct antenna {
    int count;
    Vec pos[M];
} Antenna;

static char map[N][N + 1];      // antenna map (input file)
static Antenna antenna[FRQ];    // antenna positions per frequency
static bool antinode[2][N][N];  // two antinode maps for part 1 & 2

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
    // Read input file
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
                antinode[1][i][j] = true;  // part 2
            }

    for (int i = 0; i < FRQ; ++i) {
        const Antenna *const a = &antenna[i];
        for (int j = 0; j < a->count; ++j)
            for (int k = 0; k < j; ++k) {
                const Vec dif = sub(a->pos[j], a->pos[k]);
                // One Direction
                Vec pos = add(a->pos[j], dif);
                if (ismap(pos)) {
                    antinode[0][pos.y][pos.x] = true;  // part 1
                    do {
                        antinode[1][pos.y][pos.x] = true;  // part 2
                        pos = add(pos, dif);
                    } while (ismap(pos));
                }
                // Backstreet Boys
                pos = sub(a->pos[k], dif);
                if (ismap(pos)) {
                    antinode[0][pos.y][pos.x] = true;  // part 1
                    do {
                        antinode[1][pos.y][pos.x] = true;  // part 2
                        pos = sub(pos, dif);
                    } while (ismap(pos));
                }

            }
    }
    printf("%d %d\n", sum(antinode[0]), sum(antinode[1]));  // example: 14 34, input: 244 912

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
    return 0;
}
