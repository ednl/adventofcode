/**
 * Advent of Code 2022
 * Day 20: Grove Positioning System
 * https://adventofcode.com/2022/day/20
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -O3 -march=native -Wall -Wextra 20.c ../startstoptimer.c
 *    gcc   -std=gnu17 -O3 -march=native -Wall -Wextra 20.c ../startstoptimer.c
 * Get minimum runtime:
 *     m=999999;for((i=0;i<200;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * Minimum runtime:
 *     Mac Mini 2020 (M1 3.2 GHz)          :  85 ms
 *     iMac 2013 (i5 Haswell 4570 3.2 GHz) :   ? ms
 *     Raspberry Pi 5 (2.4 GHz)            :   ? ms
 *     Raspberry Pi 4 (1.8 GHz)            :   ? ms
 */

#include <stdio.h>
#include <stdlib.h>    // atoi
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRId64
#include "../startstoptimer.h"

#define EXAMPLE 0
#if EXAMPLE == 1
    #define NAME "../aocinput/2022-20-example.txt"
    #define N 7
#else
    #define NAME "../aocinput/2022-20-input.txt"
    #define N 5000
#endif
#define M (N - 1)
#define OFS 1000
#define OFS_A (OFS % N)
#define OFS_B (OFS_A - N)
#define REDUCED_OFS (OFS_A <= -OFS_B ? OFS_A : OFS_B)

// Part 2
#define REPEAT 10
#define KEY 811589153
#define REDUCED_KEY (KEY % M)

static int value[N], shift[N], prev[N], next[N];

#ifdef SHOW
static void show(void)
{
    printf("%2d", value[0]);
    for (int i = next[0]; i; i = next[i])
        printf(",%2d", value[i]);
    putchar('\n');
}
#endif

static int closest(const int x)
{
    const int a = x % M;
    if (a >= 0) {
        const int b = a - M;
        return a <= -b ? a : b;
    }
    const int b = a + M;
    return b <= -a ? b : a;
}

static int parse(void)
{
    FILE *f = fopen(NAME, "r");
    if (!f)
        return -1;  // file not found
    char buf[8];
    int zeroindex = -1;
    for (int i = 0; i < N && fgets(buf, sizeof buf, f); ++i) {
        value[i] = atoi(buf);
        shift[i] = closest(value[i]);
        prev[i] = i - 1;
        next[i] = i + 1;
        if (!value[i])
            zeroindex = i;
    }
    fclose(f);
    if (zeroindex < 0)
        return -2;  // wrong file
    prev[0] = M;
    next[M] = 0;
#ifdef SHOW
    show();
#endif
    return zeroindex;  // zeroindex>=0: success
}

static void move(const int index, int steps)
{
    int p = prev[index], n = next[index];
    // Remove
    next[p] = n;
    prev[n] = p;
    // Find
    if (steps >= 0)
        while (steps--) p = next[p];
    else
        while (steps++) p = prev[p];
    n = next[p];
    // Insert
    next[p] = index;
    prev[n] = index;
    prev[index] = p;
    next[index] = n;
}

static int decrypt(const int startindex)
{
    int sum = 0;
    for (int i = 0, j = startindex; i < 3; ++i) {
        #if REDUCED_OFS > 0
            for (int k = REDUCED_OFS; k; --k)
                j = next[j];
        #elif REDUCED_OFS < 0
            for (int k = REDUCED_OFS; k; ++k)
                j = prev[j];
        #endif
        sum += value[j];
    }
    return sum;
}

int main(void)
{
    starttimer();
    const int zeroindex = parse();
    if (zeroindex < 0)
        return zeroindex;

    // Part 1
    for (int i = 0; i < N; ++i) {
        move(i, shift[i]);
    #ifdef SHOW
        show();
    #endif
    }
    printf("Part 1: %d\n", decrypt(zeroindex));  // example=3, input=4066

    // Reset
    for (int i = 0; i < N; ++i) {
        shift[i] = closest(shift[i] * REDUCED_KEY);  // new shift value for part 2
        prev[i] = i - 1;
        next[i] = i + 1;
    }
    prev[0] = M;
    next[M] = 0;

    // Part 2
    for (int k = 0; k < REPEAT; ++k) {
        for (int i = 0; i < N; ++i)
            move(i, shift[i]);
    #ifdef SHOW
        show();
    #endif
    }
    printf("Part 2: %"PRId64"\n", (int64_t)KEY * decrypt(zeroindex));  // example=1623178306, input=6704537992933

#if EXAMPLE
    printf("Time: %.0f us\n", stoptimer_us());
#else
    printf("Time: %.0f ms\n", stoptimer_ms());
#endif
    return 0;
}
