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
 *     m=50000;for((i=0;i<100;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * Minimum runtime:
 *     Apple M1 Mac Mini      : 29 ms
 *     Raspberry Pi 5 2.4 GHz : 61 ms
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

static int min(const int a, const int b)
{
    return a < b ? a : b;
}

static int max(const int a, const int b)
{
    return a > b ? a : b;
}

// Roll round rocks all the way to the left
static void roll(void)
{
    for (int i = 1; i < LIM; ++i)  // row
        for (int head = 1; head < LIM; ++head) {  // col
            while (head < LIM && map[i][head] != 'O')  // look for first round rock
                ++head;
            if (head < LIM) {
                int dest = head;
                while (map[i][--dest] == '.');  // look for open space to the left
                const int shift = head - ++dest;  // how far to move left
                if (shift) {
                    int tail = head;
                    while (map[i][++tail] == 'O');  // look for end of round rock sequence
                    const int len = min(shift, tail - head);  // how many rocks to add+remove
                    const int rem = max(head, tail - shift);  // start of rocks to be removed
                    memset(&map[i][dest], 'O', (size_t)len);  // add round rocks
                    memset(&map[i][rem] , '.', (size_t)len);  // remove round rocks
                }
            }
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
