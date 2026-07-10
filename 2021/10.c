/**
 * Advent of Code 2021
 * Day 10: Syntax Scoring
 * https://adventofcode.com/2021/day/10
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 10.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 10.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  2.94 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :  5.91 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 10.7  µs
 */

#include <stdio.h>
#include <stdint.h>  // int64_t
#include <inttypes.h>  // PRId64
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2021-10-input.txt"
#define FSIZE 10000  // needed for my input: 9371
#define SSIZE  16    // needed for my input: 15
#define INCOMP 48    // needed for my input: 47

static char input[FSIZE];
static const char match[] = {
    [')'] = '(',
    [']'] = '[',
    ['}'] = '{',
    ['>'] = '<',
};
static const int16_t illegal[] = {
    [')'] = 3,
    [']'] = 57,
    ['}'] = 1197,
    ['>'] = 25137,
};
static const char complete[] = {
    ['('] = 1,
    ['['] = 2,
    ['{'] = 3,
    ['<'] = 4,
};
static char stack[SSIZE];
static int64_t incomp[INCOMP];

static void swap(const int i, const int j)
{
    const int64_t tmp = incomp[i];
    incomp[i] = incomp[j];
    incomp[j] = tmp;
}

// https://en.wikipedia.org/wiki/Quickselect
static int partition(const int l, const int r, const int pi)
{
    const int64_t pv = incomp[pi];
    swap(pi, r);
    int si = l;
    for (int i = l; i < r; ++i)
        if (incomp[i] <= pv)
            swap(si++, i);
    swap(r, si);
    return si;
}

// https://en.wikipedia.org/wiki/Quickselect
static int64_t qselect(const int l, const int r, const int k)
{
    if (l == r)
        return incomp[l];
    const int pi = partition(l, r, l);
    if (k == pi)
        return incomp[k];
    if (k < pi)
        return qselect(l, pi - 1, k);
    return qselect(pi + 1, r, k);
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    const char *const end = input + fread(input, 1, FSIZE, f);
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    int part1 = 0, count = 0;
    for (const char *c = input; c != end; ++c) {
        int sp = 0;
        for (; *c != '\n'; ++c) {
            const char m = match[*c];
            if (m) {  // closing bracket?
                if (m != stack[--sp]) {  // unmatched?
                    part1 += illegal[*c];
                    for (; *c != '\n'; ++c);  // skip to EOL
                    goto nextline;
                }
            } else
                stack[sp++] = *c;  // opening bracket
        }
        int64_t score = 0;
        while (sp)
            score = score * 5 + complete[stack[--sp]];
        incomp[count++] = score;
    nextline:;
    }
    printf("%d %"PRId64"\n", part1, qselect(0, count - 1, count / 2));  // 413733 3354640192

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
