/**
 * Advent of Code 2017
 * Day 24: Electromagnetic Moat
 * https://adventofcode.com/2017/day/24
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 24.c
 * Enable timer:
 *     cc -std=gnu17 -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 24.c
 * Get minimum runtime from timer output:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  46 ms
 *     Mac Mini 2020 (M1 3.2 GHz)    :  68 ms
 *     Raspberry Pi 5 (2.4 GHz)      : 118 ms
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define DEBUG
#define FNAME "../aocinput/2017-24-input.txt"
#define N 57  // number of lines in input file

// Components with two ports, sum of ports, used flag
typedef struct comp {
    int port1, port2, strength;
    bool used;
} Comp;

#ifdef DEBUG
static int exist[51][51];
#endif
static Comp comp[N] = {0};
static int maxsum = 0;
static int maxlen = 0;
static int maxsum2 = 0;

static int load(void)
{
    FILE *f = fopen(FNAME, "r");
    if (!f) {
        fprintf(stderr, "File not found: "FNAME"\n");
        return 0;  // zero lines read
    }
    int n = 0;
    for (int a, b; n < N && fscanf(f, "%d/%d ", &a, &b) == 2; ++n) {
        comp[n] = (Comp){a, b, a + b, false};
    #ifdef DEBUG
        exist[a][b]++;
        if (a != b)
            exist[b][a]++;
    #endif
    }
    fclose(f);
#ifdef DEBUG
    printf("      ");
    for (int j = 0; j < 51; ++j)
        printf(" %d", j % 10);
    putchar('\n');
    for (int i = 0; i < 51; ++i) {
        printf("    %2d ", i);
        for (int j = 0; j < 51; ++j)
            printf("%c ", exist[i][j] ? (i == j ? 'X' : 'O') : '.');
        printf("%-4d", i);
        for (int j = 0; j < 51; ++j)
            if (exist[i][j])
                printf("%3d", j);
        printf("\n");
    }
    printf("      ");
    for (int j = 0; j < 51; ++j)
        printf(" %d", j % 10);
    putchar('\n');
#endif
    return n;
}

static void bridge(int port, int len, int sum)
{
    // Part 1
    if (sum > maxsum)
        maxsum = sum;

    // Part 2
    if (len > maxlen) {
        maxlen = len;
        maxsum2 = sum;
    } else if (len == maxlen && sum > maxsum2)
        maxsum2 = sum;

    for (int i = 0; i < N; ++i)
        if (!comp[i].used && (port == comp[i].port1 || port == comp[i].port2)) {
            comp[i].used = true;
            bridge(
                port == comp[i].port1 ? comp[i].port2 : comp[i].port1,
                len + 1,
                sum + comp[i].strength);
            comp[i].used = false;
        }
}

int main(void)
{
    if (!load())
        return 1;

#ifdef TIMER
    starttimer();
#endif

    bridge(0, 0, 0);
    printf("%d %d\n", maxsum, maxsum2);   // 1868 1841

#ifdef TIMER
    printf("Time: %.0f ms\n", stoptimer_ms());
#endif
}
