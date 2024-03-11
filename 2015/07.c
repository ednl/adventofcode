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
 *     iMac 2013 (i5 Haswell 4570 3.2 GHz) :  68 µs
 *     Raspberry Pi 5 (2.4 GHz)            :  50 µs
 *     Raspberry Pi 4 (1.8 GHz)            : 131 µs
 */

#include <stdio.h>
#include <stdint.h>  // uint16_t
#include <stdbool.h>
#include <string.h>  // memset
#include "../startstoptimer.h"

#define BASE 26  // 26 letters
#define N   512  // max. wire name "ma" = 339

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

// Convert wire name to array index, e.g.: a=1, aa=27, zz=702.
// (max. index for my input: "ma" = 339)
// Also advance the char pointer.
static uint16_t namehash(char* s[])
{
    int id = *(*s)++ - 'a' + 1;  // +1 to tell apart "a" and "aa"
    if (**s >= 'a' && **s <= 'z')
        id = id * BASE + *(*s)++ - 'a' + 1;
    return (uint16_t)id;
}

// Read unsigned int value from string.
// Also advance the char pointer.
static uint16_t intval(char* s[])
{
    int n = *(*s)++ - '0';
    while (**s >= '0' && **s <= '9')
        n = n * 10 + *(*s)++ - '0';
    return (uint16_t)n;
}

// Recursively evaluate a wire by index.
// (max. recursion depth for my input = 40)
static uint16_t eval(const uint16_t index)
{
    Wire* const w = &wire[index];
    if (incache[index])  // already evaluated?
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
    w->val = x;  // save wire value to avoid more recursion
    incache[index] = true;
    return x;
}

int main(void)
{
    starttimer();

    // Parse input file
    FILE* f = fopen("../aocinput/2015-07-input.txt", "r");
    if (!f) return 1;
    char line[32];
    while (fgets(line, sizeof line, f)) {
        Wire w = {0};  // .func = EQ, .val = 0, .arg[i] = 0, .iswire[i] = false
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

    // Part 1
    const uint16_t a = eval(1);  // wire name "a" = index 1
    printf("Part 1: %u\n", a);  // 46065

    // Part 2
    memset(incache, 0, sizeof incache);  // reset cache
    wire[2] = (Wire){EQ, 0, {a, 0}, {false, false}};  // set rule: "<value> -> b"
    printf("Part 2: %u\n", eval(1));  // 14134

    printf("Time: %.0f us\n", stoptimer_us());
    return 0;
}
