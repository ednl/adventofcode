/**
 * Advent of Code 2024
 * Day 3: ?
 * https://adventofcode.com/2024/day/3
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -Wall -Wextra 03.c
 *    gcc   -std=gnu17 -Wall -Wextra 03.c
 * Enable timer:
 *    clang -DTIMER -O3 -march=native 03.c ../startstoptimer.c
 *    gcc   -DTIMER -O3 -march=native 03.c ../startstoptimer.c
 * Get minimum runtime from timer output:
 *     m=999999;for((i=0;i<5000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Mac Mini 2020 (M1 3.2 GHz)                       :   ? µs
 *     iMac 2013 (Core i5 Haswell 4570 3.2 GHz)         :   ? µs
 *     Raspberry Pi 5 (2.4 GHz)                         :   ? µs
 *     Macbook Air 2013 (Core i5 Haswell 4250U 1.3 GHz) :  86 µs
 *     Raspberry Pi 4 (1.8 GHz)                         :   ? µs
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2024-03-input.txt"
#define FSIZE (5 << 12)  // 20480 > inpput file size

#define MUL  (*(int32_t *)"mul(")
#define DO   (*(int32_t *)"do()")
#define DONT (*(int64_t *)"don't()")
#define MASK ((1LL << 56) - 1)  // "don't()" is 7 bytes

static char input[FSIZE];

// Parse 1-3 digit positive number, update char pointer, return 0 if incorrect
static int num(const char **const c, const char sep)
{
    if (**c < '1' || **c > '9')
        return 0;
    int x = *(*c)++ & 15;
    for (int i = 0; i < 2 && **c >= '0' && **c <= '9'; ++i)
        x = x * 10 + (*(*c)++ & 15);
    if (**c == sep) {
        ++(*c);
        return x;
    }
    return 0;
}

int main(void)
{
#ifdef TIMER
    starttimer();
#endif

    // Read input file
    FILE *f = fopen(FNAME, "rb");
    if (!f) { fputs("File not found.\n", stderr); return 1; }
    fread(input, sizeof input, 1, f);  // read whole file at once
    fclose(f);

    int sum1 = 0, sum2 = 0;
    bool enabled = true;
    for (const char *c = input; *c; ) {
        switch (*c) {
        case 'm':
            if (*(int32_t *)c == MUL) {
                c += 4;
                int a, b;
                if ((a = num(&c, ',')) && (b = num(&c, ')'))) {
                    const int mul = a * b;
                    sum1 += mul;
                    sum2 += mul * enabled;
                }
            } else
                ++c;
            break;
        case 'd':
            if (*(int32_t *)c == DO) {
                enabled = true;
                c += 4;
            } else if ((*(int64_t *)c & MASK) == DONT) {
                enabled = false;
                c += 7;
            } else
                ++c;
            break;
        default:
            ++c;
            break;
        }
    }
    printf("%d %d\n", sum1, sum2);  // 181345830 98729041

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
    return 0;
}
