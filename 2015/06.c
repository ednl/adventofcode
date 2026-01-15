/**
 * Advent of Code 2015
 * Day 6: Probably a Fire Hazard
 * https://adventofcode.com/2015/day/6
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 06.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 06.c
 * Get minimum runtime from timer output in bash:
 *     m=9999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 1052 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :    ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : 4695 µs
 */

#include <stdio.h>
#include <stdint.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2015-06-input.txt"
#define FSIZE 9531
#define N 1000

typedef struct range {
    int x0, y0, x1, y1;
} Range;

static char input[FSIZE];
static const char *const end = input + FSIZE;
static int_fast8_t lights1[N][N];
static int_fast8_t lights2[N][N];

static int8_t notneg(const int8_t a)
{
    return a > 0 ? a : 0;
}

static int readnum(const char **s)
{
    int n = 0;
    while (**s > ',')  // until ',' or ' ' or '\n'
        n = n * 10 + (*(*s)++ & 15);  // next digit
    return n;
}

static Range parse(const char *s)
{
    int a = readnum(&s);
    s += 1;  // skip comma
    int b = readnum(&s);
    s += 9;  // skip ' through '
    int c = readnum(&s);
    s += 1;  // skip comma
    int d = readnum(&s);
    return (Range){a, b, c + 1, d + 1};
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");  // fread requires binary mode
    if (!f) { fprintf(stderr, "File not found: %s\n", FNAME); return 1; }
    fread(input, sizeof input, 1, f);  // read whole file at once
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    Range r;
    for (const char *c = input; c != end; ++c) {
        switch (*(c + 6)) {  // first unique char for each action
            case 'n':  // turn on
                r = parse(c + 8);
                for (int i = r.x0; i < r.x1; ++i)
                    for (int j = r.y0; j < r.y1; ++j) {
                        lights1[i][j] = 1;
                        lights2[i][j] += 1;
                    }
                break;
            case 'f':  // turn off
                r = parse(c + 9);
                for (int i = r.x0; i < r.x1; ++i)
                    for (int j = r.y0; j < r.y1; ++j) {
                        lights1[i][j] = 0;
                        lights2[i][j] = notneg(lights2[i][j] - 1);
                    }
                break;
            case ' ':  // toggle
                r = parse(c + 7);
                for (int i = r.x0; i < r.x1; ++i)
                    for (int j = r.y0; j < r.y1; ++j) {
                        lights1[i][j] ^= 1;
                        lights2[i][j] += 2;
                    }
                break;
        }
        for (c += 26; *c != '\n'; ++c);  // shortest line is 26 chars + newline
    }

    int sum1 = 0, sum2 = 0;
    for (int i = 0; i < N * N; ++i) {
        sum1 += *((int_fast8_t *)lights1 + i);
        sum2 += *((int_fast8_t *)lights2 + i);
    }
    printf("%d %d\n", sum1, sum2);  // 377891 14110788

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
}
