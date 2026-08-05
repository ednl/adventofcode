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
 *     Macbook Pro 2024 (M4 4.4 GHz) : 1.51 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :    ? µs
 *     Raspberry Pi 5 (2.4 GHz)      :    ? µs
 */

#include <stdio.h>
#ifdef TIMER
    #include <string.h>  // memset
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2022-06-input.txt"
#define FSIZE 4096
#define LEN1  4
#define LEN2 14

static char input[FSIZE];
static int seen['z' + 1];

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    fread(input, FSIZE, 1, f);
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
    memset(&seen['a'], 0, sizeof *seen * ('z' - 'a' + 1));
#endif

    int i = 0, j = 0;
    while (j - i < LEN1) {
        int *const k = &seen[(int)input[j]];
        if (*k >= i)
            i = *k + 1;
        *k = j++;
    }
    printf("%d ", j);  // 1542
    while (j - i < LEN2) {
        int *const k = &seen[(int)input[j]];
        if (*k >= i)
            i = *k + 1;
        *k = j++;
    }
    printf("%d\n", j);  // 3153

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
