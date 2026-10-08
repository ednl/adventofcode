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
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 1.81 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 2.70 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 6.25 µs
 */

#include <stdio.h>
#include <stdint.h>  // uint8_t, uint64_t
#include <stdbool.h>
#ifdef TIMER
    #include <string.h>  // memset
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2024-08-input.txt"
#define N 50                // rows and cols of square grid in input file
#define SETSIZE ((N * N + 63) >> 6)  // how many u64 in bitset (= 40), don't need N+1 columns here
#define FREQ (26 * 2 + 10)  // size of perfect hash of (0..9,A..Z,a..z)
#define M 4                 // max antennas per frequency, needed for my input: 4

typedef struct vec {
    int x, y;
} Vec;

static char map[N][N + 1];    // +newline = N+1 columns
static Vec antenna[FREQ][M];  // location of every antenna per frequency
static uint8_t count[FREQ];   // how many antennae per frequency
static uint64_t antinode1[SETSIZE];  // part 1
static uint64_t antinode2[SETSIZE];  // part 2

// Vector addition by reference: a += b
static void add_r(Vec *const a, const Vec b)
{
    a->x += b.x;
    a->y += b.y;
}

// Vector subtraction by reference: a -= b
static void sub_r(Vec *const a, const Vec b)
{
    a->x -= b.x;
    a->y -= b.y;
}

// Vector difference: a-b (= go from b to a)
static Vec sub(const Vec a, const Vec b)
{
    return (Vec){a.x - b.x, a.y - b.y};
}

// Hash antenna name to frequency (= index 0..61)
static int freq(const char name)
{
    if (name >= 'a') return name - 'a';       // a..z =>  0..25
    if (name >= 'A') return name - 'A' + 26;  // A..Z => 26..51
    return name - '0' + 52;                   // 0..9 => 52..61
}

// Position within bounds of map?
static bool onmap(const Vec v)
{
    return v.x >= 0 && v.x < N && v.y >= 0 && v.y < N;
}

// Mark antinode in bitset
static void mark(uint64_t *const arr, const Vec pos)
{
    const int index = pos.y * N + pos.x;  // bitset uses N "columns", not N+1
    arr[index >> 6] |= UINT64_C(1) << (index & 63);
}

// Antinodes in two directions
// GCD for part 2 not needed for my input
static void resonate(Vec a, Vec b)
{
    const Vec step = sub(a, b);  // from b to a
    add_r(&a, step);  // one more step away from b
    if (onmap(a)) {
        mark(antinode1, a);
        do {
            mark(antinode2, a);
            add_r(&a, step);
        } while (onmap(a));
    }
    sub_r(&b, step);  // from a to b, and one more step away from a
    if (onmap(b)) {
        mark(antinode1, b);
        do {
            mark(antinode2, b);
            sub_r(&b, step);
        } while (onmap(b));
    }
}

int main(void)
{
    {
        FILE *f = fopen(FNAME, "rb");
        if (!f) { fputs("File not found: "FNAME, stderr); return 1; }
        fread(map, sizeof map, 1, f);
        fclose(f);
    }

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
    memset(count, 0, sizeof count);
#endif

    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            if (map[i][j] >= '0') {
                const int f = freq(map[i][j]);
                antenna[f][count[f]++] = (Vec){j, i};
                mark(antinode2, (Vec){j, i});  // antennae are antinodes in part 2
            }

    // Antenna counts per frequency for my input:
    // a..z: 30404000044044044044400440
    // A..Z: 40404000034044044044400330
    // 0..9: 4444444444

    for (int i = 0; i < FREQ; ++i)
        for (int j = 1; j < count[i]; ++j)
            for (int k = 0; k < j; ++k)
                resonate(antenna[i][j], antenna[i][k]);

    int part1 = 0, part2 = 0;
    for (int i = 0; i < SETSIZE; ++i) {
        part1 += __builtin_popcountll(antinode1[i]);
        part2 += __builtin_popcountll(antinode2[i]);
    }
    printf("%u %u\n", part1, part2);  // 244 912

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
