/**
 * Advent of Code 2020
 * Day 2: Password Philosophy
 * https://adventofcode.com/2020/day/2
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 02.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 02.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  7.82 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :     ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : 13.4  µs
 */

#include <stdio.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2020-02-input.txt"
#define L 30    // max line length 3+3+3+20+1
#define N 1000  // lines in input file
#define FSIZE (L * N)  // needed for my input: 20507

static char input[FSIZE];

// Read unsigned number with 1-2 digits
static int parseint(const char **s)
{
    int x = *(*s)++ & 15;
    if (**s >= '0')  // stop at dash/space
        x = x * 10 + (*(*s)++ & 15);
    (*s)++;  // skip dash/space
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

    int valid1 = 0, valid2 = 0;
    for (const char *c = input; *c; ++c) {
        const int min = parseint(&c);
        const int max = parseint(&c);
        const char ch = *c;
        const char *pw = (c += 3);
        int count = 0;
        while (*c != '\n')
            count += ch == *c++;
        valid1 += min <= count && count <= max;
        valid2 += (pw[min - 1] == ch) ^ (pw[max - 1] == ch);
    }
    printf("%d %d\n", valid1, valid2);  // 500 313

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
