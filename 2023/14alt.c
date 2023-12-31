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

#include <stdio.h>    // fopen, fclose, fgets, printf, fputs, fputc
#include <string.h>   // memset
#include <stdint.h>   // uint16_t
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
    #define N 100  // number of rows and cols of the map
    #define SEGMENTS 20  // max no. of disjoint open space segments in any row or col
    typedef __uint128_t bitmask;  // number of bits >= N
#endif

static char map[N][N + 2];  // +2 for '\n\0'
static bitmask ones[N + 1];  // ones[i] = 0,1,3,7,15,31,etc or popcount(ones[i]) = i
static bitmask space[2][N][SEGMENTS];  // 2 orientations, N rows or cols, max number of open space segments
static bitmask balls[N];  // rolling rocks 'O'

static void transpose_map(void)
{
    for (int i = 0; i < N; ++i)
        for (int j = i + 1; j < N; ++j) {
            const char tmp = map[i][j];
            map[i][j] = map[j][i];
            map[j][i] = tmp;
        }
}

static void init(void)
{
    FILE* f = fopen(NAME, "r");
    if (!f) { fputs("File not found.\n", stderr); return; }
    for (int i = 0; i < N; ++i)
        fgets(map[i], sizeof *map, f);
    fclose(f);

    // Find rolling rocks
    for (int i = 0; i < N; ++i)
        for (int j = 0, k = N - 1; j < N; ++j, --k)
            balls[i] |= (bitmask)(map[i][j] == 'O') << k;

    // Base masks with increasing count of bits set
    bitmask m = 1;
    for (int i = 1; i <= N; ++i, m = (bitmask)(m << 1 | 1))
        ones[i] = m;

    // Find open space segments in rows and cols
    for (int t = 0; t < 2; ++t) {
        for (int i = 0; i < N; ++i)
            for (int j = 0, segm = 0, k; j < N; j = k + 1) {
                while (j < N && map[i][j] == '#') ++j;  // j is start of open space
                for (k = j + 1; k < N && map[i][k] != '#'; ++k);  // k is 1 past end of open space
                if (j < N)
                    // k-j = segment length
                    // N-j-1-(k-j-1) = N-k = offset from right
                    space[t][i][segm++] = (bitmask)(ones[k - j] << (N - k));
            }
        transpose_map();
    }
}

int main(void)
{
    init();

    // Show space and square rocks in row x col and col x row
    for (int t = 0; t < 2; ++t) {
        for (int i = 0; i < N; ++i) {
            printf("%3d: ", i + 1);
            bitmask m = 0;
            for (int j = 0; j < SEGMENTS; ++j)
                m |= space[t][i][j];
            for (bitmask b = (bitmask)1 << (N - 1); b; b >>= 1)
                fputc(m & b ? '.' : '#', stdout);
            fputc('\n', stdout);
        }
        fputc('\n', stdout);
    }

    // Show round rocks
    for (int i = 0; i < N; ++i) {
        printf("%3d: ", i + 1);
        for (bitmask bit = (bitmask)1 << (N - 1); bit; bit >>= 1)
            fputc(balls[i] & bit ? 'O' : '.', stdout);
        fputc('\n', stdout);
    }
    fputc('\n', stdout);

    return 0;
}
