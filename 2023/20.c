/**
 * Advent of Code 2023
 * Day 20: Pulse Propagation
 * https://adventofcode.com/2023/day/20
 * By: E. Dronkert https://github.com/ednl
 */

#include <stdio.h>
#include <stdbool.h>

#define NAME "../aocinput/2023-20-example.txt"
#define MODULES 58
#define BUSSIZE 6

typedef enum signal {
    LO, HI, NOP
} Signal;

typedef enum type {
    NONE, BC, FLIP, CONJ
} Rank;

typedef struct bus {
    int id[BUSSIZE];
    Signal val[BUSSIZE];
    int len;
} Bus;

// %
typedef struct flipflop {
    int id;
    bool on;
    Signal inp, out;

} Flipflop;

// &
typedef struct conjunction {
    int id;
    Signal out;
} Conjunction;

static Module module[MODULES];

int main(void)
{
    FILE* f = fopen(NAME, "r");
    if (!f)
        return 1;
    char buf[32];
    int n = 0;
    while (n < MODULES && fgets(buf, sizeof buf, f)) {
        switch (buf[0]) {
            case 'b': module[n].type = BC; break;
            case '%': module[n].type = FLIP; break;
            case '&': module[n].type = CONJ; break;
        }
        ++n;
    }
    fclose(f);
    return 0;
}
