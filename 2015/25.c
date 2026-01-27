/**
 * Advent of Code 2015
 * Day 25: Let It Snow
 * https://adventofcode.com/2015/day/25
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile with warnings:
 *     cc -std=c17 -Wall -Wextra -pedantic 25.c
 * Compile for speed:
 *     cc -O3 -march=native -mtune=native 25.c
 * Run program:
 *     ./a.out                  read input file from internal file name
 *     ./a.out < input.txt      read input file using redirected input
 *     cat input.txt | ./a.out  read input file using piped input
 * Get minimum runtime from timer output in Bash:
 *     m=9999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 *     (optionally replace './a.out' with 2nd or 3rd run command above)
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 0.417 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :     ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : 2.96  µs
 */

#include <stdio.h>     // fopen, fclose, fread, FILE
#include <unistd.h>    // isatty, fileno, write, STDOUT_FILENO
#include <stdlib.h>    // div, div_t
#include <stdint.h>    // uint64_t, UINT64_C, int64_t, INT64_C
#include <inttypes.h>  // PRId64
#include <time.h>      // clock_gettime, struct timespec, CLOCK_MONOTONIC/_RAW

#ifndef CLOCK_MONOTONIC_RAW
#define CLOCK_MONOTONIC_RAW CLOCK_MONOTONIC
#endif

// Input file
#define FNAME "../aocinput/2015-25-input.txt"
#define FSIZE 100  // input file size in bytes, assumes 2x 4-digit numbers
#define ROW 80  // index of start of row number in input file
#define COL 93  // same for col, assumes row number has 4 digits

// From puzzle description
// Ref. https://adventofcode.com/2015/day/25
#define MUL UINT64_C(252533)    // multiplier (base of the modular exponentiation)
#define VAL UINT64_C(20151125)  // starting value at triangle pos (1,1) = index 0
#define MOD UINT64_C(33554393)  // modulus of the modular exponentiation

static char input[FSIZE];
static struct timespec t0, t1;

static void starttimer(void)
{
    // Warn on Raspberry Pi if not running at max performance
    FILE *f = fopen("/sys/devices/system/cpu/cpufreq/policy0/scaling_governor", "r");
    if (f) {
        if (fgetc(f) != 'p' || fgetc(f) != 'e')
            fprintf(stderr,
                "Warning: CPU not optimised for performance.\n"
                "  Resolve: echo performance | sudo tee /sys/devices/system/cpu/cpufreq/policy0/scaling_governor\n"
                "  Setting will be restored to default 'ondemand' at reboot.\n");
        fclose(f);
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &t0);
}

static int64_t stoptimer_ns(void)
{
    clock_gettime(CLOCK_MONOTONIC_RAW, &t1);
    return INT64_C(1000000000) * (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec);
}

// Read unsigned int, minus 1 because row/col are one-based
static unsigned readnum(const char *s)
{
    unsigned x = 0;
    while (*s >= '0')
        x = x * 10 + (*s++ & 15);
    return x - 1;
}

// Raw number output without printf
static void printint(unsigned x)
{
    char buf[sizeof x * 4], *end = buf + sizeof buf, *pc = end;
    *--pc = '\n';
	do {
        const div_t qr = div(x, 10);
		*--pc = '0' + qr.rem;  // remainder of negative number is negative
		x = qr.quot;
	} while (x);
    for (ssize_t n; pc < end && (n = write(STDOUT_FILENO, pc, end - pc)) != -1; pc += n);
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

    starttimer();

    // Parse input
    const unsigned row = readnum(input + ROW);
    const unsigned col = readnum(input + COL);

    // Index of row/col position on triangle grid
    const unsigned tri = row + col;
    uint64_t exp = col + (tri * (tri + 1) >> 1);

    // https://en.wikipedia.org/wiki/Modular_exponentiation
    uint64_t rem = VAL;
    for (uint64_t base = MUL; exp; exp >>= 1) {
        if (exp & 1)
            rem = rem * base % MOD;
        base = base * base % MOD;
    }
    printint(rem);  // 19980801
    printf("Time: %"PRId64" ns\n", stoptimer_ns());
}
