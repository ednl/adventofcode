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
 *     Raspberry Pi 5 2.4 GHz : 47 ms
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
static char rot[DIM][DIM + 2];

// Rotate anti-clockwise
static void rot90(void)
{
    memcpy(rot, map, sizeof map);
    for (int i = 1; i < LIM; ++i)
        for (int j = 1; j < LIM; ++j)
            map[i][j] = rot[j][LIM - i];  // transpose and mirror horiz
}

// Rotate clockwise
static void rot270(void)
{
    memcpy(rot, map, sizeof map);
    for (int i = 1; i < LIM; ++i)
        for (int j = 1; j < LIM; ++j)
            map[i][j] = rot[LIM - j][i];  // transpose and mirror vert
}

// Rotate 180 degrees
static void rot180(void)
{
    memcpy(rot, map, sizeof map);
    for (int i = 1; i < LIM; ++i)
        for (int j = 1; j < LIM; ++j)
            map[i][j] = rot[LIM - i][LIM - j];  // mirror horiz and vert
}

#if EXAMPLE
static void printmap(void)
{
    for (int i = 0; i < DIM; ++i)
        printf("%s", map[i]);  // string already has '\n'
    printf("\n");
}
#endif

// Roll round rocks all the way to the left
static void roll(void)
{
    for (int i = 1; i < LIM; ++i)  // rows
        for (int j = 1; j < LIM; ++j) {  // cols
            while (j < LIM && map[i][j] != '.')  // look for first open space
                ++j;
            int end = j + 1, rocks = 0;
            while (end < LIM && map[i][end] != '#')  // look for first square rock
                rocks += map[i][end++] == 'O';  // count round rocks
            if (rocks) {
                char* dst1 = &map[i][j];
                char* dst2 = dst1 + rocks;
                const int space = end - j - rocks;
                memset(dst1, 'O', (size_t)rocks);  // round rocks to the left
                memset(dst2, '.', (size_t)space);  // open space to the right
            }
            j = end;
        }
}

// Total load on the South support beams (= North, if N = down)
// Also used as unique configuration ID, which happens to work
static int load(void)
{
    int sum = 0;
    for (int i = 1; i < LIM; ++i)
        for (int j = 1; j < LIM; ++j)
            sum += (map[i][j] == 'O') * i;
    return sum;
}

// Roll N,W,S,E (begin and end with N = down)
static void cycle(void)
{
    rot270(); /* N = left  */ roll(); // roll left = N
    rot270(); /* N = up    */ roll(); // roll left = W
    rot270(); /* N = right */ roll(); // roll left = S
    rot270(); /* N = down  */ roll(); // roll left = E
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
    memset(&map[0][1], '#', N);
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
    rot90();  // N = left, for rolling
    roll();   // roll left = N
    #if EXAMPLE
        rot270();  // N = up, for printing
        printmap();
        rot180();  // N = down, for load calculation
    #else
        rot90();  // N = down, for load calculation
    #endif
    printf("Part 1: %d\n", load());  // example: 136, input: 113525

    // Part 2
    // Complete first cycle from part 1: begin with N = down
    rot180(); /* N = up    */ roll(); // roll left = W
    rot270(); /* N = right */ roll(); // roll left = S
    rot270(); /* N = down  */ roll(); // roll left = E

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
    printf("Part 2: %d\n", load());  // example: 64, input: 101292
    printf("Time: %.0f ms\n", stoptimer_ms());
    return 0;
}
