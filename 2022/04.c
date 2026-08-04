/**
 * Advent of Code 2022
 * Day 4: Camp Cleanup
 * https://adventofcode.com/2022/day/4
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 04.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 04.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 2.74 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :    ? µs
 *     Raspberry Pi 5 (2.4 GHz)      :    ? µs
 */

#include <stdio.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2022-04-input.txt"
#define FSIZE 12000  // needed for my input: 11407

static char input[FSIZE];

// One- or two-digit numbers, only interested in relative values
static int parseint(const char **s)
{
    int x;
    if (*(*s + 1) >= '0') {  // two digits? (most prevalent)
        x = **s << 8 | *(*s + 1);  // manual ordering
        *s += 3;
    } else {
        x = **s;
        *s += 2;
    }
    return x;
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    fread(input, 1, FSIZE, f);
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    int part1 = 0, part2 = 0;
    for (const char *s = input; *s; ) {
        const int a = parseint(&s);
        const int b = parseint(&s);
        const int c = parseint(&s);
        const int d = parseint(&s);
        part1 += (a >= c && b <= d) || (a <= c && b >= d);  // fully contained
        part2 += a <= d && b >= c;  // overlap
    }
    printf("%d %d\n", part1, part2);  // 584 933

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
