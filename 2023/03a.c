/**
 * Advent of Code 2023
 * Day 3: Gear Ratios
 * https://adventofcode.com/2023/day/3
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
 *     Macbook Pro 2024 (M4 4.4 GHz) : ? µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : ? µs
 *     iMac 2013 (i5 4570 3.2 GHz)   : 40.9 µs
 *     Raspberry Pi 5 (2.4 GHz)      : ? µs
 */

#include <stdio.h>
#include <string.h>  // memset
#include <stdint.h>  // uint16_t
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2023-03-input.txt"
#define N 140             // "schematic" square size (= lines in input file)
#define M 1280            // max number count in file (needed for my input: 1203 +1 because 1-based)
#define G 512             // max '*' count in file (needed for my input: 364)
#define ROWS (N + 2)      // two extra rows as border
#define COLS (N + 1)      // one extra column for newline
#define FSIZE (N * COLS)  // input file size
#define END ((ROWS - 1) * COLS - 1)  // 1 past last index

typedef struct number {
    int val, beg, end;
} Number;

static char schematic[ROWS * COLS];
static int numindex[ROWS * COLS];
static Number number[M];
static int gearindex[G];
static int numcount, gearcount;

static inline bool isdigit(const char c)
{
    return c >= '0' && c <= '9';
}

static inline bool notdigit(const char c)
{
    return c < '0' || c > '9';
}

// 10 35 36 37 38 42 43 45 46 47 48 49 50 51 52 53 54 55 56 57 61 64
// LF  #  $  %  &  *  +  -  .  /  0  1  2  3  4  5  6  7  8  9  =  @
static bool issymbol(const char c)
{
    return c != '.' && notdigit(c);
}

static bool ispartnumber(const Number *const n)
{
    if (schematic[n->beg - 1] != '.' || schematic[n->end] != '.')
        return true;
    for (int i = n->beg - COLS - 1; i <= n->end - COLS; ++i)
        if (issymbol(schematic[i]))
            return true;
    for (int i = n->beg + COLS - 1; i <= n->end + COLS; ++i)
        if (issymbol(schematic[i]))
            return true;
    return false;
}

static int pattern(const int index)
{
    return ((numindex[index - 1] != 0) << 2) | ((numindex[index] != 0) << 1) | (numindex[index + 1] != 0);
}

static int gears(const int pat)
{
    switch (pat) {
        case 0: return 0;
        case 5: return 2;
    }
    return 1;
}

static int ratio(const int index, const int pat)
{
    if (!pat)
        return 1;
    if (pat & 2)
        return number[numindex[index]].val;
    switch (pat) {
        case 1: return number[numindex[index + 1]].val;
        case 4: return number[numindex[index - 1]].val;
    }
    return number[numindex[index - 1]].val * number[numindex[index + 1]].val;
}

static int gearratio(const int index)
{
    const int p1 = pattern(index);
    const int p2 = pattern(index - COLS);
    const int p3 = pattern(index + COLS);
    if (gears(p1) + gears(p2) + gears(p3) != 2)
        return 0;
    return ratio(index, p1) * ratio(index - COLS, p2) * ratio(index + COLS, p3);
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");  // fread requires binary mode
    if (!f) return 1;
    fread(&schematic[COLS], FSIZE, 1, f);  // read as single block
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
    numcount = gearcount = 0;
#endif

    memset(schematic, '.', COLS);
    memset(schematic + END, '.', COLS + 1);
    for (int i = 2 * COLS - 1; i != END; i += COLS)
        schematic[i] = '.';  // replace newline with dot

    int part1 = 0, part2 = 0;
    for (int i = COLS; i < END; ++i) {
        if (isdigit(schematic[i])) {
            numcount++;
            const int i0 = i;
            int x = 0;
            do {
                numindex[i] = numcount;
                x = x * 10 + (schematic[i] & 15);
            } while (isdigit(schematic[++i]));
            number[numcount] = (Number){x, i0, i};
        }
        if (schematic[i] == '*')
            gearindex[gearcount++] = i;
    }
    for (int i = 1; i <= numcount; ++i)
        if (ispartnumber(&number[i]))
            part1 += number[i].val;
    for (int i = 0; i < gearcount; ++i)
        part2 += gearratio(gearindex[i]);
    printf("%u %u\n", part1, part2);  // 536202 78272573

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
