/**
 * Advent of Code 2022
 * Day 3: Rucksack Reorganization
 * https://adventofcode.com/2022/day/3
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
 *     Macbook Pro 2024 (M4 4.4 GHz) :  3.41 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :  5.51 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 16.0  µs
 */

#include <stdio.h>
#include <stdint.h>  // uint64_t, UINT64_C
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2022-03-input.txt"
#define FSIZE (8192 + 2048)  // needed for my input: 9954
#define N 300  // lines in input file
#define M 16   // minimum line length

typedef uint64_t u64;
static char input[FSIZE];
static int len[N];  // line lengths

// Check presence of letters in string
// Bit index ("priority"): a=1..z=26,A=27..Z=52
static u64 itemize(const char **s, const int len)
{
    u64 items = 0;
    const char *const end = *s + len;
    for (; *s != end; (*s)++)
        items |= UINT64_C(1) << ((**s & 31) + ((**s & 32) ? 0 : 26));
    return items;
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

    // Find all newlines
    const char *c = input;
    for (int i = 0; i < N; ++i) {
        const char *n = c + M;  // newline search starts here
        for (; *n != '\n'; ++n);
        len[i] = (n - c) >> 1;  // half line length
        c = n + 1;  // skip newline
    }

    c = input;
    int sum1 = 0, sum2 = 0;
    for (int i = 0; i < N; ) {
        u64 badge = -1;  // start with all items present
        for (int k = 0; k < 3; ++c, ++i, ++k) {  // batch of 3 rucksacks (lines)
            const u64 comp1 = itemize(&c, len[i]);
            const u64 comp2 = itemize(&c, len[i]);
            sum1 += __builtin_ctzll(comp1 & comp2);  // count trailing zeroes
            badge &= (comp1 | comp2);
        }
        sum2 += __builtin_ctzll(badge);
    }
    printf("%d %d\n", sum1, sum2);  // 7701 2644

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
