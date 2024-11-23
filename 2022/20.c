/**
 * Advent of Code 2022
 * Day 20: Grove Positioning System
 * https://adventofcode.com/2022/day/20
 * By: E. Dronkert https://github.com/ednl
 *
 * Benchmark with the internal timer on a Mac Mini M1 using this Bash oneliner:
 *   m=50000;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * gives a shortest runtime for my input file (not the example) of [TBD] µs.
 * On a Raspberry Pi 4 with the CPU in performance mode: [TBD] µs.
 *   echo performance | sudo tee /sys/devices/system/cpu/cpufreq/policy0/scaling_governor
 *   /boot/config.txt: arm_boost=1, no overclock
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
#define KEY INT64_C(811589153)
#define REDUCED_KEY (KEY % M)
#define REPEAT 10

static int64_t value[N];
static int shift[N], prev[N], next[N];

#if EXAMPLE
static void show(void)
{
    printf("%2"PRId64, value[0]);
    for (int i = next[0]; i; i = next[i])
        printf(",%2"PRId64, value[i]);
    putchar('\n');
}
#endif

static int closest(const int64_t x)
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
#if EXAMPLE
    show();
#endif
    return zeroindex;  // zeroindex>=0: success
}

static void move(const int index, int steps)
{
    int left, right;
    if (steps > 0) {
        left = next[index];
        while (--steps)
            left = next[left];
        right = next[left];
    } else if (steps < 0) {
        right = prev[index];
        while (++steps)
            right = prev[right];
        left = prev[right];
    } else
        return;
    // Remove
    next[prev[index]] = next[index];
    prev[next[index]] = prev[index];
    // Insert
    next[left ] = index;
    prev[right] = index;
    // Anchor
    prev[index] = left;
    next[index] = right;
#if EXAMPLE
    show();
#endif
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

    for (int i = 0; i < N; ++i)
        move(i, shift[i]);
    printf("Part 1: %d\n", decrypt(zeroindex));  // example=4-3+2=3, input=4066
    // printf("Time: %.1f ms\n", stoptimer_ms());

    // Reset
    // starttimer();
    for (int i = 0; i < N; ++i) {
        shift[i] = closest(shift[i] * REDUCED_KEY);
        prev[i] = i - 1;
        next[i] = i + 1;
    }
    prev[0] = M;
    next[M] = 0;

    for (int k = 0; k < REPEAT; ++k)
        for (int i = 0; i < N; ++i)
            move(i, shift[i]);
    printf("Part 2: %"PRId64"\n", decrypt(zeroindex) * KEY);  // example=1623178306, input=6704537992933
    printf("Time: %.0f ms\n", stoptimer_ms());
    return 0;
}
