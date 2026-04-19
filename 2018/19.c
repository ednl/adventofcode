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
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 18 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :  ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : 95 µs
*/

#include <stdio.h>
#include <string.h>  // memset
#include <stdint.h>  // int64_t
#include <inttypes.h>  // PRId64
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2018-19-input.txt"
#define FSIZE  512  // needed for my input: 405
#define MEMSIZE 64  // needed for my input: 36
#define REGCOUNT 6

typedef enum opcode {
    ADDR, ADDI, MULR, MULI, BANR, BANI, BORR, BORI,
    SETR, SETI, GTRR, GTIR, GTRI, EQRR, EQIR, EQRI,
    OPCOUNT
} Opcode;

typedef struct instr {
    Opcode op;
    int a, b, c;
} Instr;

static char input[FSIZE];
static Instr mem[MEMSIZE];
static int64_t reg[REGCOUNT];
static int ip, ipreg;

static int parseint(const char **str)
{
    int x = 0;
    while (**str >= '0')
        x = x * 10 + (*(*str)++ & 15);
    (*str)++;  // skip space/newline
    return x;
}

static void exec(void)
{
    reg[ipreg] = ip;
    const Instr *const i = &mem[ip];
    switch (i->op) {
        case ADDR: reg[i->c] = reg[i->a] +  reg[i->b]; break;
        case ADDI: reg[i->c] = reg[i->a] +      i->b ; break;
        case MULR: reg[i->c] = reg[i->a] *  reg[i->b]; break;
        case MULI: reg[i->c] = reg[i->a] *      i->b ; break;
        case BANR: reg[i->c] = reg[i->a] &  reg[i->b]; break;
        case BANI: reg[i->c] = reg[i->a] &      i->b ; break;
        case BORR: reg[i->c] = reg[i->a] |  reg[i->b]; break;
        case BORI: reg[i->c] = reg[i->a] |      i->b ; break;
        case SETR: reg[i->c] = reg[i->a]             ; break;
        case SETI: reg[i->c] =     i->a              ; break;
        case GTRR: reg[i->c] = reg[i->a] >  reg[i->b]; break;
        case GTIR: reg[i->c] =     i->a  >  reg[i->b]; break;
        case GTRI: reg[i->c] = reg[i->a] >      i->b ; break;
        case EQRR: reg[i->c] = reg[i->a] == reg[i->b]; break;
        case EQIR: reg[i->c] =     i->a  == reg[i->b]; break;
        case EQRI: reg[i->c] = reg[i->a] ==     i->b ; break;
        default: break;
    }
    ip = reg[ipreg] + 1;
}

// Ref.: https://en.wikipedia.org/wiki/Divisor_function#Formulas_at_prime_powers
static int64_t sumofdivisors(int64_t x)
{
    // Special case: prime factor = 2
    int64_t pp = 2;
    while (!(x & 1)) {
        pp <<= 1;
        x >>= 1;
    }
    // Other potential prime factors
    int64_t p = 3, prod = pp - 1;
    while (p <= x) {
        pp = p;
        while (!(x % p)) {
            pp *= p;
            x /= p;
        }
        if (pp != p)
            prod *= (pp - 1) / (p - 1);
        p += 2;
    }
    return prod;
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

    // Parse
    const char *ch = input + 4;  // skip "#ip "
    ipreg = parseint(&ch);  // global
    const char *const end = input + fsize;
    for (int n = 0; ch < end; ++n) {  // asumes MEMSIZE is big enough
        Opcode op;
        switch (*(ch + 1)) {  // second letter of the opcode is unique (apart from r/i)
            case 'a': op = *(ch + 3) == 'r' ? BANR : BANI; break;
            case 'd': op = *(ch + 3) == 'r' ? ADDR : ADDI; break;
            case 'e': op = *(ch + 3) == 'r' ? SETR : SETI; break;
            case 'o': op = *(ch + 3) == 'r' ? BORR : BORI; break;
            case 'q': op = *(ch + 2) == 'r' ? (*(ch + 3) == 'r' ? EQRR : EQRI) : EQIR; break;
            case 't': op = *(ch + 2) == 'r' ? (*(ch + 3) == 'r' ? GTRR : GTRI) : GTIR; break;
            case 'u': op = *(ch + 3) == 'r' ? MULR : MULI; break;
            default: fprintf(stderr, "Unknown opcode on line %d\n", n + 1); return 2;
        }
        ch += 5;  // skip 4-char opcode +space
        const int a = parseint(&ch);
        const int b = parseint(&ch);
        const int c = parseint(&ch);
        mem[n] = (Instr){op, a, b, c};
    }

    // Part 1
    do {
        exec();
    } while (reg[ipreg]);
    printf("%"PRId64"\n", sumofdivisors(reg[2]));  // part 1: 1922

    // Part 2
    ip = 0;
    memset(reg, 0, sizeof reg);
    reg[0] = 1;
    do {
        exec();
    } while (reg[ipreg]);
    printf("%"PRId64"\n", sumofdivisors(reg[2]));  // part 2: 22302144

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
}
