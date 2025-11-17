/**
 * Advent of Code 2024
 * Day 21: Keypad Conundrum
 * https://adventofcode.com/2024/day/21
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    cc -std=c17 -Wall -Wextra -pedantic 21.c
 * Enable timer:
 *    cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 21.c
 * Get minimum runtime from timer output in bash:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz)                    : ? µs
 *     Mac Mini 2020 (M1 3.2 GHz)                       : ? µs
 *     Raspberry Pi 5 (2.4 GHz)                         : ? µs
 */

#include <stdio.h>
#include <stdint.h>

#define NUMKEYS 11
#define DIRKEYS 5

typedef enum dir {
    ACT, RIGHT, DOWN, LEFT, UP
} Dir;

typedef struct pos {
    uint8_t x, y;
} Pos;

static const char *numpad[] = {
    "789",
    "456",
    "123",
    "_0A"};
static Pos numpos[11] = {  // order: A,0..9
    {2,3},{1,3},{0,2},{1,2},{2,2},{0,1},{1,1},{2,1},{0,0},{1,0},{2,0}};

static const char *dirpad[] = {
    "_^A",
    "<v>"};
static Pos dirpos[5] = {  // order: A,>,V,<,^ (same as 'enum dir')
    {2,0},{2,1},{1,1},{0,1},{1,0}};

static char movenum[NUMKEYS][NUMKEYS][6];  // max 5 moves on numpad
static char movedir[DIRKEYS][DIRKEYS][4];  // max 3 moves on dirpad

static Pos sub(const Pos a, const Pos b)
{
    return (Pos){a.x - b.x, a.y - b.y};
}

int main(void)
{
    // Define best moves on numpad
    for (int i = 0; i < NUMKEYS; ++i)
        for (int j = 0; j < NUMKEYS; ++j)
            if (i != j)
                ;

    // Define best moves on dirpad
    for (int i = 0; i < DIRKEYS; ++i)
        for (int j = 0; j < DIRKEYS; ++j)
            if (i != j)
                ;

    return 0;
}
