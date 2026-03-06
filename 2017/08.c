/**
 * Advent of Code 2017
 * Day 8: I Heard You Like Registers
 * https://adventofcode.com/2017/day/8
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 08.c
 * Enable timer:
 *     cc -std=gnu17 -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 08.c
 * Get minimum runtime from timer output:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 107 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :   ? µs
 *     Raspberry Pi 5 (2.4 GHz)      :   ? µs
 */

#include <stdio.h>
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2017-08-input.txt"
#define REGS 512

static int reg[REGS];
static bool used[REGS];
static int set[32];  // needed: 28
static int setlen;

// Hash of 1-3 characters (max = 508)
static unsigned hash(const char *s)
{
    unsigned h = 83 ^ *s++;
    for (unsigned m = 2; *s; m >>= 1) {
        h <<= m;
        h ^= *s++;
    }
    if (!used[h]) {
        used[h] = true;
        set[setlen++] = h;
    }
    return h;
}

int main(void)
{
    FILE *f = fopen(FNAME, "r");
    if (!f) return 1;

#ifdef TIMER
    starttimer();
#endif

    int max2 = 0;
    char r0[4], r1[4], op[4], cmp[3];
    int adj, ref;
    while (fscanf(f, "%3s %3s %d if %3s %2s %d", r0, op, &adj, r1, cmp, &ref) == 6) {
        const int val = reg[hash(r1)];
        bool ok = false;
        switch (cmp[0] ^ cmp[1]) {
            case '=' ^ '=': ok = val == ref; break;
            case '<' ^ '=': ok = val <= ref; break;
            case '>' ^ '=': ok = val >= ref; break;
            case '!' ^ '=': ok = val != ref; break;
            case '<'      : ok = val <  ref; break;
            case '>'      : ok = val >  ref; break;
        }
        if (ok) {
            const int k = hash(r0);
            reg[k] += ((op[0] & 1) * 2 - 1) * adj;
            if (reg[k] > max2)
                max2 = reg[k];
        }
    }
    int max1 = 0;
    for (int i = 0; i < setlen; ++i)
        if (reg[set[i]] > max1)
            max1 = reg[set[i]];
    printf("%d %d\n", max1, max2);  // 3089 5391

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif

    fclose(f);
}
