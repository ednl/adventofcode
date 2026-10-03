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
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  5.76 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :  8.89 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 26.0  µs
 */

#include <stdio.h>
#include <unistd.h>  // isatty, fileno
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2024-03-input.txt"
#define FSIZE (5U << 12)  // 20480, needed for my input: 19928

// Match 4 characters at once, interpreted as 32-bit unsigned (little-endian)
// Predefined to avoid non-standard multichar constants (or compile with -Wno-multichar)
#define MUL_ 0x286c756dU  // *(unsigned *)"mul(" = '(lum'
#define DO__ 0x29286f64U  // *(unsigned *)"do()" = ')(od'
#define DON_ 0x276e6f64U  // *(unsigned *)"don'" = '\'nod'

// Don't rely on undefined behaviour
// Alternative: memcpy
typedef unsigned u32_unaligned __attribute__((aligned(1)));

static char input[FSIZE];

// Parse consecutive digits as integer, update char pointer
// NB: my input contains only numbers 1-999, already restricted to allowed range
static unsigned readnum(const char **s)
{
    unsigned x = 0;
    while (**s >= '0' && **s <= '9')
        x = x * 10 + (*(*s)++ & 15);
    return x;
}

// Does 'str' start with 'pre'? Also skip all matching in str
// Undefined if str and pre have same length (will read beyond '\0')
static bool match(const char *restrict *str, const char *restrict pre)
{
    for (; **str == *pre; (*str)++, pre++);
    return !*pre;
}

int main(void)
{
    if (isatty(fileno(stdin))) {
        // Read input file from disk
        FILE *f = fopen(FNAME, "rb");  // fread() requires binary mode
        if (!f) { fputs("File not found: "FNAME, stderr); return 1; }
        fread(input, 1, sizeof input, f);  // read single bytes until EOF
        fclose(f);
    } else
        // Read input or example file from pipe or redirected stdin
        fread(input, 1, sizeof input, stdin);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    unsigned sum1 = 0, sum2 = 0, a, b;
    bool enabled = true;  // "At the beginning, mul instructions are enabled."
    for (const char *c = input; *c; ) {
        if (*c != 'm' && *c != 'd') {  // common case in front = major speedup
            c++;
            continue;
        }
        switch (*(u32_unaligned *)c) {
        case MUL_:  // "mul("
            c += 4;
            a = readnum(&c);
            if (*c == ',') {
                c++;
                b = readnum(&c);
                if (*c == ')') {
                    c++;
                    sum1 += a * b;
                    sum2 += a * b * enabled;
                }
            }
            break;
        case DON_:  // "don'"
            c += 4;
            if (match(&c, "t()"))
                enabled = false;
            break;
        case DO__:   // "do()"
            c += 4;
            enabled = true;
            break;
        default:
            c++;
        }
    }
    printf("%u %u\n", sum1, sum2);  // 181345830 98729041

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
