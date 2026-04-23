/**
 * Advent of Code 2018
 * Day 21: Chronal Conversion
 * https://adventofcode.com/2018/day/21
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 21.c
 * Enable timer:
 *     cc -std=gnu17 -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 21.c
 * Get minimum runtime from timer output:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements, includes all parsing but not reading from disk:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  ? µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 14 µs
 *     Raspberry Pi 5 (2.4 GHz)      :  ? µs
*/

#include <stdio.h>
#include <string.h>  // memset
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2018-21-input.txt"
#define FSIZE  512  // needed for my input: 390
#define MEMSIZE 32  // needed for my input: 31
#define REGCOUNT 6  // registers
#define REGINIT  0  // register index to initialise
#define PROGSTART 6

typedef unsigned int uint;

typedef uint (*microcode)(const uint a, const uint b);

typedef struct instr {
    microcode fun;
    uint *r;    // target register
    uint a, b;  // function parameters
} Instr;

static char input[FSIZE];    // text file
static Instr prog[MEMSIZE];  // program
static uint progsize;        // program size
static uint reg[REGCOUNT];   // registers
static uint *ipreg;          // pointer to special register

static uint addr(const uint a, const uint b) { return reg[a] +  reg[b]; }
static uint addi(const uint a, const uint b) { return reg[a] +      b ; }
static uint mulr(const uint a, const uint b) { return reg[a] *  reg[b]; }
static uint muli(const uint a, const uint b) { return reg[a] *      b ; }
static uint banr(const uint a, const uint b) { return reg[a] &  reg[b]; }
static uint bani(const uint a, const uint b) { return reg[a] &      b ; }
static uint borr(const uint a, const uint b) { return reg[a] |  reg[b]; }
static uint bori(const uint a, const uint b) { return reg[a] |      b ; }
static uint setr(const uint a, const uint b) { return reg[a]          ; }
static uint seti(const uint a, const uint b) { return     a           ; }
static uint gtrr(const uint a, const uint b) { return reg[a] >  reg[b]; }
static uint gtir(const uint a, const uint b) { return     a  >  reg[b]; }
static uint gtri(const uint a, const uint b) { return reg[a] >      b ; }
static uint eqrr(const uint a, const uint b) { return reg[a] == reg[b]; }
static uint eqir(const uint a, const uint b) { return     a  == reg[b]; }
static uint eqri(const uint a, const uint b) { return reg[a] ==     b ; }
static uint halt(const uint a, const uint b)
{
    printf("%u\n", reg[a]);
    return 1;
}

// Parse unsigned int, advance char pointer, skip one more
static uint parseint(const char **str)
{
    uint x = 0;
    while (**str >= '0')
        x = x * 10 + (*(*str)++ & 15);
    (*str)++;  // skip space/newline
    return x;
}

static void exec(const uint init)
{
    memset(reg, 0, sizeof reg);
    reg[REGINIT] = init;
    uint ip = PROGSTART;
    while (ip < progsize) {
        *ipreg = ip;
        *prog[ip].r = prog[ip].fun(prog[ip].a, prog[ip].b);
        ip = *ipreg + 1;
    }
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) {
        fputs("File not found: "FNAME, stderr);
        return 1;
    }
    const int fsize = fread(input, 1, FSIZE, f);
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    const char *c = input + 4;  // skip "#ip "
    ipreg = reg + parseint(&c);  // global index of special register
    const char *const end = input + fsize;
    uint n = 0;
    for (; c < end; ++n) {  // assume MEMSIZE is big enough
        microcode fun;
        switch (*(c + 1)) {  // second letter of the opcode is unique (apart from r/i)
            case 'a': fun = *(c + 3) == 'r' ? banr : bani; break;
            case 'd': fun = *(c + 3) == 'r' ? addr : addi; break;
            case 'e': fun = *(c + 3) == 'r' ? setr : seti; break;
            case 'o': fun = *(c + 3) == 'r' ? borr : bori; break;
            case 'q': fun = *(c + 2) == 'r' ? (*(c + 3) == 'r' ? halt : eqri) : eqir; break;
            case 't': fun = *(c + 2) == 'r' ? (*(c + 3) == 'r' ? gtrr : gtri) : gtir; break;
            case 'u': fun = *(c + 3) == 'r' ? mulr : muli; break;
            default: fprintf(stderr, "Unknown opcode on line %d\n", n + 1); return 2;
        }
        c += 5;  // skip 4-char opcode +space
        const uint par1 = parseint(&c);
        const uint par2 = parseint(&c);
        const uint par3 = parseint(&c);
        prog[n] = (Instr){fun, &reg[par3], par1, par2};
    }
    progsize = n;

    exec(0);  // part 1: 3173684
    prog[28].fun = eqrr;

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
}
