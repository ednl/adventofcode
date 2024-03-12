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
 *     Mac Mini 2020 (M1 3.2 GHz)          :  30 µs
 *     iMac 2013 (i5 Haswell 4570 3.2 GHz) :  67 µs
 *     Raspberry Pi 5 (2.4 GHz)            :  47 µs
 *     Raspberry Pi 4 (1.8 GHz)            : 129 µs
 */

#include <stdio.h>
#include <stdint.h>  // uint16_t
#include <stdbool.h>
#include <string.h>  // memset
#include "../startstoptimer.h"

#define CHARVAL(C) ((C) - 'a' + 1)  // +1 to tell apart "a" and "aa"
#define BASE CHARVAL('z')           // 'z' - 'a' + 1 = 26 letters

// Number of wires = 340 (my input: max. wire name "ma" = index 339)
// To anticipate any two-letter wire name, change to "zz"
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

static Wire wire[N];
static bool incache[N];  // separate array for easy reset

// Convert wire name to array index while advancing char pointer
// E.g.: a=1, aa=27, zz=702 (max. index for my input: ma=339)
static uint16_t wireindex(char* wirename[])
{
    int id = CHARVAL(*(*wirename)++);
    if (**wirename >= 'a' && **wirename <= 'z')
        id = id * BASE + CHARVAL(*(*wirename)++);
    return (uint16_t)id;
}

// Read unsigned int value from string while advancing char pointer
static uint16_t parseint(char* digits[])
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
    Wire* const w = &wire[index];
    if (incache[index])  // shortcut if already evaluated
        return w->val;
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
    w->val = x;  // save as shortcut for next evaluation of same wire
    incache[index] = true;
    return x;
}

int main(void)
{
    starttimer();
    FILE* f = fopen("../aocinput/2015-07-input.txt", "r");
    if (!f) return 1;

    // Parse input file
    char line[32];
    while (fgets(line, sizeof line, f)) {  // read with newline
        Wire w = {0};  // .func = EQ, .val = 0, .arg[] = 0, .iswire[] = false
        uint16_t args = 0;  // argument count
        char* s = line;
        while (*s) {
            if (*s >= 'a' && *s <= 'z') {         // argument is a wire name
                w.iswire[args] = true;
                w.arg[args++] = wireindex(&s);
            } else if (*s >= '0' && *s <= '9') {  // argument is a number
                w.arg[args++] = parseint(&s);     // from init: .iswire[] = false
            } else if (*s >= 'A' && *s <= 'Z') {  // input function name
                switch (*s) {
                    // EQ (patch / direct assignment) already covered by init: EQ=0
                    case 'N': w.func = NOT; break;
                    case 'A': w.func = AND; break;
                    case 'O': w.func = OR;  break;
                    case 'L': w.func = SHL; break;
                    case 'R': w.func = SHR; break;
                }
                while (*s != ' ') ++s;  // skip rest of function name
            } else if (*s == '-') {
                s += 3;  // skip arrow and space
                wire[wireindex(&s)] = w;
            }
            ++s;  // skip space or newline
        }
    }
    fclose(f);

    // Part 1
    const uint16_t a = eval(CHARVAL('a'));
    printf("Part 1: %u\n", a);                   // 46065

    // Part 2
    memset(incache, 0, sizeof incache);          // reset cache
    wire[CHARVAL('b')] = (Wire){.arg[0] = a};    // set rule: "<value> -> b"
    printf("Part 2: %u\n", eval(CHARVAL('a')));  // 14134

    printf("Time: %.0f us\n", stoptimer_us());
    return 0;
}
