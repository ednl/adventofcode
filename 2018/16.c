/**
 * Advent of Code 2018
 * Day 16: Chronal Classification
 * https://adventofcode.com/2018/day/16
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 16.c
 * Enable timer:
 *     cc -std=gnu17 -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 16.c
 * Get minimum runtime from timer output:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 208 µs (includes reading & parsing input file)
 *     Mac Mini 2020 (M1 3.2 GHz)    :   ? µs
 *     Raspberry Pi 5 (2.4 GHz)      :   ? µs
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../startstoptimer.h"

#define LANGSIZE (16U)  // there are 16 opcodes and instructions
#define REGCOUNT  (4U)  // there are 4 registers

// Instruction mnemonics
typedef enum assembly {
    ADDR, ADDI,
    MULR, MULI,
    BANR, BANI,
    BORR, BORI,
    SETR, SETI,
    GTRR, GTIR, GTRI,
    EQRR, EQIR, EQRI
} Assembly;

static const char *fname = "../aocinput/2018-16-input.txt";  // input file name
static unsigned int reg[REGCOUNT];             // registers
static bool opcodematrix[LANGSIZE][LANGSIZE];  // possible opcode-instruction pairings

// Execute an instruction with params a,b acting on global 'reg'
// return result, do not write to reg
static unsigned int exec(Assembly instr, unsigned int a, unsigned int b)
{
    switch (instr) {
        case ADDR: return reg[a] + reg[b];
        case ADDI: return reg[a] + b;
        case MULR: return reg[a] * reg[b];
        case MULI: return reg[a] * b;
        case BANR: return reg[a] & reg[b];
        case BANI: return reg[a] & b;
        case BORR: return reg[a] | reg[b];
        case BORI: return reg[a] | b;
        case SETR: return reg[a];
        case SETI: return a;
        case GTRR: return reg[a] > reg[b];
        case GTIR: return a > reg[b];
        case GTRI: return reg[a] > b;
        case EQRR: return reg[a] == reg[b];
        case EQIR: return a == reg[b];
        case EQRI: return reg[a] == b;
    }
    return 0;
}

int main(void)
{
    FILE *fp = fopen(fname, "r");
    if (!fp) return 1;

    starttimer();

    unsigned int threeormore = 0;
    unsigned int op, a, b, c, after[REGCOUNT];
    // Read register state, opcode + parameters, register state after
    while (fscanf(fp,
        "Before: [%u, %u, %u, %u] %u %u %u %u After: [%u, %u, %u, %u] ",
        &reg[0], &reg[1], &reg[2], &reg[3],
        &op, &a, &b, &c,
        &after[0], &after[1], &after[2], &after[3]) == 12) {
        unsigned int possible = 0;
        // Try every instruction for this register state and opcode
        for (Assembly in = 0; in < LANGSIZE; ++in)
            // Check if the result conforms to the register state after
            possible += (opcodematrix[op][in] = exec(in, a, b) == after[c]);
        // Count opcodes that can mean 3 or more different instructions
        threeormore += possible >= 3;
    }
    printf("Part 1: %u\n", threeormore);  // 570

    bool changed = true;
    // Reduce the possible opcode-instruction translations until no more change
    while (changed) {
        changed = false;
        for (Assembly op = 0; op < LANGSIZE; ++op) {
            unsigned int match = 0, lastmatch = 0;
            // Count the possible translations (horizontally) and remember the last one
            for (Assembly instr = 0; instr < LANGSIZE; ++instr)
                if (opcodematrix[op][instr]) {
                    ++match;
                    lastmatch = instr;
                }
            // If there was only one possibility in this row, eliminate the others in that column
            if (match == 1)
                for (Assembly op2 = 0; op2 < LANGSIZE; ++op2)
                    if (op2 != op && opcodematrix[op2][lastmatch]) {
                        opcodematrix[op2][lastmatch] = false;
                        changed = true;  // there was still something to be eliminiated
                    }
        }
    }

    // Condense the matrix of possible translations to a definite opcode-to-instruction LUT
    Assembly instr[LANGSIZE] = {0};
    for (Assembly op = 0; op < LANGSIZE; ++op)
        for (Assembly in = 0; in < LANGSIZE; ++in)
            if (opcodematrix[op][in]) {
                instr[op] = in;
                break;
            }

    // Reset the registers to zero
    for (unsigned int i = 0; i < REGCOUNT; ++i)
        reg[i] = 0;
    // Execute the program
    {
        unsigned int op, a, b, c;
        while (fscanf(fp, "%u %u %u %u ", &op, &a, &b, &c) == 4)
            reg[c] = exec(instr[op], a, b);
    }
    // What's in register 0?
    printf("Part 2: %u\n", reg[0]);  // 503
    printf("Time: %.0f ms\n", stoptimer_us());
    fclose(fp);
}
