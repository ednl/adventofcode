/**
 * Advent of Code 2022
 * Day 6: Tuning Trouble, alternative implementation
 * https://adventofcode.com/2022/day/6
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 06.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 06.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  2.44 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :     ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : 10.4  µs
 */

#include <stdio.h>
#include <stdint.h>
#ifdef TIMER
    #include <string.h>  // memset
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2022-06-input.txt"
#define FSIZE 4096
#define BINS (('z' & 31) + 1)  // room for 'z' & 31

static char input[FSIZE];
static uint8_t bin[BINS];  // frequency bins
static int prev, mark;  // global, in order to reset between timing runs

static int find(const int len)
{
    int dup = 0;                                  // duplicates counter
    for (int i = 0; i < len - prev; ++i, ++mark)  // extend window to new length
        dup += ++bin[input[mark] & 31] == 2;      // 1->2 : duplicate added
    prev = len;                                   // remember len for next function call
    for (; dup; ++mark)                           // loop until no more duplicates (for sane input)
        dup += (++bin[input[mark] & 31] == 2) - (--bin[input[mark - len] & 31] == 1);
    return mark;                                  // start-of-message marker
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    fread(input, FSIZE, 1, f);
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
    memset(bin, 0, sizeof bin);
    prev = mark = 0;
#endif

    printf("%d %d\n", find(4), find(14));  // 1542 3153

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
