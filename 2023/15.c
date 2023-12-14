/**
 * Advent of Code 2023
 * Day 15: ???
 * https://adventofcode.com/2023/day/15
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -Ofast -march-native -Wall 15.c ../startstoptimer.c
 *    gcc   -std=gnu17 -Ofast -march-native -Wall 15.c ../startstoptimer.c
 * Get minimum runtime:
 *     m=999999;for((i=0;i<500;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * Minimum runtime:
 *     Apple M1 Mac Mini      : ?? ms
 *     Raspberry Pi 5 2.4 GHz : ?? ms
 */

#include <stdio.h>    // fopen, fclose, fgets, printf
#include <string.h>   // memset
#include <stdbool.h>  // bool
#include "../startstoptimer.h"

#define EXAMPLE 1
#if EXAMPLE
#define NAME "../aocinput/2023-15-example.txt"
#define N 10
#else
#define NAME "../aocinput/2023-15-input.txt"
#define N 100
#endif

static char map[N][N + 2];

int main(void)
{
    starttimer();
    FILE* f = fopen(NAME, "r");
    if (!f)
        return 1;
    for (int i = 0; i < N; ++i)
        fgets(map[i], N + 1, f);
    fclose(f);

    //TODO

    printf("Time: %.0f ms\n", stoptimer_ms());
    return 0;
}
