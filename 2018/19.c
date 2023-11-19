#include <stdio.h>
#include <string.h>  // strcmp
#include <stdint.h>

#define SHOW 0
#define NAME "../aocinput/2018-19-input.txt"
#define MEMSIZE 64
#define REGCOUNT 6

typedef enum _opcode_t {
    NOP,
    ADDR, ADDI, MULR, MULI, BANR, BANI, BORR, BORI,
    SETR, SETI, GTRR, GTIR, GTRI, EQRR, EQIR, EQRI,
    OPCOUNT
} opcode_t;

static const char opname[OPCOUNT][5] = {
    "nop",
    "addr", "addi", "mulr", "muli", "banr", "bani", "borr", "bori",
    "setr", "seti", "gtrr", "gtir", "gtri", "eqrr", "eqir", "eqri"
};

typedef struct _instr_t {
    opcode_t op;
    int a, b, c;
} instr_t;

static instr_t mem[MEMSIZE];
static int64_t reg[REGCOUNT];
static int progsize, ip, ipreg;

static int read(void)
{
    FILE *f = fopen(NAME, "r");
    if (!f)
        return 0;
    fscanf(f, "#ip %d ", &ipreg);
    char s[5];
    int n = 0, a, b, c;
    while (n < MEMSIZE && fscanf(f, "%4s %d %d %d ", s, &a, &b, &c) == 4) {
        opcode_t op = NOP;
        for (int i = 1; i < OPCOUNT; ++i)
            if (!strcmp(s, opname[i])) {
                op = (opcode_t)i;
                break;
            }
        mem[n++] = (instr_t){op, a, b, c};
    }
    fclose(f);
    return n;
}

static void exec(void)
{
#if SHOW
    printf("%2d:  %4s %d %2d %2d  ", ip, opname[mem[ip].op], mem[ip].a, mem[ip].b, mem[ip].c);
#endif
    reg[ipreg] = ip;
    instr_t * const i = &mem[ip];
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
    }
    ip = (int)reg[ipreg] + 1;
#if SHOW
    printf("[%lld,%lld,%8lld,%8lld,%2lld,%lld] => %2d\n", reg[0], reg[1], reg[2], reg[3], reg[4], reg[5], ip);
#endif
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
    if (!(progsize = read())) {
        fputs("File not found.", stderr);
        return 1;
    }

    // Part 1
    do {
        exec();
    } while (reg[ipreg]);
    printf("Part 1: %lld\n", sumofdivisors(reg[2]));  // part 1: 1922

    // Part 2
    ip = 0;
    reg[0] = 1;
    for (int i = 1; i < REGCOUNT; ++i)
        reg[i] = 0;
    do {
        exec();
    } while (reg[ipreg]);
    printf("Part 2: %lld\n", sumofdivisors(reg[2]));  // part 2: 22302144

    return 0;
}
