/**
 * Advent of Code 2015
 * Day 7: Some Assembly Required
 * https://adventofcode.com/2015/day/7
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 07.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 07.c
 * Get minimum runtime from timer output in bash:
 *     m=9999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  5.75 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :  ?    µs
 *     Raspberry Pi 5 (2.4 GHz)      : 21.0  µs
 */

#include <stdio.h>
#include <string.h>  // memset
#include <stdint.h>  // uint16_t
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2015-07-input.txt"
#define FSIZE 5303
#define CHARVAL(C) ((C) - 'a' + 1)  // +1 to tell apart "a" and "aa"
#define BASE CHARVAL('z')           // 'z' - 'a' + 1 = 26 letters

// Number of wires = 340 (my input: max. wire name "ma" = index 339)
// To anticipate _any_ two-letter wire name, change to "zz"
#define N (BASE * CHARVAL('m') + CHARVAL('a') + 1)

// EQ means a direct patch from one wire to another, or to a value
// e.g.: "cd -> aa" or "0 -> b"
typedef enum func {
    EQ=0, NOT, AND, OR, SHL, SHR
} Func;

// A wire has a direct value or is connected to 1 or 2 other wires
// optionally combined via a function.
typedef struct wire {
    Func func;
    uint16_t val;     // cached result
    uint16_t arg[2];  // max. two arguments (wire index or direct value)
    bool iswire[2];   // whether arg[i] is another wire or a direct value
} Wire;

static char input[FSIZE];
static const char *const end = input + FSIZE;
static Wire wire[N];
static bool incache[N];  // separate array for easy reset

// Convert wire name to array index while advancing char pointer
// E.g.: a=1, aa=27, zz=702 (max. index for my input: ma=339)
static uint16_t wireindex(const char **wirename)
{
    int id = CHARVAL(*(*wirename)++);
    if (**wirename >= 'a' && **wirename <= 'z')
        id = id * BASE + CHARVAL(*(*wirename)++);
    return (uint16_t)id;
}

// Read unsigned int value from string while advancing char pointer
// Pre: **digits is a valid digit 0..9
static uint16_t parseint(const char **digits)
{
    int n = *(*digits)++ - '0';
    while (**digits >= '0' && **digits <= '9')
        n = n * 10 + *(*digits)++ - '0';
    return (uint16_t)n;
}

// Recursively evaluate a wire by index.
// (max. recursion depth for my input = 40)
static uint16_t eval(const uint16_t index)
{
    Wire *const w = &wire[index];  // convenient alias
    if (incache[index])  // already evaluated?
        return w->val;   // shortcut ("memoisation")
    uint16_t x = w->iswire[0] ? eval(w->arg[0]) : w->arg[0];
    const uint16_t y = w->iswire[1] ? eval(w->arg[1]) : w->arg[1];
    switch (w->func) {
        case EQ : break;  // wire patch or direct value
        case NOT: x =  ~x; break;
        case AND: x &=  y; break;
        case OR : x |=  y; break;
        case SHL: x <<= y; break;
        case SHR: x >>= y; break;
    }
    incache[index] = true;
    return (w->val = x);  // save as shortcut for next evaluation of same wire
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");  // fread requires binary mode
    if (!f) { fprintf(stderr, "File not found: %s\n", FNAME); return 1; }
    fread(input, sizeof input, 1, f);  // read whole file at once
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    // Parse input file
    for (const char *c = input; c != end; ++c) {  // skip newline
        Wire w = {0};  // .func = EQ, .val = 0, .arg[] = 0, .iswire[] = false
        uint16_t args = 0;  // argument count
        for (;; ++c) {  // skip space
            if (*c >= 'a' && *c <= 'z') {         // argument is a wire name
                w.iswire[args] = true;
                w.arg[args++] = wireindex(&c);
            } else if (*c >= '0' && *c <= '9') {  // argument is a number
                w.arg[args++] = parseint(&c);     // already set: .iswire[] = false
            } else if (*c >= 'A' && *c <= 'Z') {  // input function name
                switch (*c) {
                    case 'N': w.func = NOT; c += 3; break;
                    case 'A': w.func = AND; c += 3; break;
                    case 'O': w.func = OR;  c += 2; break;
                    case 'L': w.func = SHL; c += 3; break;
                    case 'R': w.func = SHR; c += 3; break;
                }
            } else if (*c == '-') {
                c += 3;  // skip arrow and space (patch / direct assignment already set: EQ=0)
                wire[wireindex(&c)] = w;
                break;  // *c=='\n': EOL
            }
        }
    }

    // Part 1
    const uint16_t a = eval(CHARVAL('a'));
    printf("Part 1: %u\n", a);                   // 46065

    // Part 2
    memset(incache, 0, sizeof incache);          // reset cache
    wire[CHARVAL('b')] = (Wire){.arg[0] = a};    // set rule: "<value_from_part1> -> b"
    printf("Part 2: %u\n", eval(CHARVAL('a')));  // 14134

#ifdef TIMER
    printf("Time: %.0f ns\n", stoptimer_ns());
#endif
}
