/**
 * Advent of Code 2024
 * Day 24: Crossed Wires
 * https://adventofcode.com/2024/day/24
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 24.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 24.c
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : ? µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : ? µs
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2024-24-input.txt"
#define FSIZE (5<<10)  // =5120, needed for my input: 4805
#define BITS 45  // add two 45-bit numbers with carry to 46-bit number
#define WIRES (1<<15)  // 3x 5-bit
#define SETSZ ((WIRES + 63)>>6)  // how many 64-bit numbers (=512)

typedef uint32_t u32;
typedef uint64_t u64;
typedef enum gatetype {AND,XOR,OR} GateType;
typedef struct gate {
    u32 inp1, inp2, outp;
    GateType type;
} Name;

static char input[FSIZE];
static uint64_t valid[SETSZ];
static uint64_t value[SETSZ];

static u32 wireid(const char *s)
{
    return ((*s & 31) << 10) | ((*(s + 1) & 31) << 5) | (*(s + 2) & 31);
}

// 'A' & 3 ^ 1 = 0
// 'X' & 3 ^ 1 = 1
// 'O' & 3 ^ 1 = 2
static GateType gatetype(const char *s)
{
    return (*s & 3) ^ 1;
}

static void set(const u32 ix, const bool val)
{
    const u32 word = ix >> 6;
    const u64 bit = UINT64_C(1) << (ix & 63);
    if (val)
        value[word] |= bit;
    else
        value[word] &= ~bit;
    valid[word] |= bit;
}

static bool get(const u32 ix, bool *const val)
{
    if (valid[ix >> 6] >> (ix & 63) & 1) {
        *val = value[ix >> 6] >> (ix & 63) & 1;
        return true;
    }
    return false;
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");  // binary mode needed for fread
    if (!f) { fputs("File not found: "FNAME, stderr); return 1; }
    fread(input, 1, sizeof input, f);  // read single bytes until EOF
    fclose(f);

    const char *s = input;
    for (u32 i = 0; i < (BITS * 2); s += 7, ++i) {
        u32 k = wireid(s);
        bool v = *(s + 5) & 1;
        set(k, v);
        if (get(k, &v))
            printf("%.*s: %5u = %u\n", 3, s, k, v);
    }
}
