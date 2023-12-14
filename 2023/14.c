/**
 * Advent of Code 2023
 * Day 14: Parabolic Reflector Dish
 * https://adventofcode.com/2023/day/14
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -Ofast -march-native -Wall 14.c ../startstoptimer.c
 *    gcc   -std=gnu17 -Ofast -march-native -Wall 14.c ../startstoptimer.c
 * Get minimum runtime:
 *     m=50000;for((i=0;i<200;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * Minimum runtime:
 *     Apple M1 Mac Mini      : 23 ms
 *     Raspberry Pi 5 2.4 GHz : 42 ms
 */

#include <stdio.h>    // fopen, fclose, fgets, printf
#include <string.h>   // memset
#include <stdbool.h>  // bool
#include "../startstoptimer.h"

#define EXAMPLE 0
#if EXAMPLE
#define NAME "../aocinput/2023-14-example.txt"
#define N 10
#else
#define NAME "../aocinput/2023-14-input.txt"
#define N 100
#endif

#define LIM (N + 1)
#define DIM (N + 2)
#define CYCLES 1000000000
#define STEADY 100  // how many cycles to reach steady state loop

static char map[DIM][DIM + 2];

// Transpose
static void transpose(void)
{
    for (int i = 1; i < LIM; ++i)
        for (int j = i + 1; j < LIM; ++j) {
            const char tmp = map[i][j];
            map[i][j] = map[j][i];
            map[j][i] = tmp;
        }
}

#if EXAMPLE
static void printmap(void)
{
    for (int i = 0; i < DIM; ++i)
        printf("%s", map[i]);  // string already has '\n'
    printf("\n");
}
#endif

// Roll round rocks all the way to the left or right
static void roll(const bool left)
{
    for (int i = 1; i < LIM; ++i)  // rows
        for (int j = 1; j < LIM; ++j) {  // cols
            while (j < LIM && map[i][j] == '#')  // look for first round rock or empty space
                ++j;
            int end = j, rocks = 0;
            while (end < LIM && map[i][end] != '#')  // look for first square rock
                rocks += map[i][end++] == 'O';  // count round rocks
            const int space = end - j - rocks;
            if (rocks && space) {
                char* dst1 = &map[i][j];
                if (left) {
                    char* dst2 = dst1 + rocks;
                    memset(dst1, 'O', (size_t)rocks);  // round rocks to the left
                    memset(dst2, '.', (size_t)space);  // open space to the right
                } else {
                    char* dst2 = dst1 + space;
                    memset(dst1, '.', (size_t)space);  // open space to the left
                    memset(dst2, 'O', (size_t)rocks);  // round rocks to the rightt
                }
            }
            j = end;
        }
}

// Total load on the North support beams (if N = up)
// Also used as unique configuration ID for cycle detection, which happens to work
static int load(void)
{
    int sum = 0;
    for (int i = 1; i < LIM; ++i) {  // rows
        const int val = LIM - i;
        for (int j = 1; j < LIM; ++j)
            sum += (map[i][j] == 'O') * val;
    }
    return sum;
}

// Roll N,W,S,E (begin and end with N = up)
static void cycle(void)
{
    transpose(); /* N = left */ roll(true);  // roll left  = N
    transpose(); /* N = up   */ roll(true);  // roll left  = W
    transpose(); /* N = left */ roll(false); // roll right = S
    transpose(); /* N = up   */ roll(false); // roll right = E
}

int main(void)
{
    starttimer();
    FILE* f = fopen(NAME, "r");
    if (!f)
        return 1;
    for (int i = 1; i < LIM; ++i)
        fgets(&map[i][1], DIM, f);  // leave room for border
    fclose(f);
    memset(&map[0][1], '#', N);  // top and bottom border not used
    memset(&map[DIM - 1][1], '#', N);
    for (int i = 0; i < DIM; ++i) {
        map[i][0] = '#';
        map[i][DIM - 1] = '#';
        map[i][DIM    ] = '\n';
        map[i][DIM + 1] = '\0';
    }

    // Part 1, begin with N = up
    #if EXAMPLE
        printmap();
    #endif
    transpose(); /* N = left */ roll(true); // roll left = N
    transpose(); /* N = up   */
    #if EXAMPLE
        printmap();
    #endif
    printf("Part 1: %d\n", load());  // example: 136, input: 113525

    // Part 2
    // Complete first cycle from part 1: begin with N = up
    roll(true);  // roll left  = W
    transpose(); /* N = left */ roll(false); // roll right = S
    transpose(); /* N = up   */ roll(false); // roll right = E
    #if EXAMPLE
        printmap();
    #endif

    // Get reference value after 100 cycles
    // (for my input enough to reach steady state loop)
    for (int i = 1; i < STEADY; ++i)  // one cycle already done
        cycle();
    const int ref = load();  // not necessarily unique, but it works

    // How long is the loop?
    int loop = 0;
    do {
        cycle();
        ++loop;
    } while (load() != ref);

    // How many left to reach all teh CYCLES in a whole number of loops?
    const int mod = (CYCLES - STEADY) % loop;
    for (int i = 0; i < mod; ++i)
        cycle();

    #if EXAMPLE
        printmap();
    #endif
    printf("Part 2: %d\n", load());  // example: 64, input: 101292
    printf("Time: %.0f ms\n", stoptimer_ms());
    return 0;
}
