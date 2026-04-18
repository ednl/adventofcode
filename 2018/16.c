/**
 * Advent of Code 2018
 * Day 16: Chronal Classification
 * https://adventofcode.com/2018/day/16
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic -Wno-unused-parameter 16.c
 * Enable timer:
 *     cc -std=gnu17 -Wno-unused-parameter -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 16.c
 * Get minimum runtime from timer output:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";sp
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 16 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :  ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : 47 µs
*/

#include <stdio.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2018-16-input.txt"
#define FSIZE 48000  // needed for my input: 46876

#define TEST 1024  // test samples, needed for my input: 773
#define PROG 1024  // program lines, needed for my input: 853
#define REG  4     // registers

typedef unsigned short u16;

typedef struct vec2 {
    u16 part1, part2;
} Vec2;

typedef enum assembly {
    ADDR, ADDI,
    MULR, MULI,
    BANR, BANI,
    BORR, BORI,
    SETR, SETI,
    GTRR, GTIR, GTRI,
    EQRR, EQIR, EQRI,
    SIZE  // assembly language size
} Assembly;

typedef struct code {
    u16 op, a, b, c;
} Code;

typedef void (*microcode)(u16 *reg, u16 a, u16 b, u16 c);

static char input[FSIZE];
static u16 before[TEST][REG];
static u16 after[TEST][REG];
static Code test[TEST];
static Code prog[PROG];
static u16 match[SIZE];

static void parse_reg(u16 *reg, const char **str)
{
    *str += 9;
    for (u16 i = 0; i < REG; ++i, *str += 3)
        reg[i] = **str & 15;
}

static void parse_code(Code *code, const char **str)
{
    if (*(*str + 1) == ' ')
        code->op = **str & 15;
    else {
        code->op = (**str & 15) * 10 + (*(*str + 1) & 15);
        (*str)++;
    }
    code->a = *(*str + 2) & 15;
    code->b = *(*str + 4) & 15;
    code->c = *(*str + 6) & 15;
    *str += 8;
}

static Vec2 parse(const u16 fsize)
{
    u16 i = 0, j = 0;
    const char *c = input;
    const char *const end = input + fsize;
    // Test samples
    for (; *c == 'B' && i < TEST; ++i) {
        parse_reg(before[i], &c);
        parse_code(&test[i], &c);
        parse_reg(after[i], &c);
        c++;  // empty line
    }
    // Program
    c += 2;
    for (; c < end && j < PROG; ++j)
        parse_code(&prog[j], &c);
    return (Vec2){.part1 = i, .part2 = j};
}

static void runall(u16 *restrict out, const u16 *restrict reg, const u16 a, const u16 b)
{
    out[ADDR] = reg[a] +  reg[b];
    out[ADDI] = reg[a] +      b ;
    out[MULR] = reg[a] *  reg[b];
    out[MULI] = reg[a] *      b ;
    out[BANR] = reg[a] &  reg[b];
    out[BANI] = reg[a] &      b ;
    out[BORR] = reg[a] |  reg[b];
    out[BORI] = reg[a] |      b ;
    out[SETR] = reg[a];
    out[SETI] =     a ;
    out[GTRR] = reg[a] >  reg[b];
    out[GTIR] =     a  >  reg[b];
    out[GTRI] = reg[a] >      b ;
    out[EQRR] = reg[a] == reg[b];
    out[EQIR] =     a  == reg[b];
    out[EQRI] = reg[a] ==     b ;
}

static void addr(u16 *r, u16 a, u16 b, u16 c) { r[c] = r[a] +  r[b]; }
static void addi(u16 *r, u16 a, u16 b, u16 c) { r[c] = r[a] +    b ; }
static void mulr(u16 *r, u16 a, u16 b, u16 c) { r[c] = r[a] *  r[b]; }
static void muli(u16 *r, u16 a, u16 b, u16 c) { r[c] = r[a] *    b ; }
static void banr(u16 *r, u16 a, u16 b, u16 c) { r[c] = r[a] &  r[b]; }
static void bani(u16 *r, u16 a, u16 b, u16 c) { r[c] = r[a] &    b ; }
static void borr(u16 *r, u16 a, u16 b, u16 c) { r[c] = r[a] |  r[b]; }
static void bori(u16 *r, u16 a, u16 b, u16 c) { r[c] = r[a] |    b ; }
static void setr(u16 *r, u16 a, u16 b, u16 c) { r[c] = r[a];         }
static void seti(u16 *r, u16 a, u16 b, u16 c) { r[c] =   a ;         }
static void gtrr(u16 *r, u16 a, u16 b, u16 c) { r[c] = r[a] >  r[b]; }
static void gtir(u16 *r, u16 a, u16 b, u16 c) { r[c] =   a  >  r[b]; }
static void gtri(u16 *r, u16 a, u16 b, u16 c) { r[c] = r[a] >    b ; }
static void eqrr(u16 *r, u16 a, u16 b, u16 c) { r[c] = r[a] == r[b]; }
static void eqir(u16 *r, u16 a, u16 b, u16 c) { r[c] =   a  == r[b]; }
static void eqri(u16 *r, u16 a, u16 b, u16 c) { r[c] = r[a] ==   b ; }

static const microcode funlist[SIZE] = {addr, addi, mulr, muli, banr, bani, borr, bori, setr, seti, gtrr, gtir, gtri, eqrr, eqir, eqri};
static microcode funmap[SIZE];

int main(void)
{
    // Read input file from disk
    FILE *f = fopen(FNAME, "rb");  // binary mode required for fread
    if (!f) {
        fputs("File not found: "FNAME, stderr);
        return 1;
    }
    const u16 fsize = fread(input, 1, FSIZE, f);
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    // Parse input
    const Vec2 inputsize = parse(fsize);

    // Part 1
    u16 threeormore = 0;
    u16 out[SIZE];
    for (u16 i = 0; i < inputsize.part1; ++i) {
        runall(out, before[i], test[i].a, test[i].b);
        const u16 res = after[i][test[i].c];
        u16 *const m = &match[test[i].op];
        u16 behavelike = 0;
        for (u16 j = 0; j < SIZE; ++j)
            if (out[j] == res) {
                behavelike++;
                *m |= 1U << j;
            }
        threeormore += behavelike >= 3;
    }
    printf("%u\n", threeormore);  // part 1: 570

    // Part 2
    u16 possible[SIZE];  // number of possibilities per opcode
    u16 solved[SIZE];  // stack of row indexes with only one match
    u16 sp = 0;  // stack pointer
    for (u16 i = 0; i < SIZE; ++i)
        if ((possible[i] = __builtin_popcount(match[i])) == 1)
            solved[sp++] = i;
    // Assumes solution exists, can be found via rows, and is unique
    while (sp < SIZE) {
        for (u16 i = 0; i < sp; ++i) {
            const u16 reset = ~match[solved[i]];
            for (u16 j = 0; j < SIZE; ++j)
                if (possible[j] > 1)
                    match[j] &= reset;  // delete this column from other rows
        }
        sp = 0;
        for (u16 i = 0; i < SIZE; ++i)
            if ((possible[i] = __builtin_popcount(match[i])) == 1)
                solved[sp++] = i;
    }

    // Condense match table to direct translation array
    for (u16 i = 0; i < SIZE; ++i)
        funmap[i] = funlist[31 - __builtin_clz(match[i])];

    // Run program
    u16 reg[REG] = {0};
    for (u16 i = 0; i < inputsize.part2; ++i)
        (*funmap[prog[i].op])(reg, prog[i].a, prog[i].b, prog[i].c);
    printf("%u\n", reg[0]);  // part 2: 503

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
}
