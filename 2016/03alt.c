/**
 * Advent of Code 2016
 * Day 3: Squares With Three Sides
 * https://adventofcode.com/2016/day/3
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *   cc -std=c17 -Wall -Wextra -pedantic -O3 -march=native -mtune=native ../startstoptimer.c 03.c
 * Get minimum runtime from timer output in bash:
 *     m=9999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 2.13 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :    ? µs
 *     Raspberry Pi 5 (2.4 GHz)      :    ? µs
 */

#include <stdio.h>
#include "../startstoptimer.h"

#define FNAME    "../aocinput/2016-03-input.txt"
#define LINES    1902
#define FIELDS   3
#define DATALEN  3  // max number of digits per number
#define FIELDLEN (DATALEN + 2)  // 2 spaces at the start of every field
#define LINELEN  (FIELDS * FIELDLEN + 1)  // +newline
#define FSIZE    (LINES * LINELEN)
#define LOOP     1000

static char input[FSIZE];
static int data[LINES][FIELDS];

static int triangle(int a, int b, int c)
{
    if (a > b) {
        const int tmp = b;
        b = a;
        a = tmp;
    }
    if (b > c) {
        const int tmp = c;
        c = b;
        b = tmp;
    }
    return a + b > c;
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    fread(input, sizeof input, 1, f);
    fclose(f);

    starttimer();
    int n = 0, m = 0;
    for (int loop = 0; loop < LOOP; ++loop) {
        const char *c = input;
        for (int i = 0; i < LINES; ++i, ++c)  // also skip newline
            for (int j = 0; j < FIELDS; ++j) {
                c += FIELDLEN - DATALEN;  // 2 spaces
                int x = 0;
                for (int k = 0; k < DATALEN; ++k)
                    x = x * 10 + (*c++ & 15);  // also works for leading spaces
                data[i][j] = x;
            }
        for (int i = 0; i < LINES; i += FIELDS) {
            const int *const a = &data[i][0];
            n += triangle(a[0], a[1], a[2]);
            n += triangle(a[3], a[4], a[5]);
            n += triangle(a[6], a[7], a[8]);
            m += triangle(a[0], a[3], a[6]);
            m += triangle(a[1], a[4], a[7]);
            m += triangle(a[2], a[5], a[8]);
        }
    }
    const double t = stoptimer_us();
    printf("%d %d\n", n / LOOP, m / LOOP);  // 982 1826
    printf("Time: %.0f ns\n", t);
}
