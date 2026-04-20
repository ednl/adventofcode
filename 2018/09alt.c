/**
 * Advent of Code 2018
 * Day 9: Marble Mania
 * https://adventofcode.com/2018/day/9
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 09alt.c
 * Enable timer:
 *     cc -std=gnu17 -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 09alt.c
 * Get minimum runtime from timer output:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  3.12 ms
 *     Mac Mini 2020 (M1 3.2 GHz)    :  4.89 ms
 *     Raspberry Pi 5 (2.4 GHz)      :  ?    ms
*/

#include <stdio.h>
#include <stdlib.h>  // calloc, free
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2018-09-input.txt"
#define KEEP 23U  // puzzle rule

typedef unsigned int u32;

int main(void)
{
    FILE *f = fopen(FNAME, "r");
    if (!f)
        return 1;
    u32 inp1 = 0, inp2 = 0;
    if (fscanf(f, "%u players; last marble is worth %u", &inp1, &inp2) != 2)
        return 2;
    fclose(f);

    const u32 players = inp1;
    const u32 marbles1 = inp2;
    const u32 marbles2 = marbles1 * 100U;  // part 2
    const u32 len = marbles2 * 37U / 23U;  // needed for my input: 11586785

    u32 *score = calloc(players, sizeof *score);
    u32 *line = calloc(len, sizeof *line);

#ifdef TIMER
    starttimer();
#endif

    u32 i = 0, j = 1, n = 1, m = KEEP + 1, k = KEEP, hi = 0;

    for (u32 t = 0; t != 6U; ++t) {
        line[j++] = line[i++];      // skip one
        line[j++] = n++;            // 1-6
    }

    for (; k <= marbles1; k += KEEP) {
        for (u32 t = 0; t != 12U; ++t) {
            line[j++] = line[i++];  // skip one
            line[j++] = n++;        // 7-18
        }

        // Keep multiple of 23 + take marble at index i
        score[k % players] += k + line[i];

        line[i] = n++;              // 19
        line[j++] = line[i++];      // skip one

        for (u32 t = 0; t != 3U; ++t) {
            line[j++] = line[i++];  // skip one
            line[j++] = m++;        // 24, 26, 28
            line[j++] = n++;        // 20, 21, 22
            line[j++] = m++;        // 25, 27, 29
        }

        n += KEEP - 16U;  // 16 already counted, need 7 more to make 23
        m += KEEP - 6U;   // 6 already counted, need 17 more to make 23
    }

    for (u32 t = 0; t != players; ++t)
        if (score[t] > hi)
            hi = score[t];
    printf("Part 1: %u\n", hi);  // 390093

    for (; k <= marbles2; k += KEEP) {
        for (u32 t = 0; t < 12U; ++t) {
            line[j++] = line[i++];  // skip one
            line[j++] = n++;        // 7-18
        }

        // Keep multiple of 23 + take marble at index i
        score[k % players] += k + line[i];

        line[i] = n++;              // 19
        line[j++] = line[i++];      // skip one

        for (u32 t = 0; t < 3U; ++t) {
            line[j++] = line[i++];  // skip one
            line[j++] = m++;        // 24, 26, 28
            line[j++] = n++;        // 20, 21, 22
            line[j++] = m++;        // 25, 27, 29
        }

        n += KEEP - 16U;  // 16 already counted, need 7 more to make 23
        m += KEEP - 6U;   // 6 already counted, need 17 more to make 23
    }

    for (u32 t = 0; t < players; ++t)
        if (score[t] > hi)
            hi = score[t];
    printf("Part 2: %u\n", hi);  // 3150377341

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
    free(line);
    free(score);
}
