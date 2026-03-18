/**
 * Advent of Code 2017
 * Day 12: Digital Plumber
 * https://adventofcode.com/2017/day/12
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 12.c
 * Enable timer:
 *     cc -std=gnu17 -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 12.c
 * Test with timer enabled, without a million lines of identical output:
 *     ./a.out | tail -n1                 built-in file name
 *     ./a.out myinput.txt | tail -n1     custom file name
 *     ./a.out < myinput.txt | tail -n1   redirected input
 * Get minimum runtime from timer output on stderr:
 *     ./a.out 2>&1 1>/dev/null
 * Minimum runtime measurements, includes parsing and output:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 13.6 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 22.3 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 50.9 µs
 */

#include <stdio.h>
#include <unistd.h>  // fileno, isatty
#include <string.h>  // memset
#ifdef TIMER
    #include "../startstoptimer.h"
    #define LOOPS 100000  // takes ~1.5 seconds for 14 µs loop
#endif

#define FNAME "../aocinput/2017-12-input.txt"
#define FSIZE (36 * 1024)  // needed for my input: 35543
#define N 2000  // 2000 "programs" (nodes)
#define M 6     // max 6 "pipes" (edges) from any program

typedef struct program {
    int group;
    int pipe[M];
} Program;

static char input[FSIZE];
static Program program[N];

// Fast atoi() for known-good space/comma/newline-separated ASCII input
// Parse unsigned int, advance char pointer
static int readnum(const char **s)
{
    int x = 0;
    while (**s & 16)  // until space, comma or newline where the 16 bit is not set
        x = x * 10 + (*(*s)++ & 15);
    return x;
}

// Recursively count and connect nodes in the same graph
static int connect(int id, int group)
{
    Program *const p = &program[id];  // convenience pointer
    if (p->group)  // already in this (or any) group?
        return 0;  // don't count again
    p->group = group;
    int count = 1 + connect(p->pipe[0], group);  // always at least one pipe
    for (int j = 1; j < M && p->pipe[j]; ++j)    // until no more pipe
        count += connect(p->pipe[j], group);
    return count;
}

int main(int argc, char *argv[])
{
    size_t fsize = 0;
    if (isatty(fileno(stdin))) {
        // Read input file from disk
        const char *fname = argc > 1 ? argv[1] : FNAME;  // file name as argument, or built-in
        FILE *f = fopen(fname, "rb");
        if (!f) {
            fprintf(stderr, "File not found: %s\n", fname);
            return 1;
        }
        fsize = fread(input, 1, FSIZE, f);
        fclose(f);
    } else
        // Read from pipe or redirected input
        fsize = fread(input, 1, FSIZE, stdin);
    // End of input file
    const char *const end = input + fsize;

#ifdef TIMER
    double mintime = 1e9;
    for (int loop = 0; loop < LOOPS; ++loop) {
        memset(program, 0, sizeof program);  // reset before every timing loop
        starttimer();
#endif

    // Parse known-good input file
    for (const char *c = input; c != end; ++c) {  // skip newline
        const int id = readnum(&c);
        c += 5;  // skip " <-> "
        program[id].pipe[0] = readnum(&c);
        for (int i = 1; *c == ','; ++i) {  // while current char is comma
            c += 2;  // skip ", "
            program[id].pipe[i] = readnum(&c);
        }
    }

    // Size of first group with program ID 0
    const int count = connect(0, 1);

    // Count groups for all other programs not in a group yet
    int group = 1;
    for (int n = 1; n < N; ++n)
        if (!program[n].group)
            connect(n, ++group);

    // Solution
    printf("%d %d\n", count, group);  // 145 207

#ifdef TIMER
        const double looptime = stoptimer_us();
        if (looptime < mintime)
            mintime = looptime;
    }
    fprintf(stderr, "Time: %.1f us\n", mintime);
#endif
}
