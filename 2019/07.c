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
 *     Macbook Pro 2024 (M4 4.4 GHz) :  55.6 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :  95.6 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 231.9 µs
 */

#include <stdio.h>
#include <stdint.h>  // uint16_t, int32_t
#include <string.h>  // memcpy
#include <stdbool.h>
#include "../combperm.h"  // permutations
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

// Puzzle specific constants
#define FNAME "../aocinput/2019-07-input.txt"
#define FSIZE  1254  // needed for my input: 1254 (bytes in input file)
#define VM_SIZE 523  // needed for my input: 523 (CSV count)
#define SERIES 5

// VM macros
#define POS (&vm->mem[vm->mem[vm->ip++]])  // get positional parameter address, advance instruction pointer
#define IMM (&vm->mem[vm->ip++])           // get immediate parameter address, advance instruction pointer

// VM constants
#define VM_OK   0
#define VM_ERR -1

// VM uses 32-bit int
typedef int32_t VMType;

typedef enum vmstate {
    VM_STATE_NUL = 0,  // default state, needs to be initialised (resize, fresh code, get ID)
    VM_STATE_INI,      // initialised, needs to be reset (reset IP, next state)
    VM_STATE_1ST,      // reset done (IP+state), first INP on first run will get "phase setting" from ID
    VM_STATE_RUN,      // running after getting "phase setting", or suspended after OUT
    VM_STATE_HLT,      // halted after HLT or error
} VMState;

// Day 2: "Encountering an unknown opcode means something went wrong."
typedef enum vmopcode {
    VM_OP_ADD =  1,  // add params and store
    VM_OP_MUL =  2,  // multiply params and store
    VM_OP_INP =  3,  // input & store value
    VM_OP_OUT =  4,  // output value
    VM_OP_JNZ =  5,  // if par0 != 0 then ip = par1
    VM_OP_JZ  =  6,  // if par0 == 0 then ip = par1
    VM_OP_LT  =  7,  // par2 = par0  < par1
    VM_OP_EQ  =  8,  // par2 = par0 == par1
    VM_OP_HLT = 99,  // halt program
} VMOpCode;

// Translation of parameter modes and opcode from
// decimal to binary with added info: parameter count
// MSByte = opcode
// LSByte = 3x 2-bit parmode, 1x 2-bit parcount
static const uint16_t vm_decode[] = {
    [    1] = 0x0103,  // 00000001 00 00 00 11  ADD
    [  101] = 0x0107,  // 00000001 00 00 01 11
    [ 1001] = 0x0113,  // 00000001 00 01 00 11
    [ 1101] = 0x0117,  // 00000001 00 01 01 11
    [    2] = 0x0203,  // 00000010 00 00 00 11  MUL
    [  102] = 0x0207,  // 00000010 00 00 01 11
    [ 1002] = 0x0213,  // 00000010 00 01 00 11
    [ 1102] = 0x0217,  // 00000010 00 01 01 11
    [    3] = 0x0301,  // 00000011 00 00 00 01  INP
    [    4] = 0x0401,  // 00000100 00 00 00 01  OUT
    [  104] = 0x0405,  // 00000100 00 00 01 01
    [    5] = 0x0502,  // 00000101 00 00 00 10  JNZ
    [  105] = 0x0506,  // 00000101 00 00 01 10
    [ 1005] = 0x0512,  // 00000101 00 01 00 10
    [ 1105] = 0x0516,  // 00000101 00 01 01 10
    [    6] = 0x0602,  // 00000110 00 00 00 10  JZ
    [  106] = 0x0606,  // 00000110 00 00 01 10
    [ 1006] = 0x0612,  // 00000110 00 01 00 10
    [ 1106] = 0x0616,  // 00000110 00 01 01 10
    [    7] = 0x0703,  // 00000111 00 00 00 11  LT
    [  107] = 0x0707,  // 00000111 00 00 01 11
    [ 1007] = 0x0713,  // 00000111 00 01 00 11
    [ 1107] = 0x0717,  // 00000111 00 01 01 11
    [    8] = 0x0803,  // 00001000 00 00 00 11  EQ
    [  108] = 0x0807,  // 00001000 00 00 01 11
    [ 1008] = 0x0813,  // 00001000 00 01 00 11
    [ 1108] = 0x0817,  // 00001000 00 01 01 11
    [   99] = 0x6300,  // 01100011 00 00 00 00  HLT
};

typedef struct vm {
    VMType mem[VM_SIZE];  // no dynamic size needed
    int id, ip;  // ID = "phase setting", ip = instruction pointer
    VMState state;
} VM;

static char input[FSIZE];
static VMType intcode[VM_SIZE];  // intcode program from input file CSV
static VM vm[SERIES];

// Parse one-line CSV of signed ints from input file to VM data
static int vm_parse(const char *const input, VMType *const code)
{
    int n = 0;
    for (const char *c = input; *c; ++c) {
        const VMType sgn = *c == '-' ? (c++, -1) : 1;
        VMType x = *c++ & 15;
        while (*c > ',')
            x = x * 10 + (*c++ & 15);
        code[n++] = x * sgn;
    }
    return n;
}

