/**
 * Advent of Code 2025
 * Day 10: Factory
 * https://adventofcode.com/2025/day/10
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    cc -std=c17 -Wall -Wextra -pedantic ../combperm.c 10.c
 * Enable timer:
 *    cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c ../combperm.c 10.c
 * Get minimum runtime from timer output in bash:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 82 µs (part 1)
 *     Mac Mini 2020 (M1 3.2 GHz)    :  ? µs
 *     Raspberry Pi 5 (2.4 GHz)      :  ? µs
 */

#include <stdio.h>
#include "../combperm.h"
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define EXAMPLE 1
#if EXAMPLE == 1
    #define FNAME "../aocinput/2025-10-example.txt"
    #define FSIZE 175
    #define M 3  // number of machines (=lines in example file)
    #define L 6  // max number of lights/joltages per machine
    #define B 6  // max number of buttons per machine
#else
    #define FNAME "../aocinput/2025-10-input.txt"
    #define FSIZE 19254
    #define M 185  // number of machines (=lines in input file)
    #define L  10  // max number of lights/joltages per machine
    #define B  13  // max number of buttons per machine
#endif

typedef struct machine {
    int lights;  // light mask (light 0 is at bit len-1, 1 at len-2, etc.)
    int size;    // number of lights and joltages 4..L
    int btns;    // number of buttons 1..B
    int button[B];
    int jolts[L];
} Machine;

static char input[FSIZE];
static Machine machine[M];

#if EXAMPLE || DEBUG
static void bin(const int x, const int len)
{
    for (int mask = 1 << (len - 1); mask; mask >>= 1)
        putchar(x & mask ? '#' : '.');
}
#endif

static int clicks(const int *const button, const int n, const int lights)
{
    const int *a = combinations(0, 0);  // reset
    for (int k = 1; k < n; ++k)
        while ((a = combinations(n, k))) {
            int xor = lights;
            for (int i = 0; i < k; ++i)
                xor ^= button[a[i]];
            if (!xor)
                return k;
        }
    return n;
}

int main(void)
{
    // Read input file from disk
    FILE *f = fopen(FNAME, "rb");  // fread requires binary mode
    if (!f) { fprintf(stderr, "File not found: %s\n", FNAME); return 1; }
    fread(input, sizeof input, 1, f);  // read whole file at once
    fclose(f);

#if DEBUG
    fwrite(input, sizeof input, 1, stdout);
#endif

#ifdef TIMER
    starttimer();
#endif

    const char *c = input;
    for (int i = 0; i < M; ++i) {
        c++;  // skip '['
        int lights = 0, size = 0;
        for (; *c != ']'; ++c, ++size)
            lights = lights << 1 | (*c == '#');  // last light ends up in bit 0
        machine[i].lights = lights;
        machine[i].size = size;
        c += 2;  // skip ']' and space
        while (*c++ == '(') {  // skip '(' or '{'
            int btn = 0;
            do {  // store in reverse: wire '0' ends up in bit size-1
                btn |= 1 << (size - 1 - (*c++ & 15));  // skip digit
            } while (*c++ == ',');  // skip ',' or ')'
            machine[i].button[machine[i].btns++] = btn;
            c++;  // skip space
        }
        for (int j = 0; j < size; ++j) {
            int x = *c++ & 15;  // skip digit
            while (*c >= '0' && *c <= '9')
                x = x * 10 + (*c++ & 15);  // skip digit
            machine[i].jolts[j] = x;
            c++;  // skip ',' or '}'
        }
        c++;  // skip '\n'
    }

#if EXAMPLE || DEBUG
    for (int i = 0; i < M; ++i) {
        printf("Machine %d\n%4d lights  : [", i, machine[i].size);
        bin(machine[i].lights, machine[i].size);
        printf("]\n%4d buttons :", machine[i].btns);
        for (int j = 0; j < machine[i].btns; ++j) {
            printf(" (");
            bin(machine[i].button[j], machine[i].size);
            printf(")");
        }
        printf("\n%4d jolts   : {%d", machine[i].size, machine[i].jolts[0]);
        for (int j = 1; j < machine[i].size; ++j)
            printf(",%d", machine[i].jolts[j]);
        printf("}\n\n");
    }
#endif

    int total = 0;
    for (int i = 0; i < M; ++i)
        total += clicks(machine[i].button, machine[i].btns, machine[i].lights);
    printf("Part 1: %d\n", total);  // example: 7, input: 512
    combinations(0, 0);  // free memory

    // (3) (1,3) (2) (2,3) (0,2) (0,1) {3,5,4,7}
    // 0 0 0 0 1 1  |  3
    // 0 1 0 0 0 1  |  5
    // 0 0 1 1 1 0  |  4
    // 1 1 0 1 0 0  |  7
    // -------------+---
    // 1 3 0 3 1 2  | 10

    // (0,2,3,4) (2,3) (0,4) (0,1,2) (1,2,3,4) {7,5,12,7,2}
    // 1 0 1 1 0  |  7
    // 0 0 0 1 1  |  5
    // 1 1 0 1 1  | 12
    // 1 1 0 0 1  |  7
    // 1 0 1 0 1  |  2
    // -----------+---
    // 2 5 0 5 0  | 12

    // (0,1,2,3,4) (0,3,4) (0,1,2,4,5) (1,2) {10,11,11,5,10,5}
    // 1 1 1 0  | 10
    // 1 0 1 1  | 11
    // 1 0 1 1  | 11
    // 1 1 0 0  |  5
    // 1 1 1 0  | 10
    // 0 0 1 0  |  5
    // ---------+---
    // 5 0 5 1  | 11

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
}
