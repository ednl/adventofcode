/**
 * Advent of Code 2024
 * Day 3: Mull It Over
 * https://adventofcode.com/2024/day/3
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 03.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 03.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  6.04 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : ? µs
 */

#include <stdio.h>
#include <unistd.h>   // isatty, fileno
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2024-03-input.txt"
#define FSIZE (5 << 12)  // 20480, needed for my input: 19928

static char input[FSIZE];

// Parse consecutive digits as integer, update char pointer
// NB: my input contains only numbers 1-999, already restricted to allowed range
static int readnum(const char **s)
{
    int x = 0;
    while (**s >= '0' && **s <= '9')
        x = x * 10 + (*(*s)++ & 15);
    return x;
}

static bool match(const char *restrict str, const char *restrict pre)
{
    for (; *str == *pre; str++, pre++);
    return !*pre;
}

int main(void)
{
    if (isatty(fileno(stdin))) {
        // Read input file from disk
        FILE *f = fopen(FNAME, "rb");  // fread() requires binary mode
        if (!f) { fputs("File not found: "FNAME, stderr); return 1; }
        fread(input, 1, sizeof input, f);  // read single bytes until EOF
        fclose(f);
    } else
        // Read input or example file from pipe or redirected stdin
        fread(input, 1, sizeof input, stdin);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    int sum1 = 0, sum2 = 0;
    bool enabled = true;  // "At the beginning, mul instructions are enabled."
    for (const char *c = input; *c; ) {
        if (*c != 'm' && *c != 'd') {
            c++;
            continue;
        }
        if (match(c, "mul(")) {
            c += 4;
            const int a = readnum(&c);
            if (*c == ',') {
                c++;
                const int b = readnum(&c);
                if (*c == ')') {
                    c++;
                    const int prod = a * b;
                    sum1 += prod;
                    sum2 += prod * enabled;
                }
            }
        } else if (match(c, "do()")) {
            c += 4;
            enabled = true;
        } else if (match(c, "don't()")) {
            c += 7;
            enabled = false;
        } else
            c++;
    }
    printf("%u %u\n", sum1, sum2);  // 181345830 98729041

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
