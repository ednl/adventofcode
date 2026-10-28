/**
 * Advent of Code 2025
 * Day 2: Gift Shop
 * https://adventofcode.com/2025/day/2
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
 *     Macbook Pro 2024 (M4 4.4 GHz) :  1.21 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :  1.89 µs
 *     Raspberry Pi 5 (2.4 GHz)      :  4.14 µs
 */

#include <stdio.h>
#include <stdlib.h>    // lldiv, lldiv_t
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRIu64
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2025-02-input.txt"
#define N 30  // ranges in input file
#define FSIZE 512  // needed for my input: 401

#define MAXDIGITS 10  // max digits of any number in input is 10
#define MAXPARTLEN (MAXDIGITS >> 1) // so in 2 parts that's partlen 5

static const int64_t pow10[MAXDIGITS + 1] = {
    INT64_C(1),   // 10^0: 1 digit
    INT64_C(10),  // 10^1: 2 digits, etc.
    INT64_C(100),
    INT64_C(1000),
    INT64_C(10000),
    INT64_C(100000),
    INT64_C(1000000),
    INT64_C(10000000),
    INT64_C(100000000),
    INT64_C(1000000000),
    INT64_C(10000000000)};  // 10^10: 11 digits (INT64_MAX=9223372036854775807: 19 digits)

static char input[FSIZE];
static int64_t range[N][2];
static int8_t digits[N][2];

// Smallest number with 'partlen' digits in base 10 which repeated is at least 'val'
static int64_t next(int64_t val, const int partlen)
{
    int64_t min = 0;
    do {
        const lldiv_t x = lldiv(val, pow10[partlen]);
        min = x.rem + (x.rem < min);
        val = x.quot;
    } while (val);
    return min;
}

// Largest number with 'partlen' digits in base 10 which repeated is at most 'val'
static int64_t prev(int64_t val, const int partlen)
{
    int64_t max = pow10[partlen] - 1;
    do {
        const lldiv_t x = lldiv(val, pow10[partlen]);
        max = x.rem - (x.rem > max);
        val = x.quot;
    } while (val);
    return max;
}

int main(void)
{
    // Read file from disk
    FILE *f = fopen(FNAME, "rb");
    if (!f) { fputs("File not found: "FNAME, stderr); return 1; }
    fread(input, 1, sizeof input, f);  // read single bytes until EOF
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    // Parse ranges
    const char *s = input;
    for (int i = 0; i < N; i++) {
        for (int j = 0, k; j < 2; ++s, ++j) {  // skip non-digit
            int64_t x = 0;
            for (k = 0; *s & 16; ++k)  // until '-', ',' or '\n'
                x = x * 10 + (*s++ & 15);
            range[i][j] = x;
            digits[i][j] = k;
        }
    }

    // Evaluate ranges
    int64_t sum1 = 0, sum2 = 0;
    for (int i = 0; i < N; ++i) {
        bool haspartlen[MAXPARTLEN + 1] = {0};  // partial sum for each partlen 1..5
        for (int partcount = 2; partcount <= digits[i][1]; ++partcount) {
            const int n = digits[i][0] + (digits[i][0] % partcount != 0);
            const int m = digits[i][1] - (digits[i][1] % partcount != 0);
            if (n != m)
                continue;
            const int partlen = n / partcount;
            const int64_t a = n == digits[i][0] ? next(range[i][0], partlen) : pow10[partlen - 1];
            const int64_t b = n == digits[i][1] ? prev(range[i][1], partlen) : pow10[partlen] - 1;
            if (a > b)
                continue;
            haspartlen[partlen] = true;
            const int64_t rangelen = b - a + 1;
            const int64_t partsum = a * rangelen + (((rangelen - 1) * rangelen) >> 1);
            int64_t rangesum = 0;
            for (int j = 0; j < partcount; ++j)
                rangesum = rangesum * pow10[partlen] + partsum;
            if (partcount == 2)
                sum1 += rangesum;  // part 1
            if (partlen == 2 && haspartlen[4])
                continue;  // skip duplicate (there are no partlen 6, 8, etc.)
            sum2 += rangesum;
            // Deduplicate overlapping entries, e.g.:
            //   range = 74025-113072
            //     2x3=6 : 100-112 includes 111111
            //     3x2=6 :  10- 11 includes 111111
            //     5x1   :   7-  9 no dup!
            //     6x1   :   1-  1 includes 111111
            //   range = 79725385-79874177
            //     2*4=8 : 7972-7986 includes 79797979
            //     4*2=8 :   79-  79 includes 79797979
            //   range = 4526-8370
            //     2*2=4 : 45-82 includes 5555, 6666, 7777
            //     4*1=4 :  5- 7 includes 5555, 6666, 7777
            if (partlen == 1 && !(n & 1))  // no odd lengths because multiples would be 6, 9 etc. and we don't have those
                for (int j = 2; j <= MAXPARTLEN; ++j)  // max partlen is 5
                    if (haspartlen[j])
                        sum2 -= rangesum;
        }
    }
    printf("%"PRIu64" %"PRIu64"\n", sum1, sum2);  // 30323879646 43872163557

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
