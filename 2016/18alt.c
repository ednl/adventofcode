/**
 * Advent of Code 2016
 * Day 18: Like a Rogue
 * https://adventofcode.com/2016/day/18
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    cc -std=c17 -Wall -Wextra -pedantic 18.c
 * Enable timer:
 *    cc -std=gnu17 -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 18.c
 * Get minimum runtime from timer output:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 255 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 462 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 836 µs
 */

#include <stdio.h>   // fopen, fclose, fread, fwrite
#include <stdint.h>  // uint64_t
#include <unistd.h>  // isatty, fileno
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define ROWS1 40
#define ROWS2 400000

typedef struct u128 {
    uint64_t even, odd;
} u128;

static char input[128];
static u128 traps;
static uint64_t mask;

static int count(void)
{
    return __builtin_popcountll(traps.even ^ mask) + __builtin_popcountll(traps.odd ^ mask);
}

static u128 next(void)
{
    return (u128){traps.odd ^ (traps.odd >> 1), (traps.even ^ (traps.even << 1)) & mask};
}

// Fast manual conversion non-negative int->ascii, +newline
static void print_int(int x)
{
    char buf[sizeof x * 4];
    size_t i = sizeof buf;
    buf[--i] = '\n';
    do {
        buf[--i] = '0' | x % 10;  // assumes ASCII
        x /= 10;
    } while (x);
    fwrite(buf + i, sizeof buf - i, 1, stdout);
}

int main(void)
{
    if (isatty(fileno(stdin))) {
        FILE *f = fopen("../aocinput/2016-18-input.txt", "r");
        if (!f)
            return 1;
        fread(input, sizeof input, 1, f);
        fclose(f);
    } else
        fread(input, sizeof input, 1, stdin);

#ifdef TIMER
    starttimer();
#endif

    // '.' = 0x2e, '^' = 0x5e, '\n' = 0x0a
    for (const char *a = input, *b = input + 1; *a & 0x60; a += 2, b += 2) {
        traps.even = (traps.even << 1) | (*a == '^');
        traps.odd  = (traps.odd  << 1) | (*b == '^');
        mask = (mask << 1) | 1;
    }

    int safe = count();  // row 0
    for (int i = 0; i < ROWS1 - 1; ++i) {
        traps = next();
        safe += count();
    }
    print_int(safe);  // 1956

    for (int i = 0; i < ROWS2 - ROWS1; ++i) {
        traps = next();
        safe += count();
    }
    print_int(safe);  // 19995121

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
}
