/**
 * Advent of Code 2024
 * Day 2: Red-Nosed Reports
 * https://adventofcode.com/2024/day/2
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -O3 -march=native -Wall -Wextra 02.c ../startstoptimer.c
 *    gcc   -std=gnu17 -O3 -march=native -Wall -Wextra 02.c ../startstoptimer.c
 * Get minimum runtime:
 *     m=999999;for((i=0;i<5000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime:
 *     Mac Mini 2020 (M1 3.2 GHz)                       :  92 µs
 *     Raspberry Pi 5 (2.4 GHz)                         : 142 µs
 *     iMac 2013 (Core i5 Haswell 4570 3.2 GHz)         : 181 µs
 *     Macbook Air 2013 (Core i5 Haswell 4250U 1.3 GHz) : 231 µs
 *     Raspberry Pi 4 (1.8 GHz)                         : 449 µs
 */

#define USETIMER 1

#include <stdio.h>
#include <stdlib.h>  // abs
#include <stdbool.h>
#if USETIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2024-02-input.txt"
#define N 1000  // lines in input file
#define M 8     // max numbers per line ("levels per report")
#define LEN (M * 3 + 1)  // line buf size (2 digits + 1 space/newline per number, +'\0')

static int data[N][M];  // N reports with M levels
static int count[N];    // actual levels per report

// Parse 1- or 2-digit number, update char pointer
static inline int num(const char **const c)
{
    int x = (*(*c)++ & 15);
    if (**c >= '0' && **c <= '9')
        x = x * 10 + (*(*c)++ & 15);
    (*c)++;
    return x;
}

// +1 for a<b, -1 for a>b, 0 for a=b
static inline int cmp(const int a, const int b)
{
    return (a < b) - (a > b);
}

// Is this report safe? It has 'count' levels. If 0<=skip<count, skip that index.
static bool issafe(const int *const level, const int count, const int skip)
{
    const int end = count - 1 - (skip == count - 1);
    const int len = count - 1 - (skip >= 0 && skip < count);
    int sum = 0;
    bool safe = true;
    for (int i = 0; safe && i < end; ++i)
        if (i != skip) {
            const int j = skip != i + 1 ? i + 1 : i + 2;
            sum += cmp(level[i], level[j]);
            const int dist = abs(level[i] - level[j]);
            safe = safe && dist && dist < 4;
        }
    return safe && abs(sum) == len;
}

int main(void)
{
#if USETIMER
    starttimer();
#endif

    // Parse input file
    FILE *f = fopen(FNAME, "r");
    if (!f) { fputs("File not found.\n", stderr); return 1; }
    char buf[LEN];
    for (int i = 0; fgets(buf, sizeof buf, f); ++i) {
        int j = 0;
        for (const char *c = buf; *c; data[i][j++] = num(&c));
        count[i] = j;  // actual number of levels in this report
    }
    fclose(f);

    // Evaluate reports
    int safe = 0, damp = 0;
    for (int i = 0; i < N; ++i)      // for every report
        for (int skip = -1; skip < count[i]; ++skip)  // try different versions
            if (issafe(data[i], count[i], skip)) {
                safe += skip == -1;  // part 1
                ++damp;              // part 2
                break;               // stop at first safe version
            }
    printf("%d %d\n", safe, damp);   // 516 561

#if USETIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
    return 0;
}
