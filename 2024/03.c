/**
 * Advent of Code 2024
 * Day 3: Mull It Over
 * https://adventofcode.com/2024/day/3
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 03.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 03.c
 * Get minimum runtime from timer output in bash:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz)                    :  13 µs
 *     Mac Mini 2020 (M1 3.2 GHz)                       :  19 µs
 *     iMac 2013 (Core i5 Haswell 4570 3.2 GHz)         :  29 µs
 *     Raspberry Pi 5 (2.4 GHz)                         :  35 µs
 *     Macbook Air 2013 (Core i5 Haswell 4250U 1.3 GHz) :   ? µs
 */

#include <stdio.h>
#include <unistd.h>   // isatty, fileno
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2024-03-input.txt"
#define FSIZE (5 << 12)  // 20480 >= input file size in bytes

// Match 4 characters at once, interpreted as 32-bit int (little-endian!)
#define MUL  0x286c756d  // *(int *)"mul("
#define DO   0x29286f64  // *(int *)"do()"
#define DON  0x276e6f64  // *(int *)"don'"
#define DONT 0x00292874  // *(int *)"t()"
#define MASK ((1 << 24) - 1)  // "'t()" is 3 bytes, so disregard last=MSB

// Don't rely on undefined behaviour
typedef int unaligned_int __attribute__((aligned(1)));

static char input[FSIZE];

// Parse consecutive digits as integer, update char pointer
// NB: my input contains only numbers 1-999, already restricted to allowed range
static int num(const char **const c)
{
    int x = 0;
    while (**c >= '0' && **c <= '9')
        x = x * 10 + (*(*c)++ & 15);
    return x;
}

// Parse 2 numbers, update char pointer, return product, or 0 if incorrect
static int pair(const char **const c)
{
    const int a = num(c);
    if (!a || **c != ',')  // must be 1-999 and must be followed by comma
        return 0;
    ++(*c);  // skip comma
    const int b = num(c);
    if (!b || **c != ')')  // must be 1-999 and must be followed by parenthesis
        return 0;
    ++(*c);  // skip closing parenthesis
    return a * b;
}

int main(void)
{
    if (isatty(fileno(stdin))) {
        // Read input file from disk
        FILE *f = fopen(FNAME, "rb");  // fread() requires binary mode
        if (!f) { fputs("File not found", stderr); return 1; }
        fread(input, sizeof input, 1, f);  // read whole file at once
        fclose(f);
    } else
        // Read input or example file from pipe or redirected stdin
        fread(input, sizeof input, 1, stdin);

#ifdef TIMER
    starttimer();
#endif

    // Matchy matchy
    int sum1 = 0, sum2 = 0, mul;
    bool enabled = true;  // "At the beginning, mul instructions are enabled."
    for (const char *c = input; *c; ) {
        // interpret char pointer as unaligned 32-bit int pointer
        // for the next 4 characters at once
        switch (*(unaligned_int *)c) {
        case MUL:  // "mul("
            c += 4;
            if ((mul = pair(&c))) {
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
            if ((*(unaligned_int *)c & MASK) == DONT) {  // disregard MSB = match 3 chars
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
