/**
 * Advent of Code 2017
 * Day 8: I Heard You Like Registers
 * https://adventofcode.com/2017/day/8
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic -Wno-multichar 08.c
 * Enable timer:
 *     cc -std=gnu17 -Wno-multichar -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 08.c
 * Test with timer enabled, without a thousand lines of identical output:
 *     ./a.out | head -n1
 * Get minimum runtime from timer output on stderr:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 10.7 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :    ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : 48.0 µs
 */

#include <stdio.h>
#include <unistd.h>  // isatty, fileno
#include <stdint.h>  // int16_t
#include <stdbool.h>
#ifdef TIMER
#include "../startstoptimer.h"
#define LOOPS 1000  // 1000 timer loops (measuring ms means µs per loop)
#endif

#define FNAME "../aocinput/2017-08-input.txt"
#define FSIZE 32768 // needed for my input: 25146
#define REGS  512   // needed for my input and this hash function: 508

// Parameter index: register 0, operator (inc/dec), adjustment,
// literal "if", register 1, comparator, reference value
typedef enum par {
    R0, OP, ADJ, IF, R1, CMP, REF
} Par;

static char input[FSIZE];

// Hash of 1-3 characters (max = 508)
// Tried different init and shifts until no collisions for my input
static unsigned hash(const char *s)
{
    unsigned h = 83 ^ *s++;
    for (unsigned m = 2; *s & 64; m >>= 1) {  // until space which doesn't have the 64 bit
        h <<= m;
        h ^= *s++;
    }
    return h;
}

// Fast atoi(), specifically for known-good space/newline-separated input
static int getnum(const char *s)
{
    const int sign = *s == '-' ? (s++, -1) : 1;
    int x = *s++ & 15;  // assumes ASCII
    while (*s & 16)  // until space or newline which don't have the 16 bit
        x = x * 10 + (*s++ & 15);
    return x * sign;
}

int main(void)
{
    if (isatty(fileno(stdin))) {
        // Read input file from disk
        FILE *f = fopen(FNAME, "rb");
        if (!f) {
            fprintf(stderr, "File not found: "FNAME"\n");
            return 1;
        }
        fread(input, 1, FSIZE, f);
        fclose(f);
    } else
        // Read input file from stdin pipe/redirection
        fread(input, 1, FSIZE, stdin);

#ifdef TIMER
starttimer(); for (int loop = 0; loop < LOOPS; ++loop) {
#endif

    int16_t reg[REGS] = {0};  // reset at start of each timer loop for correct result
    int16_t max2 = 0;
    for (const char *c = input; *c; ++c) {  // skip newline, until zero byte
        // Tokenize line into space-separated parameters 0..6
        const char *p[7] = {c};
        for (int i = 1; i < 7; ++i) {
            for (; *c != ' '; ++c);  // skip to space
            p[i] = ++c;  // pointer to first letter
        }
        for (; *c != '\n'; ++c);  // skip to newline

        const int16_t val = reg[hash(p[R1])];
        const int16_t ref = getnum(p[REF]);
        bool ok = false;
        // Multi-character constants reversed: assumes little-endian system
        switch (*(const int16_t *)p[CMP]) {
            case ' <': ok = val <  ref; break;
            case ' >': ok = val >  ref; break;
            case '=!': ok = val != ref; break;
            case '=<': ok = val <= ref; break;
            case '==': ok = val == ref; break;
            case '=>': ok = val >= ref; break;
        }
        if (ok) {
            const int r0 = hash(p[R0]);
            const int16_t adj = getnum(p[ADJ]);
            reg[r0] += ((p[OP][0] & 1) * 2 - 1) * adj;  // 'i'=1, 'd'=-1
            if (reg[r0] > max2)
                max2 = reg[r0];
        }
    }
    // For REGS=512, simply checking all buckets is faster
    // than smart solution with separate set of hashes used
    int16_t max1 = 0;
    for (int i = 0; i < REGS; ++i)
        if (reg[i] > max1)
            max1 = reg[i];
    printf("%d %d\n", max1, max2);  // 3089 5391

#ifdef TIMER
// 1000 loops means µs measurement is ns per loop
} fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());
#endif
}
