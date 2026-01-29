/**
 * Advent of Code 2015
 * Day 25: Let It Snow
 * https://adventofcode.com/2015/day/25
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile with warnings:
 *     cc -std=c17 -Wall -Wextra -pedantic 25.c
 * Compile for speed:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 25.c
 * Run program:
 *     ./a.out                  read input file from internal file name
 *     ./a.out < input.txt      read input file using redirected input
 *     cat input.txt | ./a.out  read input file using piped input
 * Get minimum runtime from timer output in Bash:
 *     m=9999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 *     (optionally replace './a.out' with 2nd or 3rd run command above)
 *     output redirection is needed to only select the timing info, which is printed to stderr
 * Minimum runtime measurements including result output which is redirected to /dev/null in shell:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  89 ns
 *     Mac Mini 2020 (M1 3.2 GHz)    :   ? ns
 *     Raspberry Pi 5 (2.4 GHz)      : 309 ns
 */

#include <stdio.h>   // fopen, fclose, fread, FILE, fputs, fprintf, stdin, stderr
#include <unistd.h>  // isatty, fileno
#include <stdlib.h>  // div, div_t
#include <stdint.h>  // int64_t, INT64_C
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

// Input file
#define FNAME "../aocinput/2015-25-input.txt"
#define FSIZE 100  // input file size in bytes, assumes 2x 4-digit numbers
#define ROW 80  // index of start of row number in input file
#define COL 93  // same for col, assumes row number has 4 digits

// From puzzle description
// Ref. https://adventofcode.com/2015/day/25
#define MUL INT64_C(252533)    // multiplier (base of the modular exponentiation)
#define VAL INT64_C(20151125)  // starting value at triangle pos (1,1) = index 0
#define MOD INT64_C(33554393)  // modulus of the modular exponentiation

static char input[FSIZE];

// Read unsigned int, minus 1 because row/col are one-based
static int readnum(const char *s)
{
    int x = 0;
    while (*s >= '0')
        x = x * 10 + (*s++ & 15);
    return x - 1;
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
    starttimer(); for (int i = 0; i < 1000; ++i) {
#endif

    // Parse input
    const int row = readnum(input + ROW);
    const int col = readnum(input + COL);

    // Index of row/col position on triangle grid
    const int tri = row + col;
    int64_t exp = col + (tri * (tri + 1) >> 1);

    // https://en.wikipedia.org/wiki/Modular_exponentiation
    int64_t base = MUL;
    volatile int64_t rem = VAL;  // volatile not needed but is faster??
    for (; exp; exp >>= 1) {
        if (exp & 1)
            rem = rem * base % MOD;
        base = base * base % MOD;
    }
    printf("%d\n", (int)rem);  // 19980801

#ifdef TIMER
    } fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());
#endif
}
