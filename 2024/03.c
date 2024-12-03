/**
 * Advent of Code 2024
 * Day 3: Mull It Over
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
 *     Mac Mini 2020 (M1 3.2 GHz)                       :  32 µs
 *     Raspberry Pi 5 (2.4 GHz)                         :  51 µs
 *     Macbook Air 2013 (Core i5 Haswell 4250U 1.3 GHz) :  87 µs
 *     Raspberry Pi 4 (1.8 GHz)                         : 156 µs
 *     iMac 2013 (Core i5 Haswell 4570 3.2 GHz)         :   ? µs
 */

#include <stdio.h>
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2024-03-input.txt"
#define FSIZE (5 << 12)  // 20480 >= input file size in bytes

// Match 4 characters at once, interpreted as 32-bit int (little-endian)
#define MUL  0x286c756d  // *(int *)"mul("
#define DO   0x29286f64  // *(int *)"do()"
#define DON  0x276e6f64  // *(int *)"don'"
#define DONT 0x00292874  // *(int *)"t()"
#define MASK ((1 << 24) - 1)  // "'t()" is 3 bytes, so disregard MSB

static char input[FSIZE];

// Parse 1-3 digit positive number, update char pointer, return 0 if incorrect
static int num(const char **const c, const char sep)
{
    if (**c < '1' || **c > '9')  // only positive numbers = must start with 1..9
        return 0;
    int x = *(*c)++ & 15;  // first digit
    for (int i = 0; i < 2 && **c >= '0' && **c <= '9'; ++i)  // max 2 more digits
        x = x * 10 + (*(*c)++ & 15);
    if (**c == sep) {  // must be followed by either ',' or ')'
        ++(*c);  // skip correct separator
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
    FILE *f = fopen(FNAME, "rb");  // fread() requires binary mode
    if (!f) { fputs("File not found.\n", stderr); return 1; }
    fread(input, sizeof input, 1, f);  // read whole file at once
    fclose(f);

    // Matchy matchy
    int sum1 = 0, sum2 = 0, a, b;
    bool enabled = true;  // "At the beginning, mul instructions are enabled."
    for (const char *c = input; *c; ) {
        switch (*(int *)c) {  // interpret char pointer as 32-bit int pointer
        case MUL:  // "mul("
            c += 4;
            if ((a = num(&c, ',')) && (b = num(&c, ')'))) {
                const int mul = a * b;
                sum1 += mul;
                sum2 += mul * enabled;
            }
            break;
        case DO:   // "do()"
            c += 4;
            enabled = true;
            break;
        case DON:  // "don'"
            c += 4;
            if ((*(int *)c & MASK) == DONT) {  // disregard MSB = match 3 chars
                c += 3;
                enabled = false;
            }
            break;
        default:  // no instruction found, go to next char
            ++c;
        }
    }
    printf("%d %d\n", sum1, sum2);  // 181345830 98729041

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
    return 0;
}
