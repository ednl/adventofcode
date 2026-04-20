/**
 * Advent of Code 2018
 * Day 19: Go With The Flow
 * https://adventofcode.com/2018/day/19
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 19.c
 * Enable timer:
 *     cc -std=gnu17 -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 19.c
 * Get minimum runtime from timer output:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements, includes all parsing but not reading from disk:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  18 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :  29 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 104 µs
*/

#include <stdio.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2018-19-input.txt"
#define FSIZE  512  // needed for my input: 405
#define MEMSIZE 64  // needed for my input: 36
#define OPCOUNT 16  // assembly language has 16 different opcodes
#define REGCOUNT 6  // registers
#define REGINIT  0  // register index to initialise
#define REGDIVS  2  // register index of interest

typedef unsigned int uint;

typedef enum opcode {
    ADDR, ADDI, MULR, MULI, BANR, BANI, BORR, BORI,
    SETR, SETI, GTRR, GTIR, GTRI, EQRR, EQIR, EQRI
} Opcode;

typedef struct instr {
    Opcode op;
    uint a, b, c;
} Instr;

static char input[FSIZE];
static Instr mem[MEMSIZE];  // program memory
static uint ipreg;  // special register index

// Parse unsigned int, advance char pointer, skip one more
static uint parseint(const char **str)
{
    uint x = 0;
    while (**str >= '0')
        x = x * 10 + (*(*str)++ & 15);
    (*str)++;  // skip space/newline
    return x;
}

// Reverse engineered from input file: algo is to calculate sum of divisors
// Ref.: https://en.wikipedia.org/wiki/Divisor_function#Formulas_at_prime_powers
//   sigma1(n) = prod[(p^(a+1) - 1)/(p - 1)]
//   product over every prime where a is maximum p^a that divides n
//   (if p is not a divisor of n, product term is 1)
static uint sumofdivisors(uint x)
{
    // Special case: prime factor = 2
    uint pa = 2;  // p^a starting with 2^1
    while (!(x & 1)) {
        pa <<= 1;
        x >>= 1;
    }
    uint prod = pa - 1;  // (2^(a+1)-1)/(2-1)
    // Other potential prime factors
    for (uint p = 3; p <= x; p += 2) {
        pa = p;  // p^a where a=1
        while (!(x % p)) {
            pa *= p;
            x /= p;
        }
        if (pa != p)  // mathematically unnecessary test but speeds it up
            prod *= (pa - 1) / (p - 1);
    }
    return prod;
}

static uint exec(const int init)
{
    uint reg[REGCOUNT] = {0};
    reg[REGINIT] = init;
    uint ip = 0;
    do {
        reg[ipreg] = ip;  // always store in reg according to puzzle description
        const Instr *const m = &mem[ip];  // convenience pointer
        switch (m->op) {
            case ADDR: reg[m->c] = reg[m->a] +  reg[m->b]; break;
            case ADDI: reg[m->c] = reg[m->a] +      m->b ; break;
            case MULR: reg[m->c] = reg[m->a] *  reg[m->b]; break;
            case MULI: reg[m->c] = reg[m->a] *      m->b ; break;
            case BANR: reg[m->c] = reg[m->a] &  reg[m->b]; break;
            case BANI: reg[m->c] = reg[m->a] &      m->b ; break;
            case BORR: reg[m->c] = reg[m->a] |  reg[m->b]; break;
            case BORI: reg[m->c] = reg[m->a] |      m->b ; break;
            case SETR: reg[m->c] = reg[m->a]             ; break;
            case SETI: reg[m->c] =     m->a              ; break;
            case GTRR: reg[m->c] = reg[m->a] >  reg[m->b]; break;
            case GTIR: reg[m->c] =     m->a  >  reg[m->b]; break;
            case GTRI: reg[m->c] = reg[m->a] >      m->b ; break;
            case EQRR: reg[m->c] = reg[m->a] == reg[m->b]; break;
            case EQIR: reg[m->c] =     m->a  == reg[m->b]; break;
            case EQRI: reg[m->c] = reg[m->a] ==     m->b ; break;
        }
        ip = reg[ipreg] + 1;  // always +1 according to puzzle description
        // No need to check for ip bounds because jump back to start comes sooner
    } while (reg[ipreg]);  // until jump back to address 1 (0+1=1)
    return sumofdivisors(reg[REGDIVS]);  // native implementation of reverse engineered algo
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
    ipreg = parseint(&c);  // global index of special register
    const char *const end = input + fsize;
    for (int n = 0; c < end; ++n) {  // assume MEMSIZE is big enough
        Opcode op;
        switch (*(c + 1)) {  // second letter of the opcode is unique (apart from r/i)
            case 'a': op = *(c + 3) == 'r' ? BANR : BANI; break;
            case 'd': op = *(c + 3) == 'r' ? ADDR : ADDI; break;
            case 'e': op = *(c + 3) == 'r' ? SETR : SETI; break;
            case 'o': op = *(c + 3) == 'r' ? BORR : BORI; break;
            case 'q': op = *(c + 2) == 'r' ? (*(c + 3) == 'r' ? EQRR : EQRI) : EQIR; break;
            case 't': op = *(c + 2) == 'r' ? (*(c + 3) == 'r' ? GTRR : GTRI) : GTIR; break;
            case 'u': op = *(c + 3) == 'r' ? MULR : MULI; break;
            default: fprintf(stderr, "Unknown opcode on line %d\n", n + 1); return 2;
        }
        c += 5;  // skip 4-char opcode +space
        const int par1 = parseint(&c);
        const int par2 = parseint(&c);
        const int par3 = parseint(&c);
        mem[n] = (Instr){op, par1, par2, par3};
    }

    printf("%u\n", exec(0));  // part 1: 1922
    printf("%u\n", exec(1));  // part 2: 22302144

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
}
