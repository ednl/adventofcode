/**
 * Advent of Code 2018
 * Day 20: A Regular Map
 * https://adventofcode.com/2018/day/20
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 20alt.c
 * Enable timer:
 *     cc -std=gnu17 -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 20alt.c
 * Get minimum runtime from timer output:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements, does not include reading from disk:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  45 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :  79 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 144 µs
 */

#include <stdio.h>
#include <stdint.h>  // int8_t, uint16_t, uint32_t
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2018-20-input.txt"
#define FSIZE   16384  // needed for my input: 14185
#define STACKSIZE 256  // needed for my input: 225
#define BIGDIST  1000  // distance >=1000 is big
#define BITINDEX    5  // 5 bits needed to count 0..31
#define WORDINDEX (16 - BITINDEX)  // 11 bits left to index bitfield of 32-bit words

// Interpret 2x 8-bit position as 16-bit index
// Assume every x,y fits in signed char
// Needed for my input: x:-52..47 y:-47..52 = 100x100 grid
typedef union pos {
    uint16_t index;  // top 11 bits = array index, bottom 5 bits = bit index
    struct { int8_t x, y; };
} Pos;

typedef struct room {
    Pos pos;
    uint16_t dist;
} Room;

// Solution
typedef struct sol {
    uint16_t max;  // part 1: max distance
    uint16_t big;  // part 2: big distance count
} Sol;

static char input[FSIZE];
static uint32_t seen[1 << WORDINDEX];  // 11-bit size
static Room stack[STACKSIZE];
static int sp;  // stackpointer

// No checks: assume stack is never full
static void push(const Room x)
{
    stack[sp++] = x;
}

// No checks: assume parentheses in input are grammatically correct
static Room pop(void)
{
    return stack[--sp];
}

// No checks: assume pipes in input are grammatically correct
static Room peek(void)
{
    return stack[sp - 1];
}

// Check new room against current solution
// Assume every room revisit has greater distance than first visit
static void check(Sol *const sol, const Room room)
{
    const unsigned i = room.pos.index >> 5;
    const uint32_t bit = 1u << (room.pos.index & 0x1f);
    if (seen[i] & bit)
        return;
    seen[i] |= bit;
    if (room.dist > sol->max)
        sol->max = room.dist;
    if (room.dist >= BIGDIST)
        sol->big++;
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) {
        fputs("File not found: "FNAME, stderr);
        return 1;
    }
    fread(input, 1, FSIZE, f);
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    Sol sol = {0};
    Room room = {0};
    for (const char *c = input; *c; ++c)
        switch (*c) {
            case '(': push(room); break;
            case ')': room = pop(); break;
            case 'E': room.pos.x++; room.dist++; check(&sol, room); break;
            case 'N': room.pos.y--; room.dist++; check(&sol, room); break;
            case 'S': room.pos.y++; room.dist++; check(&sol, room); break;
            case 'W': room.pos.x--; room.dist++; check(&sol, room); break;
            case '|': room = peek(); break;
        }
    printf("%d %d\n", sol.max, sol.big);  // 3810 8615

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
}
