#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

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

static const char *fname = "../aocinput/2018-16-input.txt";                // input file name
static unsigned int reg[REGCOUNT] = {0};                 // registers
static bool opcodematrix[LANGSIZE][LANGSIZE] = {false};  // possible opcode-instruction pairings

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
    unsigned int threeormore = 0;
    FILE *fp = fopen(fname, "r");
    if (fp) {
        unsigned int op, a, b, c, after[REGCOUNT];
        // Read register state, opcode + parameters, register state after
        while (fscanf(fp,
            "Before: [%u, %u, %u, %u] %u %u %u %u After: [%u, %u, %u, %u] ",
            &reg[0], &reg[1], &reg[2], &reg[3],
            &op, &a, &b, &c,
            &after[0], &after[1], &after[2], &after[3]) == 12) {
            unsigned int possible = 0;
            // Try every instruction for this register state and opcode
            for (Assembly in = 0; in < LANGSIZE; ++in) {
                // Check if the result conforms to the register state after
                if (exec(in, a, b) == after[c]) {
                    ++possible;
                    opcodematrix[op][in] = true;
                }
            }
            // Count opcodes that can mean 3 or more different instructions
            if (possible >= 3) {
                ++threeormore;
            }
        }
        fclose(fp);
    }
    printf("Part 1: %u\n", threeormore);

    bool changed = true;
    // Reduce the possible opcode-instruction translations until no more change
    while (changed) {
        changed = false;
        for (Assembly op = 0; op < LANGSIZE; ++op) {
            unsigned int match = 0, lastmatch = 0;
            // Count the possible translations (horizontally) and remember the last one
            for (Assembly instr = 0; instr < LANGSIZE; ++instr) {
                if (opcodematrix[op][instr]) {
                    ++match;
                    lastmatch = instr;
                }
            }
            // If there was only one possibility in this row, eliminate the others in that column
            if (match == 1) {
                for (Assembly op2 = 0; op2 < LANGSIZE; ++op2) {
                    if (op2 != op && opcodematrix[op2][lastmatch]) {
                        opcodematrix[op2][lastmatch] = false;
                        changed = true;  // there was still something to be eliminiated
                    }
                }
            }
        }
    }

    // Condense the matrix of possible translations to a definite opcode-to-instruction LUT
    Assembly instr[LANGSIZE] = {0};
    for (Assembly op = 0; op < LANGSIZE; ++op) {
        for (Assembly in = 0; in < LANGSIZE; ++in) {
            if (opcodematrix[op][in]) {
                instr[op] = in;
                break;
            }
        }
    }

    // Reset the registers to zero
    for (unsigned int i = 0; i < REGCOUNT; ++i) {
        reg[i] = 0;
    }
    fp = fopen(fname, "r");
    if (fp) {
        // Look for three consecutive blank lines
        ssize_t n0 = 2, n1 = 2, n2 = 2;
        char *buf = NULL;
        size_t buflen = 0;
        while (n0 > 1 || n1 > 1 || n2 > 1) {
            n0 = n1;
            n1 = n2;
            n2 = getline(&buf, &buflen, fp);
        }
        free(buf);

        // Execute the program
        unsigned int op, a, b, c;
        while (fscanf(fp, "%u %u %u %u ", &op, &a, &b, &c) == 4) {
            reg[c] = exec(instr[op], a, b);
        }
        fclose(fp);
    }
    // What's in register 0?
    printf("Part 2: %u\n", reg[0]);
    return 0;
}
