/**
 * Advent of Code 2024
 * Day 12: Garden Groups
 * https://adventofcode.com/2024/day/12
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 12alt.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 12alt.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  77.8 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 166   µs
 *     Raspberry Pi 5 (2.4 GHz)      : 323   µs
 */

#include <stdio.h>
#include <stdint.h>  // uint64_t, uint8_t
#include <stdbool.h>
#ifdef TIMER
    #include <string.h>  // memset
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2024-12-input.txt"
#define N 140
#define STKSZ 128  // max needed for my input: 116

// Derived values
#define COLS (N + 1)  // +newline
#define FSIZE (N * COLS)  // input file size
#define MAPSZ ((N + 2) * COLS)  // +border rows top and bottom
#define SETSZ ((MAPSZ + 63) >> 6)  // how many u64 in bitset (= 34)
#define BEG (COLS)
#define END (BEG + FSIZE - 1)

// Cardinal directions
#define TOP (1u << 0)
#define BTM (1u << 1)
#define LFT (1u << 2)
#define RGT (1u << 3)

// Diagonal directions
#define TL (TOP | LFT)
#define BL (BTM | LFT)
#define TR (TOP | RGT)
#define BR (BTM | RGT)

typedef struct vec {
    int x, y;
} Vec;

static char map[MAPSZ];  // input file incl. newlines and border rows top+bottom
static uint64_t seen[SETSZ];  // which destinations already visited? (part 1)
static int stack[STKSZ];  // grid locations (index) still to process
static int stacklen;

// Add vectors in place ("by reference")
static void add_r(Vec *const a, const Vec b)
{
    a->x += b.x;
    a->y += b.y;
}

static void mark(const int ix)
{
    seen[ix >> 6] |= UINT64_C(1) << (ix & 63);
}

static bool ismarked(const int ix)
{
    return seen[ix >> 6] >> (ix & 63) & 1;
}

// Save onto stack if not seen yet
// Assume stack always has room, needed for my input: 116
static void push(const int ix)
{
    if (ismarked(ix))
        return;
    mark(ix);
    stack[stacklen++] = ix;
}

// Remove from stack, set row/col, return false if stack was empty
static bool pop(int *const ix)
{
    if (!stacklen)
        return false;
    *ix = stack[--stacklen];
    return true;
}

// Explore plot with same plants starting from index ix
// Return plot price for parts 1 and 2 together in one Vec
static Vec price(int ix)
{
    const char plant = map[ix];
    int area = 0, perim = 0, corners = 0;  // number of corners is same as sides
    mark(ix);
    do {
        area++;  // every plant only counted once
        // Part 1, and neighbours inventory for part 2
        unsigned nb = 0;
        if (map[ix - COLS] == plant) { nb |= TOP; push(ix - COLS); } else perim++;
        if (map[ix + COLS] == plant) { nb |= BTM; push(ix + COLS); } else perim++;
        if (map[ix - 1]    == plant) { nb |= LFT; push(ix - 1);    } else perim++;
        if (map[ix + 1]    == plant) { nb |= RGT; push(ix + 1);    } else perim++;
        // Part 2
        switch (nb & TL) {  // top-left
            case TL: if (map[ix - COLS - 1] == plant) break;
            case 0: corners++;
        }
        switch (nb & TR) {  // top-right
            case TR: if (map[ix - COLS + 1] == plant) break;
            case 0: corners++;
        }
        switch (nb & BL) {  // bottom-left
            case BL: if (map[ix + COLS - 1] == plant) break;
            case 0: corners++;
        }
        switch (nb & BR) {  // bottom-right
            case BR: if (map[ix + COLS + 1] == plant) break;
            case 0: corners++;
        }
    } while (pop(&ix));
    return (Vec){area * perim, area * corners};
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) { fputs("File not found: "FNAME, stderr); return 1; }
    fread(&map[BEG], FSIZE, 1, f);
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
    memset(seen, 0, sizeof seen);
#endif

    Vec sum = {0};
    for (int i = BEG; i < END; ++i)
        if ((map[i] & 64) && !ismarked(i))  // skip [^A-Z] (=newlines) and seen
            add_r(&sum, price(i));
    printf("%u %u\n", sum.x, sum.y);  // 1361494 830516

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
