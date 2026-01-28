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
 *     m=9999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 *     (optionally replace './a.out' with 2nd or 3rd run command above)
 * Minimum runtime measurements including result output:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 0.326 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :     ? µs
 *     Raspberry Pi 5 (2.4 GHz)      :     ? µs
 * Minimum runtime measurements NOT including result output:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 0.037 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :     ? µs
 *     Raspberry Pi 5 (2.4 GHz)      :     ? µs
 */

#include <stdio.h>     // fopen, fclose, fread, FILE, fputs, fprintf, stderr
#include <unistd.h>    // isatty, fileno, write, STDOUT_FILENO
#include <stdlib.h>    // div, div_t
#include <stdint.h>    // uint64_t, UINT64_C
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
#define MUL UINT64_C(252533)    // multiplier (base of the modular exponentiation)
#define VAL UINT64_C(20151125)  // starting value at triangle pos (1,1) = index 0
#define MOD UINT64_C(33554393)  // modulus of the modular exponentiation

static char input[FSIZE];

// Read unsigned int, minus 1 because row/col are one-based
static unsigned readnum(const char *s)
{
    unsigned x = 0;
    while (*s >= '0')
        x = x * 10 + (*s++ & 15);
    return x - 1;
}

// Raw number output without printf
static void printint(int x)
{
    char buf[sizeof x * 4];
    char *end = buf + sizeof buf;
    char *pc = end;
    *--pc = '\n';  // terminate with newline
	do {
        const div_t qr = div(x, 10);
		*--pc = '0' + qr.rem;  // only works for non-negative x
		x = qr.quot;
	} while (x);
    // write() doesn't guarantee writing everything at once
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

    // Declare result variable outside timing loop
    volatile uint64_t rem;

#ifdef TIMER
    starttimer(); for (int i = 0; i < 1000; ++i) {
#endif

    // Parse input
    const unsigned row = readnum(input + ROW);
    const unsigned col = readnum(input + COL);

    // Index of row/col position on triangle grid
    const unsigned tri = row + col;
    uint64_t exp = col + (tri * (tri + 1) >> 1);

    // https://en.wikipedia.org/wiki/Modular_exponentiation
    rem = VAL;
    for (uint64_t base = MUL; exp; exp >>= 1) {
        if (exp & 1)
            rem = rem * base % MOD;
        base = base * base % MOD;
    }

#ifdef TIMER
    } fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());
#endif

    printint(rem);  // 19980801
}
