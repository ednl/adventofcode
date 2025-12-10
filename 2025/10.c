/**
 * Advent of Code 2025
 * Day 10: Factory
 * https://adventofcode.com/2025/day/10
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    cc -std=c17 -Wall -Wextra -pedantic 10.c
 * Enable timer:
 *    cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 10.c
 * Get minimum runtime from timer output in bash:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : ? µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : ? µs
 */

#include <stdio.h>
// #include <stdlib.h>  // qsort
#include "../combperm.h"
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define EXAMPLE 1
#if EXAMPLE == 1
    #define FNAME "../aocinput/2025-10-example.txt"
    #define M 3  // number of machines (=lines in example file)
    #define L 6  // max number of lights/joltages per machine
    #define B 6  // max number of buttons per machine
#else
    #define FNAME "../aocinput/2025-10-input.txt"
    #define M 185  // number of machines (=lines in input file)
    #define L  10  // max number of lights/joltages per machine
    #define B  13  // max number of buttons per machine
#endif

// Wrong: must use current button count
// #define P (1 << L)  // power of 2 = combinations pattern count

typedef struct button {
    int btn[B];  // button masks
    int len;     // number of buttons
} Button;

// Machine::active  = buttons with 1+ bits in common with 'lights'
// Machine::passive = buttons with no bits in common with 'lights'
typedef struct machine {
    int lights;  // light mask (light 0 is at bit len-1, 1 at len-2, etc.)
    int size;    // number of lights and joltages
    Button active, passive;
    int jolt[L];
} Machine;

static Machine machine[M];
// static int pat[P];
// static int pop[P];

static void bin(const int x, const int len)
{
    for (int mask = 1 << (len - 1); mask; mask >>= 1)
        putchar(x & mask ? '#' : '.');
}

// static int cmp_pop(const void *p, const void *q)
// {
//     const int i = *(const int *)p;
//     const int j = *(const int *)q;
//     if (pop[i] < pop[j]) return -1;
//     if (pop[i] > pop[j]) return  1;
//     if (i < j) return -1;
//     if (i > j) return  1;
//     return 0;
// }

// static void init(void)
// {
//     pat[0] = pop[0] = 0;
//     for (int i = 1; i < P; ++i) {
//         pat[i] = i;
//         pop[i] = __builtin_popcount(i);
//     }
//     qsort(pat, sizeof pat / sizeof *pat, sizeof *pat, cmp_pop);
// }

// static void show(void)
// {
//     for (int i = 0; i < 3; ++i) {
//         printf("    %2d %2d ", i, pat[i]);
//         bin(pat[i], L);
//         printf(" %d", pop[pat[i]]);
//         putchar('\n');
//     }
//     printf("    (snip)\n");
//     for (int i = P - 3; i < P; ++i) {
//         printf("    %2d %2d ", i, pat[i]);
//         bin(pat[i], L);
//         printf(" %d", pop[pat[i]]);
//         putchar('\n');
//     }
//     putchar('\n');
// }

int main(void)
{
#ifdef TIMER
    starttimer();
#endif

    FILE *f = fopen(FNAME, "r");
    if (!f) { fprintf(stderr, "File not found: %s\n", FNAME); return 1; }
    char buf[BUFSIZ];
    for (int i = 0; i < M && fgets(buf, sizeof buf, f); ++i) {
        const char *c = buf + 1;
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
            if (btn & lights)
                machine[i].active.btn[machine[i].active.len++] = btn;
            else
                machine[i].passive.btn[machine[i].passive.len++] = btn;
            c++;  // skip space
        }
    }
    fclose(f);

    // init();
    // show();

    for (int i = 0; i < M; ++i) {
        printf("Machine %d\n  %2d lights  : [", i, machine[i].size);
        bin(machine[i].lights, machine[i].size);
        printf("]\n  %2d active  :", machine[i].active.len);
        for (int j = 0; j < machine[i].active.len; ++j) {
            printf(" (");
            bin(machine[i].active.btn[j], machine[i].size);
            printf(")");
        }
        printf("\n  %2d passive :", machine[i].passive.len);
        for (int j = 0; j < machine[i].passive.len; ++j) {
            printf(" (");
            bin(machine[i].passive.btn[j], machine[i].size);
            printf(")");
        }
        printf("\n\n");
    }
#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
}
