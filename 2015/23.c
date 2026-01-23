/**
 * Advent of Code 2015
 * Day 23: Opening the Turing Lock
 * https://adventofcode.com/2015/day/23
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 23.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 23.c
 * Get minimum runtime from timer output in bash:
 *     m=9999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  4.08 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :  6.42 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 15.2  µs
 */

#include <stdio.h>
#include <stdlib.h>  // atoi
#include <stdint.h>  // uint32_t, uint64_t
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define VALIDATE 0  // change to 1 for a direct Collatz computation
#define MEMSIZE 64  // needed for my input: 48
#define BUFSIZE 16  // needed for my input: 12

// JIO = jump if ONE (not odd), JIE = jump if even
typedef enum opcode {
    INCB, INC, HLF, TPL, JMP, JIO, JIE
} OpCode;

// Instructions always operate on register A, except INCB
typedef struct instr {
    OpCode op;
    int jmp;
} Instr;

static Instr mem[MEMSIZE];
static int progsize;

static int parse(const char *fname)
{
    FILE *f = fopen(fname, "r");
    if (!f)
        return 0;
    char buf[BUFSIZE];
    int n = 0;
    while (n < MEMSIZE && fgets(buf, sizeof buf, f)) {
        OpCode op;
        switch (buf[2]) {
            case 'c': op = buf[4] == 'a' ? INC : INCB; break;
            case 'e': op = JIE; break;
            case 'f': op = HLF; break;
            case 'l': op = TPL; break;
            case 'o': op = JIO; break;
            case 'p': op = JMP; break;
            default: return 0;  // illegal instruction
        }
        int jmp;
        switch (op) {
            case JMP: jmp = atoi(&buf[4]) - 1; break;  // -1 to compensate for ip++
            case JIO: /* fallthrough */
            case JIE: jmp = atoi(&buf[7]) - 1; break;
            default : jmp = 0;  // keep compiler happy because op is enum
        }
        mem[n++] = (Instr){op, jmp};
    }
    fclose(f);
    return n;
}

// For my input, the value in register A never exceeds 30 bits
static int run(uint32_t a)
{
    int b = 0;  // counter of Collatz steps
    // Assume program is well-formed and reaches 'end' exactly
    const Instr *const end = mem + progsize;
    for (const Instr *ip = mem; ip != end; ++ip)
        switch (ip->op) {
            case INCB: b++;                         break;  // increment register B
            case INC : a++;                         break;  // increment register A
            case HLF : a >>= 1;                     break;  // halve register A
            case TPL : a *= 3;                      break;  // triple register A
            case JMP : ip += ip->jmp;               break;  // unconditional jump
            case JIO : if (a == 1) ip += ip->jmp;   break;  // jump if register A is one (not odd!)
            case JIE : if (!(a & 1)) ip += ip->jmp; break;  // jump if register A is even
        }
    return b;
}

#if VALIDATE
static int collatz(uint64_t a)
{
    int b = 0;
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
    progsize = parse("../aocinput/2015-23-input.txt");

#ifdef TIMER
    starttimer();
#endif

    printf("Part 1: %d\n", run(0));  // 255
    printf("Part 2: %d\n", run(1));  // 334
#if VALIDATE
    printf("Validation: %d %d\n",
        collatz((((((((0+1)*3*3*3+1)*3+1)*3+1+1)*3+1+1)*3+1+1)*3+1+1)*3),
        collatz((((((1*3*3*3*3+1+1)*3+1)*3+1+1)*3+1+1)*3+1)*3*3));
#endif

#ifdef TIMER
    printf("Time: %.0f ns\n", stoptimer_ns());
#endif
}
