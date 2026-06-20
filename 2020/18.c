/**
 * Advent of Code 2020
 * Day 18: Operation Order
 * https://adventofcode.com/2020/day/18
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 18.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 18.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 19.8 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : ? µs
 */

#include <stdio.h>
#include <stdint.h>  // uint64_t
#include <inttypes.h>  // PRId64
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2020-18-input.txt"
#define FSIZE 24000  // needed for my input: 20422

typedef uint64_t u64;
static char input[FSIZE];
static char *ch = input;

static u64 sum1(void)
{
    u64 res = 0;
    char op = '+';
    for (;;)
        if (*ch >= '0') {
            if (op == '+')
                res += *ch++ & 15;
            else
                res *= *ch++ & 15;
        } else switch (*ch) {
            case '*': /* fall-through */
            case '+': op = *ch; ch += 2; break;
            case ' ': ch++; break;
            case '(': ch++; if (op == '+') res += sum1(); else res *= sum1(); break;
            default : ch++; return res;  // ')' or '\n'
        }
}

static u64 sum2(void)
{
    u64 res = 0;
    char op = '+';
    for (;;)
        if (*ch >= '0') {
            if (op == '+')
                res += *ch++ & 15;
            else
                res *= *ch++ & 15;
        } else switch (*ch) {
            case '*': ch += 2; return res * sum2();
            case '+': ch += 2; op = '+'; break;
            case ' ': ch++; break;
            case '(': ch++; if (op == '+') res += sum2(); else res *= sum2(); break;
            default : ch++; return res;  // ')' or '\n'
        }
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

    u64 part1 = 0, part2 = 0;
    for (ch = input; *ch; part1 += sum1());
    for (ch = input; *ch; part2 += sum2());
    printf("%"PRIu64" %"PRIu64"\n", part1, part2);  // 280014646144 9966990988262

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
