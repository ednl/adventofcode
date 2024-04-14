#include <stdio.h>     // fopen, getline, printf
#include <stdlib.h>    // atoi, free
#include <stdint.h>    // uint64_t
#include <inttypes.h>  // PRIu64
#include "../startstoptimer.h"

#define VALIDATE 0
#define MEMSIZE 64

typedef enum {
    NOP, INC, HLF, TPL, JMP, JIE, JIO
} opc_t;

typedef enum {
    A, B, REGSIZE
} reg_t;

typedef struct {
    opc_t opc;
    reg_t reg;
    int   jmp;
} ins_t;

static ins_t mem[MEMSIZE];
static int progsize;

static int parse(const char * const s)
{
    int n = 0;
    char *line = NULL;
    size_t size;
    FILE *f = fopen(s, "r");
    if (!f)
        return 0;
    while (n < MEMSIZE && getline(&line, &size, f) > 1) {
        opc_t o = NOP;
        switch (line[2]) {
            case 'c': o = INC; break;
            case 'f': o = HLF; break;
            case 'l': o = TPL; break;
            case 'p': o = JMP; break;
            case 'e': o = JIE; break;
            case 'o': o = JIO; break;
        }
        reg_t r = line[4] == 'a' ? A : B;
        int j = 0;
        switch (o) {
            case NOP:
            case INC:
            case HLF:
            case TPL: j = 1; break;
            case JMP: j = atoi(&line[4]); break;
            case JIE:
            case JIO: j = atoi(&line[7]); break;
        }
        mem[n++] = (ins_t){o, r, j};
    }
    fclose(f);
    free(line);
    return n;
}

static uint64_t run(const unsigned int a)
{
    uint64_t reg[REGSIZE] = {a, 0};
    int ip = 0;
    while (ip < progsize) {
        ins_t    *i = &mem[ip];
        uint64_t *r = &reg[i->reg];
        int       j = i->jmp;
        switch (i->opc) {
            case INC: *r += 1; break;
            case HLF: *r /= 2; break;
            case TPL: *r *= 3; break;
            case JIE: if (*r  & 1) j = 1; break;
            case JIO: if (*r != 1) j = 1; break;
            case NOP:
            case JMP: break;
        }
        ip += j;
    }
    return reg[B];
}

#if VALIDATE
static uint64_t collatz(uint64_t a)
{
    uint64_t b = 0;
    while (a > 1) {
        ++b;
        if (a & 1)
            a = a * 3 + 1;
        else
            a >>= 1;
    }
    return b;
}
#endif

int main(void)
{
    starttimer();
    progsize = parse("../aocinput/2015-23-input.txt");
    printf("Part 1: %"PRIu64"\n", run(0));  // 255
    printf("Part 2: %"PRIu64"\n", run(1));  // 334
    printf("Time: %.0f µs\n", stoptimer_us());
#if VALIDATE
    printf("Validation: %"PRIu64, collatz((((((((0+1)*3*3*3+1)*3+1)*3+1+1)*3+1+1)*3+1+1)*3+1+1)*3));
    printf(" %"PRIu64"\n", collatz((((((1*3*3*3*3+1+1)*3+1)*3+1+1)*3+1+1)*3+1)*3*3));
#endif
    return 0;
}
