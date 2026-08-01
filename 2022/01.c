/**
 * Advent of Code 2022
 * Day 1: Calorie Counting
 * https://adventofcode.com/2021/day/1
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 01.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 01.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  2.31 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :     ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : 10.4  µs
 */

#include <stdio.h>
#include <string.h>  // memset, memmove
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2022-01-input.txt"
#define FSIZE (8192 + 4096)  // needed for my input: 10516 (+1 as zero byte EOF marker)

static char input[FSIZE];
static int top[3];

static int parseint(const char **s)
{
    int x = *(*s)++ & 15;
    while (**s & '0')  // only digits and newlines in input
        x = x * 10 + (*(*s)++ & 15);
    (*s)++;  // skip newline
    return x;
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");  // fread requires binary mode
    if (!f) return 1;
    fread(input, 1, FSIZE, f);  // read single bytes until EOF or FSIZE
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
    memset(top, 0, sizeof top);  // reset before every run or max will be repeated
#endif

    for (const char *c = input; *c; c++) {  // size of 'input' must be greater than input file size
        int cal = 0;
        while (*c & '0')  // first char on line is digit, '\n' or '\0'
            cal += parseint(&c);

        // Sort into top 3
        if (cal > top[2]) {
            if (cal > top[1]) {
                if (cal > top[0]) {
                    memmove(&top[1], &top[0], 2 * sizeof *top);
                    top[0] = cal;
                } else {
                    top[2] = top[1];
                    top[1] = cal;
                }
            } else
                top[2] = cal;
        }
    }
    printf("%d %d\n", top[0], top[0] + top[1] + top[2]);  // 69795 208437

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
