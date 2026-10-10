/**
 * Advent of Code 2024
 * Day 10: Hoof It
 * https://adventofcode.com/2024/day/10
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 10.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 10.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  9.25 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 14.9  µs
 *     Raspberry Pi 5 (2.4 GHz)      : 45.4  µs
 */

#include <stdio.h>
#include <string.h>  // memset
#include <stdint.h>  // uint64_t
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2024-10-input.txt"
#define N 45      // square grid dimension in input file
#define HEAD '0'  // trailhead
#define GOAL '9'  // end of the trail
#define STACK 16  // stack size, needed for my input: 8

// Derived values
#define COLS (N + 1)  // +newline
#define FSIZE (N * COLS)  // input file size
#define MAPSIZE ((N + 2) * COLS)  // plus border rows top+bottom
#define SETSIZE ((MAPSIZE + 63) >> 6)  // how many u64 in bitset (= 34)
#define BEG (COLS)  // first grid location inside map
#define END ((N + 1) * COLS - 1)  // last+1 grid location inside map

typedef struct pair {
    int part1, part2;
} Pair;

static char map[MAPSIZE];  // input file incl. newlines and border rows top+bottom
static uint64_t seen[SETSIZE];  // which destinations already visited? (part 1)
static int stack[STACK];  // grid locations (index) still to process
static int stacklen;

// Add vectors by reference: a+=b
static void add_r(Pair *const a, const Pair b)
{
    a->part1 += b.part1;
    a->part2 += b.part2;
}

static void mark(const int ix)
{
    seen[ix >> 6] |= UINT64_C(1) << (ix & 63);
}

static bool ismarked(const int ix)
{
    return seen[ix >> 6] >> (ix & 63) & 1;
}

// Save location onto stack
// Assume stack always large enough, needed for my input: 8
static void push(const int ix)
{
    stack[stacklen++] = ix;
}

// Retrieve location from stack
static bool pop(int *ix)
{
    if (stacklen) {
        *ix = stack[--stacklen];
        return true;
    }
    return false;
}

// Depth-first search (DFS), parts 1 & 2 combined
static Pair findtrails(int ix)
{
    Pair count = {0};
    memset(seen, 0, sizeof seen);  // for part 1
    do {
        const char height = map[ix];
        if (height != GOAL) {
            const char next = height + 1;
            if (map[ix - COLS] == next) push(ix - COLS);
            if (map[ix + COLS] == next) push(ix + COLS);
            if (map[ix - 1] == next) push(ix - 1);
            if (map[ix + 1] == next) push(ix + 1);
        } else {
            if (!ismarked(ix)) {  // part 1
                mark(ix);
                count.part1++;
            }
            count.part2++;  // part 2
        }
    } while (pop(&ix));
    return count;
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) { fputs("File not found: "FNAME, stderr); return 1; }
    fread(&map[BEG], FSIZE, 1, f);  // leave one row blank at top (and bottom)
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    Pair sum = {0};
    for (int i = BEG; i < END; ++i)
        if (map[i] == HEAD)  // find trails for every starting position
            add_r(&sum, findtrails(i));
    printf("%u %u\n", sum.part1, sum.part2);  // 552 1225

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
