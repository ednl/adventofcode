/**
 * Advent of Code 2023
 * Day 14: Parabolic Reflector Dish
 * https://adventofcode.com/2023/day/14
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -Ofast -march=native -Wall -Wextra 14alt.c ../startstoptimer.c
 *    gcc   -std=gnu17 -Ofast -march=native -Wall -Wextra 14alt.c ../startstoptimer.c
 * Get minimum runtime:
 *     m=999999;for((i=0;i<5000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * Minimum runtime:
 *     Mac Mini 2020 (M1 3.2 GHz)          : ? µs
 *     iMac 2013 (i5 Haswell 4570 3.2 GHz) : ? µs
 *     Raspberry Pi 5 (2.4 GHz)            : ? µs
 *     Raspberry Pi 4 (1.8 GHz)            : ? µs
 */

#include <stdio.h>    // fopen, fclose, fgets, printf
#include <string.h>   // memset
#include <stdint.h>
#include <stdbool.h>  // bool
// #include "../startstoptimer.h"

#define EXAMPLE 1
#if EXAMPLE
    #define NAME "../aocinput/2023-14-example.txt"
    #define N 10
    #define SEGMENTS 3
    typedef uint16_t bitmask;
#else
    #define NAME "../aocinput/2023-14-input.txt"
    #define N 100
    #define SEGMENTS 20
    typedef __uint128_t bitmask;
#endif

static char map[N][N + 2];
static bitmask space[2][N][SEGMENTS];  // 2 orientations, N rows or cols, max number of open space segments
static bitmask base[N];  // mask with [0..N) bits and zero offset

static void transpose_map(void)
{
    for (int i = 0; i < N; ++i)
        for (int j = i + 1; j < N; ++j) {
            const char tmp = map[i][j];
            map[i][j] = map[j][i];
            map[j][i] = tmp;
        }
}

int main(void)
{
    FILE* f = fopen(NAME, "r");
    if (!f) { fputs("File not found.\n", stderr); return 1; }
    for (int i = 0; i < N; ++i)
        fgets(map[i], sizeof *map, f);
    fclose(f);

    bitmask m = 1u;
    for (int i = 0; i < N; ++i, m = m << 1 | 1)
        base[i] = m;

    // Map open space segments
    for (int i = 0, segm = 0; i < N; ++i) {
        for (int j = 0, k; j < N; j = k + 1) {
            while (j < N && map[i][j] == '#') ++j;  // j is start of open space
            if (j < N) {
                for (k = j + 1; k < N && map[i][j] != '#'; ++k);  // k is 1 past end of open space
                space[0][i][segm++] = base[k - j] << (N - j - k);  // k-j = length
            }
        }
    }

    return 0;
}
