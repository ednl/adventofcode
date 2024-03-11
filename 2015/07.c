/**
 * Advent of Code 2015
 * Day 7: Some Assembly Required
 * https://adventofcode.com/2015/day/7
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -Ofast -march=native -Wall -Wextra 07.c ../startstoptimer.c
 *    gcc   -std=gnu17 -Ofast -march=native -Wall -Wextra 07.c ../startstoptimer.c
 * Get minimum runtime:
 *     m=999999;for((i=0;i<2000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * Minimum runtime:
 *     Mac Mini 2020 (M1 3.2 GHz)          :  31 µs
 *     iMac 2013 (i5 Haswell 4570 3.2 GHz) :   ? µs
 *     Raspberry Pi 5 (2.4 GHz)            :  50 µs
 *     Raspberry Pi 4 (1.8 GHz)            : 131 µs
 */

#include <stdio.h>
#include <stdint.h>  // uint16_t
#include <stdbool.h>
#include <string.h>  // memset
#include "../startstoptimer.h"

#define ORD(X) ((uint16_t)((X) - 'a' + 1))  // a..z = 1..26 (must make diff between "a" and "aa")
#define Z ORD('z')                          // 26 letters

#define EXAMPLE 0
#if EXAMPLE
    #define FNAME "../aocinput/2015-07-example.txt"
    #define N (ORD('y') + 1u)    // 25 + 1 = 26 (max wire name "y" = 25)
#else
    #define FNAME "../aocinput/2015-07-input.txt"
    #define A ORD('a')                        // index of "a" = 1
    #define B ORD('b')                        // index of "b" = 2
    #define N (Z * ORD('m') + ORD('a') + 1u)  // 26 * 13 + 1 + 1 = 340 (max wire name "ma" = 339)
#endif

// EQ means a direct patch from one wire to another, or to a value
// e.g.: "cd -> aa" or "0 -> b"
typedef enum func {
    EQ=0, NOT, AND, OR, SHL, SHR
} Func;

// A wire has a direct value or is connected to 1 or 2 other wires
// optionally combined via a function.
typedef struct wire {
    Func func;
    uint16_t arg[2];  // max. two arguments (wire index or direct value)
    bool iswire[2];   // whether arg[i] is another wire or a direct value
} Wire;

static Wire wire[N];
static uint16_t cache[N];  // cached value of a wire that has been evaluated
static bool incache[N];    // separate array for easy reset

// Convert wire name to array index, e.g.: a=1, aa=27, zz=702.
// Max. index for my input: ma=339.
// Advance the char pointer.
static uint16_t namehash(char* s[])
{
    uint16_t id = ORD(*(*s)++);
    while (**s >= 'a' && **s <= 'z')
        id = id * Z + ORD(*(*s)++);
    return id;
}

// Read unsigned int value from string.
// Advance the char pointer.
static uint16_t intval(char* s[])
{
    uint16_t n = (uint16_t)(*(*s)++ - '0');
    while (**s >= '0' && **s <= '9')
        n = n * 10 + (uint16_t)(*(*s)++ - '0');
    return n;
}

// Recursively evaluate a wire.
// Max. recursion depth for my input = 40.
static uint16_t eval(const uint16_t index)
{
    if (incache[index])  // already evaluated?
        return cache[index];  // for my imput: 174 hits per part
    const Wire* const w = &wire[index];
    uint16_t x = w->iswire[0] ? eval(w->arg[0]) : w->arg[0];
    uint16_t y = w->iswire[1] ? eval(w->arg[1]) : w->arg[1];
    switch (w->func) {
        case EQ : break;  // wire patch or direct value
        case NOT: x =  ~x; break;
        case AND: x &=  y; break;
        case OR : x |=  y; break;
        case SHL: x <<= y; break;
        case SHR: x >>= y; break;
    }
    cache[index] = x;  // save wire value to avoid more recursion
    incache[index] = true;
    return x;
}

int main(void)
{
    starttimer();

    // Parse input file
    FILE* f = fopen(FNAME, "r");
    if (!f) { fprintf(stderr, "File not found: %s\n", FNAME); return 1; }
    char line[32];
    while (fgets(line, sizeof line, f)) {
        Wire w = {0};  // .func = EQ, .arg[i] = 0, .iswire[i] = false
        uint16_t hash = 0, args = 0;
        char* s = line;
        while (*s != '\n' && *s != '\0') {
            while (*s == ' ') ++s;  // skip space(s)
            if (*s >= 'a' && *s <= 'z') {  // wire name
                w.arg[args] = namehash(&s);
                w.iswire[args++] = true;  // argument is another wire
            } else if (*s >= '0' && *s <= '9') {  // number (unsigned int)
                w.arg[args] = intval(&s);
                w.iswire[args++] = false;  // argument is a number
            } else if (*s >= 'A' && *s <= 'Z') {  // input function with 1 or 2 arguments
                switch (*s) {
                    // EQ (patch / direct assignment) already covered by init to 0=EQ
                    case 'N': w.func = NOT; break;
                    case 'A': w.func = AND; break;
                    case 'O': w.func = OR;  break;
                    case 'L': w.func = SHL; break;
                    case 'R': w.func = SHR; break;
                }
                while (*s != ' ') ++s;  // skip the rest of the function name
            } else if (*s == '-') {
                s += 3;  // skip arrow and space
                hash = namehash(&s);
            }
        }
        if (hash)
            wire[hash] = w;
    }
    fclose(f);

#if EXAMPLE
    const char wirenames[] = "defghixy";
    for (const char* s = wirenames; *s; ++s)
        printf("%c: %u\n", *s, eval(ORD(*s)));
    // d: 72
    // e: 507
    // f: 492
    // g: 114
    // h: 65412
    // i: 65079
    // x: 123
    // y: 456
#else
    // Part 1
    const uint16_t a = eval(A);
    printf("Part 1: %u\n", a);  // 46065

    // Part 2
    memset(incache, 0, sizeof incache);  // reset cache
    wire[B] = (Wire){EQ, {a, 0}, {false, false}};  // set rule "<part1_value> -> b"
    printf("Part 2: %u\n", eval(A));  // 14134
#endif

    printf("Time: %.0f us\n", stoptimer_us());
    return 0;
}
