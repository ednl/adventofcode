/**
 * Advent of Code 2015
 * Day 6: Probably a Fire Hazard
 * https://adventofcode.com/2015/day/6
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -O3 -march=native -Wall -Wextra 06.c ../startstoptimer.c
 *    gcc   -std=gnu17 -O3 -march=native -Wall -Wextra 06.c ../startstoptimer.c
 * Get minimum runtime:
 *     m=999999;for((i=0;i<5000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * Minimum runtime:
 *     Mac Mini 2020 (M1 3.2 GHz)                       :     ? µs
 *     Macbook Air 2013 (Core i5 Haswell 4250U 1.3 GHz) :  4843 µs
 *     iMac 2013 (Core i5 Haswell 4570 3.2 GHz)         :  4895 µs
 *     Raspberry Pi 5 (2.4 GHz)                         :  5343 µs
 *     Raspberry Pi 4 (1.8 GHz)                         : 50957 µs
 */

#include <stdio.h>
#include <stdint.h>
#include "../startstoptimer.h"

#define N 1000

typedef struct range {
    int x0, y0, x1, y1;
} Range;

static int8_t lights1[N][N];
static int8_t lights2[N][N];

static int8_t notneg(const int8_t a)
{
    return a > 0 ? a : 0;
}

static int readnum(const char** s)
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
    starttimer();
    FILE *f = fopen("../aocinput/2015-06-input.txt", "r");
    if (!f) { fputs("File not found.\n", stderr); return 1; }

    Range r;
    char buf[64];
    while (fgets(buf, sizeof buf, f))
        switch (buf[6]) {  // first unique char for each action
            case 'n':  // turn on
                r = parse(buf + 8);
                for (int i = r.x0; i < r.x1; ++i)
                    for (int j = r.y0; j < r.y1; ++j) {
                        lights1[i][j] = 1;
                        lights2[i][j] += 1;
                    }
                break;
            case 'f':  // turn off
                r = parse(buf + 9);
                for (int i = r.x0; i < r.x1; ++i)
                    for (int j = r.y0; j < r.y1; ++j) {
                        lights1[i][j] = 0;
                        lights2[i][j] = notneg(lights2[i][j] - 1);
                    }
                break;
            case ' ':  // toggle
                r = parse(buf + 7);
                for (int i = r.x0; i < r.x1; ++i)
                    for (int j = r.y0; j < r.y1; ++j) {
                        lights1[i][j] ^= 1;
                        lights2[i][j] += 2;
                    }
                break;
        }
    fclose(f);

    int sum1 = 0, sum2 = 0;
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j) {
            sum1 += lights1[i][j];
            sum2 += lights2[i][j];
        }
    printf("%d %d\n", sum1, sum2);  // 377891 14110788
    printf("Time: %.0f us\n", stoptimer_us());
    return 0;
}
