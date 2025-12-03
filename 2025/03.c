/**
 * Advent of Code 2025
 * Day 3: Lobby
 * https://adventofcode.com/2025/day/3
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    cc -std=c17 -Wall -Wextra -pedantic 03.c
 * Enable timer:
 *    cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 03.c
 * Get minimum runtime from timer output in bash:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  49 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :   ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : 130 µs
 */

#include <stdio.h>
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRId64
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define EXAMPLE 0
#if EXAMPLE
    #define FNAME "../aocinput/2025-03-example.txt"
    #define BANKS    4
    #define BANKLEN 15
#else
    #define FNAME "../aocinput/2025-03-input.txt"
    #define BANKS   200
    #define BANKLEN 100
#endif
#define LINELEN (BANKLEN + 1)

static char battery[BANKS][LINELEN];

static int64_t joltage(const int batteries) {
    const int firstend = LINELEN - batteries;
    int64_t total = 0;
    for (int bank = 0; bank < BANKS; ++bank) {
        int64_t series = 0;
        for (int i = 0, key = -1; i < batteries; ++i) {
            const int end = firstend + i;
            char maxjolt = 0;
            for (int k = key + 1; maxjolt < '9' && k < end; ++k)
                if (battery[bank][k] > maxjolt)
                    maxjolt = battery[bank][(key = k)];
            series = series * 10 + (maxjolt & 15);
        }
        total += series;
    }
    return total;
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");  // fread requires binary mode
    if (!f) { fprintf(stderr, "File not found: %s\n", FNAME); return 1; }
    fread(battery, sizeof battery, 1, f);  // read whole file at once
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    // example: 357 3121910778619, input: 17324 171846613143331
    printf("%"PRId64" %"PRId64"\n", joltage(2), joltage(12));

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
    return 0;
}