// Full VM initialisation
static void vm_init(VM *const vm, const VMType *const code, const int size, const int id)
{
    memcpy(vm->mem, code, size * sizeof *code);  // fresh copy of intcode program, fixed size
    vm->id = id;  // set "phase setting" (VM series index)
    // vm->ip static init = 0
    vm->state = VM_STATE_INI;  // initialisation done, ready for reset
}

// Prepare for first run: reset instruction pointer and state
// but no fresh intcode or ID
static void vm_first(VM *const vm)
{
    vm->ip = 0;
    vm->state = VM_STATE_1ST;  // reset done, ready for first run
}

// inputval: output of previous VM, or 0 for first run of first VM
static VMType vm_input(VM *const vm, const VMType inputval)
{
    // Normal INP
    if (vm->state == VM_STATE_RUN)
        return inputval;
    // First INP: give "phase setting" from ID
    vm->state = VM_STATE_RUN;  // state was 1ST (or possibly NUL/INI)
    return vm->id;  // part 1: 0-4, part 2: 5-9
}

static VMType vm_output(VM *const vm, const VMType *const outputval)
{
    if (vm->mem[vm->ip] == VM_OP_HLT)  // next instruction is HLT?
        vm->state = VM_STATE_HLT;
    return *outputval;
}

static VMType run(VM *const vm, const VMType inputval)
{
    VMType *p1 = 0, *p2 = 0, *p3 = 0;  // max 3 instruction parameters
    for (;;) {  // rely on OUT or HLT to return
        const uint16_t instr = vm_decode[vm->mem[vm->ip++]];  // avoid decimal decoding
        switch (instr & 0xff) {  // LSByte = parmode/count
            // Parameter macros:
            //   POS = &vm->mem[vm->mem[vm->ip++]]
            //   IMM = &vm->mem[vm->ip++]
            case 0x01: p1 = POS;                     break;
            case 0x02: p1 = POS; p2 = POS;           break;
            case 0x03: p1 = POS; p2 = POS; p3 = POS; break;
            case 0x05: p1 = IMM;                     break;
            case 0x06: p1 = IMM; p2 = POS;           break;
            case 0x07: p1 = IMM; p2 = POS; p3 = POS; break;
            case 0x12: p1 = POS; p2 = IMM;           break;
            case 0x13: p1 = POS; p2 = IMM; p3 = POS; break;
            case 0x16: p1 = IMM; p2 = IMM;           break;
            case 0x17: p1 = IMM; p2 = IMM; p3 = POS; break;
            default: vm->state = VM_STATE_HLT; return VM_ERR;  // unknown parmode/count
        }
        switch (instr >> 8) {  // MSByte = opcode
            case VM_OP_ADD: *p3 = *p1 + *p2; break;
            case VM_OP_MUL: *p3 = *p1 * *p2; break;
            case VM_OP_INP: *p1 = vm_input(vm, inputval); break;  // at first INP: state from OK to RUN
            case VM_OP_OUT: return vm_output(vm, p1);  // state = HLT if next instruction is HLT
            case VM_OP_JNZ: if (*p1 != 0) vm->ip = *p2; break;
            case VM_OP_JZ : if (*p1 == 0) vm->ip = *p2; break;
            case VM_OP_LT : *p3 = *p1  < *p2; break;
            case VM_OP_EQ : *p3 = *p1 == *p2; break;
            case VM_OP_HLT: vm->state = VM_STATE_HLT; return VM_OK;
            default       : vm->state = VM_STATE_HLT; return VM_ERR;  // unknown opcode
        }
    }
}

// Run 5 VMs in series until the last one halts
// Part 1: phaseoffset=0 => phase settings 0-4, one series run
// Part 2: phaseoffset=5 => phase settings 5-9, series run until HLT
static VMType series(const int phaseoffset)
{
    VMType max = 0;
    // Full init to start
    for (int i = 0; i < SERIES; ++i)
        vm_init(&vm[i], intcode, VM_SIZE, i + phaseoffset);
    // SERIES=5 => 120 permutations, phase[] = index 0-4 shuffled
    for (int *phase; (phase = permutations(SERIES)); ) {
        // Simple reset (IP+state) for each new permutation
        for (int i = 0; i < SERIES; ++i)
            vm_first(&vm[i]);
        // When one VM halts, all should be halted
        // my input: 10 loops for every permutation
        for (VMType out = 0; vm[0].state != VM_STATE_HLT; ) {
            // One series of 5 is a complete run, last output is new thruster setting
            for (int i = 0; i < SERIES; ++i)
                out = run(&vm[phase[i]], out);
            if (out > max)
                max = out;
        }
    }
    return max;
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
#endif

    // Parse intcode CSV data
    // discard return value, should be VM_SIZE
    vm_parse(input, intcode);

    printf("%d\n", series(0));       // part 1: 929800
    printf("%d\n", series(SERIES));  // part 2: 15432220

#ifdef TIMER
    }
    fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());
#endif
}
