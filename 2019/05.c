/**
 * Advent of Code 2019
 * Day 5: Sunny with a Chance of Asteroids
 * https://adventofcode.com/2019/day/5
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 05.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 05.c
 * Test output with timer enabled:
 *     ./a.out | tail -n2
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 1.11 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 1.82 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 4.26 µs
 */

#include <stdio.h>
#include <string.h>  // memcpy
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2019-05-input.txt"
#define FSIZE 2551  // needed for my input: 2551
#define VMSIZE 678  // needed for my input: 678
#define MAXPARAM 3

typedef enum opcode {
    NOP =  0,  // no operation
    ADD =  1,  // add params and store
    MUL =  2,  // multiply params and store
    INP =  3,  // input & store value
    OUT =  4,  // output value
    JNZ =  5,  // if par0 != 0 then ip = par1
    JZ  =  6,  // if par0 == 0 then ip = par1
    LT  =  7,  // par2 = par0  < par1
    EQ  =  8,  // par2 = par0 == par1
    HLT = 99,  // halt program
} OpCode;

typedef enum parmode {
    ERR = 0,  // unused par mode
    POS = 1,  // [intcode: 0] positional (=indirect, by reference)
    IMM = 2,  // [intcode: 1] immediate (=direct, by value)
} ParMode;

typedef struct instr {
    char opcode;
    char parmode[MAXPARAM];
} Instr;

// Unspecified entries are {0,{0,0,0}} = {NOP,{ERR,ERR,ERR}}
static const Instr decode[] = {
    [    1] = {ADD, {POS,POS,POS}},
    [  101] = {ADD, {IMM,POS,POS}},
    [ 1001] = {ADD, {POS,IMM,POS}},
    [ 1101] = {ADD, {IMM,IMM,POS}},
    [    2] = {MUL, {POS,POS,POS}},
    [  102] = {MUL, {IMM,POS,POS}},
    [ 1002] = {MUL, {POS,IMM,POS}},
    [ 1102] = {MUL, {IMM,IMM,POS}},
    [    3] = {INP, {POS        }},
    [    4] = {OUT, {POS        }},
    [  104] = {OUT, {IMM        }},
    [    5] = {JNZ, {POS,POS    }},
    [  105] = {JNZ, {IMM,POS    }},
    [ 1005] = {JNZ, {POS,IMM    }},
    [ 1105] = {JNZ, {IMM,IMM    }},
    [    6] = {JZ , {POS,POS    }},
    [  106] = {JZ , {IMM,POS    }},
    [ 1006] = {JZ , {POS,IMM    }},
    [ 1106] = {JZ , {IMM,IMM    }},
    [    7] = {LT , {POS,POS,POS}},
    [  107] = {LT , {IMM,POS,POS}},
    [ 1007] = {LT , {POS,IMM,POS}},
    [ 1107] = {LT , {IMM,IMM,POS}},
    [    8] = {EQ , {POS,POS,POS}},
    [  108] = {EQ , {IMM,POS,POS}},
    [ 1008] = {EQ , {POS,IMM,POS}},
    [ 1108] = {EQ , {IMM,IMM,POS}},
    [   99] = {HLT, {ERR        }},
};

static char input[FSIZE];
static int app[VMSIZE];
static int mem[VMSIZE];
static int len;

static void run(const int inp)
{
    int *p[3] = {0};
    memcpy(mem, app, len * sizeof *app);
    for (int ip = 0;; ) {
        const Instr *const instr = &decode[mem[ip++]];
        for (int i = 0; i < MAXPARAM; ++i)
            switch (instr->parmode[i]) {
                case ERR: goto done;
                case POS: p[i] = &mem[mem[ip++]]; break;
                case IMM: p[i] = &mem[ip++];         break;
            }
    done:
        switch (instr->opcode) {
            case ADD: *p[2] = *p[0] + *p[1]; break;
            case MUL: *p[2] = *p[0] * *p[1]; break;
            case INP: *p[0] = inp; break;
            case OUT: if (*p[0] != 0) printf("%d\n", *p[0]); break;
            case JNZ: if (*p[0] != 0) ip = *p[1]; break;
            case JZ : if (*p[0] == 0) ip = *p[1]; break;
            case LT : *p[2] = *p[0]  < *p[1]; break;
            case EQ : *p[2] = *p[0] == *p[1]; break;
            case HLT: return;
        }
    }
}

int main(void)
{
    // Read from disk
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    const int fsize = fread(input, 1, FSIZE, f);
    input[fsize - 1] = '\0';  // remove newline
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
    len = 0;
#endif

    // Parse CSV signed ints to vm.app
    for (const char *c = input; *c; ++c) {
        const int sgn = *c == '-' ? (c++, -1) : 1;
        int x = *c++ & 15;
        while (*c > ',')
            x = x * 10 + (*c++ & 15);
        app[len++] = x * sgn;
    }

    run(1);  // part 1: 13547311
    run(5);  // part 2: 236453

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());
#endif
}
