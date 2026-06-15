/**
 * Advent of Code 2020
 * Day 15: Rambunctious Recitation
 * https://adventofcode.com/2020/day/15
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 15.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 15.c
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  90 ms
 *     Mac Mini 2020 (M1 3.2 GHz)    :   ? ms
 *     Raspberry Pi 5 (2.4 GHz)      :   ? ms
 *
 * Speedup with bitfield 'seen' from
 * https://github.com/maneatingape/advent-of-code-rust/blob/main/src/year2020/day15.rs
 *
 * Number of times each turn is seen with my input. So, LARGE=0x20000 is a good cut-off.
 *
 *      turn   seen=0  seen=1   seen>1  min      max
 *   -------  -------  ------   ------  ---  -------
 *     20000        0       0   131072   2   3611723
 *     40000      133     834   130105   1        26
 *     60000     1583    6924   122565   1        19
 *     80000     5651   17796   107625   1        14
 *     a0000    11644   27723    91705   1        12
 *     c0000    18124   35963    76985   1        10
 *     e0000    24830   41298    64944   1        10
 *    100000    31165   44982    54925   1         9
 *   [...]
 *   1b40000   131022      49        1   1         2
 *   1b60000   131042      30        0   1         1
 *   1b80000   131035      37        0   1         1
 *   1ba0000   131051      21        0   1         1
 *   1bc0000   131064       8        0   1         1
 *   1be0000   131067       5        0   1         1
 *   1c00000   131066       6        0   1         1
 *   1c20000   131071       1        0   1         1
 *   1c40000   131072       0        0   0         0
 *   1c60000   131072       0        0   0         0
 *   1c80000   131072       0        0   0         0
 *   1c9c380   115584       0        0   0         0
 */

#include <stdio.h>
#include <stdint.h>  // uint64_t
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2020-15-input.txt"
#define FSIZE 32  // needed for my input: 16

#define TURN1 2020U
#define TURN2 (30U * 1000 * 1000)
#define SEEN  (TURN2 >> 6)  // divide by 64
#define LARGE 0x20000U  // numbers above: mostly (but not all) seen = 0x or 1x

static char input[FSIZE];
static unsigned spoken[TURN2];
static uint64_t seen[SEEN];
static unsigned zero;

static unsigned play(const unsigned start, const unsigned end, unsigned last)
{
    for (unsigned i = start; i < end; ++i)
        if (last == 0) {
            // Never seen is by far the most common
            unsigned prev = zero;
            zero = i;
            last = prev ? i - prev : 0;
        } else if (last < LARGE) {
            // Most that ARE seen, are seen many times
            unsigned prev = spoken[last];
            spoken[last] = i;
            last = prev ? i - prev : 0;
        } else {
            unsigned word = last >> 6;
            uint64_t bit = UINT64_C(1) << (last & 63);
            if ((seen[word] & bit) == 0) {
                // Most are seen never or once
                seen[word] |= bit;
                spoken[last] = i;
                last = 0;
            } else {
                // Some are seen multiple times
                unsigned prev = spoken[last];
                spoken[last] = i;
                last = i - prev;
            }
        }
    printf("%u\n", last);
    return last;
}

int main(void)
{
    // Read input
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    const char *const end = input + fread(input, 1, FSIZE, f);
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    // Parse and init
    unsigned size = 0, last;
    for (const char *c = input; c != end; ++c) {
        last = *c++ & 15;
        if (*c >= '0')
            last = last * 10 + (*c++ & 15);
        if (last)
            spoken[last] = ++size;
        else
            zero = ++size;
    }

    // Parts 1 and 2
    play(TURN1, TURN2, play(size, TURN1, last));  // 319 2424

#ifdef TIMER
    printf("Time: %.0f ms\n", stoptimer_ms());
#endif
}
