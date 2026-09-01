/**
 * Advent of Code 2023
 * Day 1: Trebuchet?!
 * https://adventofcode.com/2023/day/1
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
 *     Macbook Pro 2024 (M4 4.4 GHz) : 13.2 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 19.3 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 44.9 µs
 */

#include <stdio.h>   // fopen, fclose, fread, printf
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2023-01-input.txt"
#define FSIZE 32768  // greater than size of input file (my input: 21985)

#define MASK15 ((1U << 15) - 1)  // 15-bit mask
#define MASK20 ((1U << 20) - 1)  // 20-bit mask
#define MASK25 ((1U << 25) - 1)  // 25-bit mask

#define ONE   ((('o' - 'a') << 10) | (('n' - 'a') << 5) | ('e' - 'a'))
#define SIX   ((('s' - 'a') << 10) | (('i' - 'a') << 5) | ('x' - 'a'))
#define TWO   ((('t' - 'a') << 10) | (('w' - 'a') << 5) | ('o' - 'a'))
#define FIVE  ((('f' - 'a') << 15) | (('i' - 'a') << 10) | (('v' - 'a') << 5) | ('e' - 'a'))
#define FOUR  ((('f' - 'a') << 15) | (('o' - 'a') << 10) | (('u' - 'a') << 5) | ('r' - 'a'))
#define NINE  ((('n' - 'a') << 15) | (('i' - 'a') << 10) | (('n' - 'a') << 5) | ('e' - 'a'))
#define EIGHT ((('e' - 'a') << 20) | (('i' - 'a') << 15) | (('g' - 'a') << 10) | (('h' - 'a') << 5) | ('t' - 'a'))
#define SEVEN ((('s' - 'a') << 20) | (('e' - 'a') << 15) | (('v' - 'a') << 10) | (('e' - 'a') << 5) | ('n' - 'a'))
#define THREE ((('t' - 'a') << 20) | (('h' - 'a') << 15) | (('r' - 'a') << 10) | (('e' - 'a') << 5) | ('e' - 'a'))

static char input[FSIZE];

// Forwards sliding window of compounded chars to look for substrings
// between start (inclusive) and end (exclusive).
static int lookfwd(const char *start, const char *const end, const int def)
{
    if (start + 3 > end) return def;
    // Startup 3/4/3.
    unsigned win = 0;
    for (int i = 0 ; i < 3 ; ++i)
        win = win << 5 | (unsigned)(*start++ - 'a');
    switch (win) {
        case ONE: return 10;  // "one"
        case SIX: return 60;  // "six"
        case TWO: return 20;  // "two"
    }
    if (start == end) return def;
    win = win << 5 | (unsigned)(*start++ - 'a');
    switch (win) {
        case FIVE: return 50;  // "five"
        case FOUR: return 40;  // "four"
        case NINE: return 90;  // "nine"
    }
    switch (win & MASK15) {
        case ONE: return 10;  // "one"
        case SIX: return 60;  // "six"
        case TWO: return 20;  // "two"
    }
    // Loop 5/4/3, 5/4/3, etc.
    while (start < end) {
        win = win << 5 | (unsigned)(*start++ - 'a');
        switch (win & MASK25) {
            case EIGHT: return 80;  // "eight"
            case SEVEN: return 70;  // "seven"
            case THREE: return 30;  // "three"
        }
        switch (win & MASK20) {
            case FIVE: return 50;  // "five"
            case FOUR: return 40;  // "four"
            case NINE: return 90;  // "nine"
        }
        switch (win & MASK15) {
            case ONE: return 10;  // "one"
            case SIX: return 60;  // "six"
            case TWO: return 20;  // "two"
        }
    }
    return def;
}

// Backwards sliding window of compounded chars to look for substrings
// between start (inclusive) and end (exclusive).
static int lookback(const char *start, const char *const end, const int def)
{
    if (start - 3 < end) return def;
    // Startup 3/4/3.
    unsigned win = 0;
    for (int i = 0 ; i < 3 ; ++i)
        win = win >> 5 | ((unsigned)(*start-- - 'a') << 20);
    switch (win >> 10) {
        case ONE: return 1;  // "one"
        case SIX: return 6;  // "six"
        case TWO: return 2;  // "two"
    }
    if (start == end) return def;
    win = win >> 5 | ((unsigned)(*start-- - 'a') << 20);
    switch (win >> 5) {
        case FIVE: return 5;  // "five"
        case FOUR: return 4;  // "four"
        case NINE: return 9;  // "nine"
    }
    switch (win >> 10) {
        case ONE: return 1;  // "one"
        case SIX: return 6;  // "six"
        case TWO: return 2;  // "two"
    }
    // Loop 5/4/3, 5/4/3, etc.
    while (start > end) {
        win = win >> 5 | ((unsigned)(*start-- - 'a') << 20);
        switch (win) {
            case EIGHT: return 8;  // "eight"
            case SEVEN: return 7;  // "seven"
            case THREE: return 3;  // "three"
        }
        switch (win >> 5) {
            case FIVE: return 5;  // "five"
            case FOUR: return 4;  // "four"
            case NINE: return 9;  // "nine"
        }
        switch (win >> 10) {
            case ONE: return 1;  // "one"
            case SIX: return 6;  // "six"
            case TWO: return 2;  // "two"
        }
    }
    return def;
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");  // fread requires binary mode
    if (!f) return 1;
    fread(input, 1, sizeof input, f);  // read single bytes until EOF
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    int part1 = 0, part2 = 0;
    for (const char *line = input, *end, *s1, *s2; *line; line = end + 2) {
        for (s1 = line; *s1 > '9'; ++s1);  // find first numerical digit (every line has at least one)
        const int d1 = (*s1 & 15) * 10;
        part1 += d1;
        part2 += lookfwd(line, s1, d1);  // look for first digit word BEFORE first numerical digit
        for (end = s1 + 1; *end != '\n'; ++end);  // newline char = end of this line
        for (s2 = --end; *s2 > '9' && s2 > s1; --s2);  // find last numerical digit backwards
        const int d2 = *s2 & 15;
        part1 += d2;
        part2 += lookback(end, s2, d2);  // look for last digit word AFTER last numerical digit
    }
    printf("%d %d\n", part1, part2);  // example: 209 281, input: 54630 54770

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
