/**
 * Advent of Code 2020
 * Day 5: Binary Boarding
 * https://adventofcode.com/2020/day/5
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 05.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 05.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 1.55 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :    ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : 4.33 µs
 */

#include <stdio.h>
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2020-05-input.txt"
#define PASS 761  // boarding passes (lines in input file)
#define LEN  10   // line length = 10x F/B/L/R (excl. newline)
#define RANGE (1 << LEN)  // all possible boarding pass IDs
#define FSIZE (PASS * (LEN + 1))  // +newline

static char input[FSIZE];
static bool pass[RANGE];  // boarding pass ID in input file?

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    fread(input, FSIZE, 1, f);  // read one block of fixed size
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    // Parse input file, check off boarding pass IDs
    const char *c = input;
    for (int i = 0; i < PASS; ++i) {
        int id = 0;
        for (int j = 0; j < LEN; ++j)
            id = id << 1 | !(*c++ & 4);  // F/L = 0, B/R = 1
        pass[id] = true;
        c++;  // skip newline
    }

    int id = RANGE - 2;  // "Your seat wasn't at the very front or back, though"
    while (!pass[id])  // skip non-existing seats at the back
        id--;
    printf("%d", id--);  // 861
    while (pass[id])
        id--;
    printf(" %d\n", id);  // 633

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
