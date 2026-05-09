/**
 * Advent of Code 2019
 * Day 7: Amplification Circuit
 * https://adventofcode.com/2019/day/7
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic ../combperm.c 07.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c ../combperm.c 07.c
 * Test output with timer enabled:
 *     ./a.out | tail -n2
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 10.7 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :  ?   µs
 *     Raspberry Pi 5 (2.4 GHz)      :  ?   µs
 */

#include <stdio.h>
#include <stdint.h>       // uint16_t, uint8_t
#include <string.h>       // memcpy
#include "../combperm.h"  // permutations
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2019-07-input.txt"
#define FSIZE 1254  // needed for my input: 1254
#define VMSIZE 523  // needed for my input: 523
#define PARAMS 3
#define SERIES 5

typedef enum opcode {
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

// LSB = opcode, MSB = 3x 2-bit parmodes, 1x 2-bit parcount
static const uint16_t decode[] = {
    [    1] = 0x0301,  // 00 00 00 11 00000001  ADD
    [  101] = 0x0701,  // 00 00 01 11 00000001
    [ 1001] = 0x1301,  // 00 01 00 11 00000001
    [ 1101] = 0x1701,  // 00 01 01 11 00000001
    [    2] = 0x0302,  // 00 00 00 11 00000010  MUL
    [  102] = 0x0702,  // 00 00 01 11 00000010
    [ 1002] = 0x1302,  // 00 01 00 11 00000010
    [ 1102] = 0x1702,  // 00 01 01 11 00000010
    [    3] = 0x0103,  // 00 00 00 01 00000011  INP
    [    4] = 0x0104,  // 00 00 00 01 00000100  OUT
    [  104] = 0x0504,  // 00 00 01 01 00000100
    [    5] = 0x0205,  // 00 00 00 10 00000101  JNZ
    [  105] = 0x0605,  // 00 00 01 10 00000101
    [ 1005] = 0x1205,  // 00 01 00 10 00000101
    [ 1105] = 0x1605,  // 00 01 01 10 00000101
    [    6] = 0x0206,  // 00 00 00 10 00000110  JZ
    [  106] = 0x0606,  // 00 00 01 10 00000110
    [ 1006] = 0x1206,  // 00 01 00 10 00000110
    [ 1106] = 0x1606,  // 00 01 01 10 00000110
    [    7] = 0x0307,  // 00 00 00 11 00000111  LT
    [  107] = 0x0707,  // 00 00 01 11 00000111
    [ 1007] = 0x1307,  // 00 01 00 11 00000111
    [ 1107] = 0x1707,  // 00 01 01 11 00000111
    [    8] = 0x0308,  // 00 00 00 11 00001000  EQ
    [  108] = 0x0708,  // 00 00 01 11 00001000
    [ 1008] = 0x1308,  // 00 01 00 11 00001000
    [ 1108] = 0x1708,  // 00 01 01 11 00001000
    [   99] = 0x0063,  // 00 00 00 00 01100011  HLT
};

static char input[FSIZE];
static int dat[VMSIZE];
static int mem[SERIES][VMSIZE];
static int len;

static int run(const int id, const int phase, const int inp)
{
    int *p[3] = {0};
    int inpcount = 0;
    int *const m = &mem[id][0];
    for (int ip = 0;; ) {
        const uint16_t instr = decode[m[ip++]];
        switch (instr >> 8) {  // MSByte = parmodes/parcount
            case 0x01: p[0] = &m[m[ip++]]; break;
            case 0x02: p[0] = &m[m[ip++]]; p[1] = &m[m[ip++]]; break;
            case 0x03: p[0] = &m[m[ip++]]; p[1] = &m[m[ip++]]; p[2] = &m[m[ip++]]; break;
            case 0x05: p[0] = &m[ip++]; break;
            case 0x06: p[0] = &m[ip++]; p[1] = &m[m[ip++]]; break;
            case 0x07: p[0] = &m[ip++]; p[1] = &m[m[ip++]]; p[2] = &m[m[ip++]]; break;
            case 0x12: p[0] = &m[m[ip++]]; p[1] = &m[ip++]; break;
            case 0x13: p[0] = &m[m[ip++]]; p[1] = &m[ip++]; p[2] = &m[m[ip++]]; break;
            case 0x16: p[0] = &m[ip++]; p[1] = &m[ip++]; break;
            case 0x17: p[0] = &m[ip++]; p[1] = &m[ip++]; p[2] = &m[m[ip++]]; break;
        }
        switch (instr & 0xff) {  // LSByte = opcode
            case ADD: *p[2] = *p[0] + *p[1]; break;
            case MUL: *p[2] = *p[0] * *p[1]; break;
            case INP: *p[0] = inpcount++ ? inp : phase; break;
            case OUT: return *p[0];
            case JNZ: if (*p[0] != 0) ip = *p[1]; break;
            case JZ : if (*p[0] == 0) ip = *p[1]; break;
            case LT : *p[2] = *p[0]  < *p[1]; break;
            case EQ : *p[2] = *p[0] == *p[1]; break;
            case HLT: return -1;
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
        dat[len++] = x * sgn;
    }

    // Init each VM once
    for (int i = 0; i < SERIES; ++i)
        memcpy(mem[i], dat, len * sizeof *dat);

    // Part 1: run in series once
    int max = 0;
    for (int *phase; (phase = permutations(SERIES)); ) {
        int out = 0;
        for (int i = 0; i < SERIES; ++i)
            out = run(i, phase[i], out);
        if (out > max)
            max = out;
    }
    printf("%d\n", max);  // 929800

    // Part 2: run with feedback loop
    const int inp[SERIES] = {5,6,7,8,9};
    printf("%d\n", max);  // 15432220

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());
#endif
}
