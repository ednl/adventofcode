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
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 24 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :  ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : 58 µs
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

typedef unsigned uint;

typedef union vec2 {
    uint arr[2];
    struct { uint x0, x1; };
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
    uint op, a, b, c;
} Code;

typedef void (*microcode)(uint *reg, uint a, uint b, uint c);

static const uint set[SIZE] = {
    0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080,
    0x0100, 0x0200, 0x0400, 0x0800, 0x1000, 0x2000, 0x4000, 0x8000,
};

static const uint clr[SIZE] = {
    0xfffe, 0xfffd, 0xfffb, 0xfff7, 0xffef, 0xffdf, 0xffbf, 0xff7f,
    0xfeff, 0xfdff, 0xfbff, 0xf7ff, 0xefff, 0xdfff, 0xbfff, 0x7fff,
};

static char input[FSIZE];
static uint before[TEST][REG];
static uint after[TEST][REG];
static Code test[TEST];
static Code prog[PROG];
static uint match[SIZE];
static uint block[SIZE];

static void parse_reg(uint *reg, const char **str)
{
    *str += 9;
    for (int i = 0; i < REG; ++i, *str += 3)
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

static Vec2 parse(const uint fsize)
{
    uint i = 0, j = 0;
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
    return (Vec2){.x0 = i, .x1 = j};
}

static void runall(uint *restrict out, const uint *restrict reg, const uint a, const uint b)
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

static void addr(uint *r, uint a, uint b, uint c) { r[c] = r[a] +  r[b]; }
static void addi(uint *r, uint a, uint b, uint c) { r[c] = r[a] +    b ; }
static void mulr(uint *r, uint a, uint b, uint c) { r[c] = r[a] *  r[b]; }
static void muli(uint *r, uint a, uint b, uint c) { r[c] = r[a] *    b ; }
static void banr(uint *r, uint a, uint b, uint c) { r[c] = r[a] &  r[b]; }
static void bani(uint *r, uint a, uint b, uint c) { r[c] = r[a] &    b ; }
static void borr(uint *r, uint a, uint b, uint c) { r[c] = r[a] |  r[b]; }
static void bori(uint *r, uint a, uint b, uint c) { r[c] = r[a] |    b ; }
static void setr(uint *r, uint a, uint b, uint c) { r[c] = r[a];         }
static void seti(uint *r, uint a, uint b, uint c) { r[c] =   a ;         }
static void gtrr(uint *r, uint a, uint b, uint c) { r[c] = r[a] >  r[b]; }
static void gtir(uint *r, uint a, uint b, uint c) { r[c] =   a  >  r[b]; }
static void gtri(uint *r, uint a, uint b, uint c) { r[c] = r[a] >    b ; }
static void eqrr(uint *r, uint a, uint b, uint c) { r[c] = r[a] == r[b]; }
static void eqir(uint *r, uint a, uint b, uint c) { r[c] =   a  == r[b]; }
static void eqri(uint *r, uint a, uint b, uint c) { r[c] = r[a] ==   b ; }

static const microcode optable[SIZE] = {addr, addi, mulr, muli, banr, bani, borr, bori, setr, seti, gtrr, gtir, gtri, eqrr, eqir, eqri};
static microcode func[SIZE];

int main(void)
{
    // Read input file from disk
    FILE *f = fopen(FNAME, "rb");  // binary mode required for fread
    if (!f) {
        fputs("File not found: "FNAME, stderr);
        return 1;
    }
    const uint fsize = fread(input, 1, FSIZE, f);
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    // Parse input
    const Vec2 inputsize = parse(fsize);

    // Part 1
    uint threeormore = 0;
    for (uint i = 0; i < inputsize.x0; ++i) {
        uint out[SIZE];
        runall(out, before[i], test[i].a, test[i].b);
        uint *const m = &match[test[i].op];
        uint *const b = &block[test[i].op];
        const uint res = after[i][test[i].c];
        uint behavelike = 0;
        for (int j = 0; j < SIZE; ++j)
            if (out[j] == res) {
                behavelike++;
                if (!(*b & set[j]))  // if not blocked
                    *m |= set[j];    // set match
            } else {
                *m &= clr[j];  // clear match
                *b |= set[j];  // set block
            }
        threeormore += behavelike >= 3;
    }
    printf("%u\n", threeormore);  // 570

    // Part 2
    uint rowcount[SIZE];
    uint only1[SIZE];
    uint done = 0;
    for (uint i = 0; i < SIZE; ++i)
        if ((rowcount[i] = __builtin_popcount(match[i])) == 1)
            only1[done++] = i;
    while (done < SIZE) {
        for (uint i = 0; i < done; ++i) {
            const uint k = only1[i];
            const uint reset = ~match[k];
            for (uint j = 0; j < SIZE; ++j)
                if (rowcount[j] > 1)
                    match[j] &= reset;
        }
        done = 0;
        for (uint i = 0; i < SIZE; ++i)
            if ((rowcount[i] = __builtin_popcount(match[i])) == 1)
                only1[done++] = i;
    }
    for (int i = 0; i < SIZE; ++i)
        func[i] = optable[31 - __builtin_clz(match[i])];

    uint reg[REG] = {0};
    for (uint i = 0; i < inputsize.x1; ++i)
        (*func[prog[i].op])(reg, prog[i].a, prog[i].b, prog[i].c);
    printf("%u\n", reg[0]);  // 503

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
}
