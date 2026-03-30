/**
 * Advent of Code 2015
 * Day 20: Infinite Elves and Infinite Houses
 * https://adventofcode.com/2015/day/20
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 20.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 20.c
 * Get minimum runtime from timer output in bash:
 *     m=9999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  8.54 ms
 *     Mac Mini 2020 (M1 3.2 GHz)    : 12.7  ms
 *     Raspberry Pi 5 (2.4 GHz)      : ?     ms
 */

#include <stdio.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define N  (1000 * 1000)  // number of houses (arbitrary)
#define M1 10  // elf multiplier (part 1)
#define M2 11  // elf multiplier (part 2)
#define Q  50  // max house visits (part 2)

static int presents[N];

int main(void)
{
    FILE *f = fopen("../aocinput/2015-20-input.txt", "r");
    if (!f) return 1;
    int P;  // puzzle input, presents threshold
    if (fscanf(f, "%d", &P) != 1) return 2;
    if (P < 1) return 3;  // also catches 32-bit signed overflow
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    // Part 1: distribute presents
    for (int house = 1; house < N; ++house)
        presents[house] = M1;
    for (int elf = 2; elf < N; ++elf) {
        const int npres = elf * M1;
        for (int house = elf; house < N; house += elf)
            presents[house] += npres;
    }

    // Part 1: find lowest house number where number of presents >= puzzle input
    for (int house = 1; house < N; ++house)
        if (presents[house] >= P) {
        #ifdef TIMER
            printf("%d\n", house);  // 776160
        #else
            printf("Part 1: %d (%d >= %d)\n", house, presents[house], P);  // 776160 (33611760 >= P)
        #endif
            break;
        }

    // Part 2: distribute presents
    for (int house = 1; house < N; ++house)
        presents[house] = M2;
    for (int elf = 2; elf < N; ++elf) {
        const int npres = elf * M2;
        for (int visit = 0, house = elf; visit < Q && house < N; ++visit, house += elf)
            presents[house] += npres;
    }

    // Part 2: find lowest house number where number of presents >= puzzle input
    for (int house = 1; house < N; ++house)
        if (presents[house] >= P) {
        #ifdef TIMER
            printf("%d\n", house);  // 786240
        #else
            printf("Part 2: %d (%d >= %d)\n", house, presents[house], P);  // 786240 (33161601 >= P)
        #endif
            break;
        }

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
}
