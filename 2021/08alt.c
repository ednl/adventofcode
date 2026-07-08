/**
 * Advent of Code 2021
 * Day 8: Seven Segment Search
 * https://adventofcode.com/2021/day/8
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 08alt.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native ../startstoptimer.c 08alt.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  6.18 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 14.0  µs
 *     Raspberry Pi 5 (2.4 GHz)      : 16.6  µs
 *
 * From: https://github.com/maneatingape/advent-of-code-rust/blob/main/src/year2021/day08.rs
 * Listing each digit and the number of segments that are lit when that digit is displayed:
 *
 * | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 |
 * |---|---|---|---|---|---|---|---|---|---|
 * | 6 | 2 | 5 | 5 | 4 | 5 | 6 | 3 | 7 | 6 |
 *
 * shows that 3 digits share 5 segments and another 3 share 6 segments so we don't have enough
 * information just yet. Listing the total occurrences of each segment summing across all 10 digits:
 *
 * | a | b | c | d | e | f | g |
 * |---|---|---|---|---|---|---|
 * | 8 | 6 | 8 | 7 | 4 | 9 | 7 |
 *
 * shows that 2 segments share 7 occurrences and 2 share 8 occurrences so this is still not quite enough
 * information. However, if we combine these 2 tables by *summing* the segment occurrences for each
 * digit, for example `1` has segments `c` and `f` for a total of 17, then the table looks like:
 *
 * | 0  |  1 |  2 |  3 |  4 |  5 |  6 |  7 |  8 |  9 |
 * |----|----|----|----|----|----|----|----|----|----|
 * | 42 | 17 | 34 | 39 | 30 | 37 | 41 | 25 | 49 | 45 |
 *
 * Now each digit can be uniquely identified.
 */

#include <stdio.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2021-08-input.txt"
#define N 200  // lines in input file
#define FSIZE (N * (61 + 4 * 8))  // max file size = 18600 (needed for my input: 16562)

// Unique sum per digit of segment occurrences across all 10 digits
static const char digit[] = {
    [42] = 0,
    [17] = 1,
    [34] = 2,
    [39] = 3,
    [30] = 4,
    [37] = 5,
    [41] = 6,
    [25] = 7,
    [49] = 8,
    [45] = 9,
};
static char input[FSIZE];

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
    const char *c = input;
    for (int i = 0; i < N; ++i) {
        // Build histogram of scrambled segment occurrences
        char hist[8] = {0};
        while (*c != '|')
            hist[*c++ & 7]++;  // space=0, 'a'=1, 'b'=2, etc.
        c += 2;  // skip "| "
        // Decode output value from scrambled segments
        int output = 0;
        for (int j = 0; j < 4; ++j) {
            int len = 0;  // number of lit segments (part 1: 2="1", 3="7", 4="4", 7="8")
            int sum = 0;  // sum of scrambled segment occurrences
            for (; *c >= 'a'; ++len)
                sum += hist[*c++ & 7];  // retrieve occurrence for 'a'=index 1, 'b'=index 2, etc.
            output = output * 10 + digit[sum];
            part1 += len < 5 || len > 6;
            c++;  // skip space or newline
        }
        part2 += output;
    }
    printf("%d %d\n", part1, part2);  // 534 1070188

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
