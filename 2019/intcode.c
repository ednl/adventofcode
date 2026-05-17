/**
 * Advent of Code 2019
 * Intcode computer
 * https://adventofcode.com/2019
 * By: E. Dronkert https://github.com/ednl
 */

#include <stdio.h>
#include <stdlib.h>  // calloc, realloc, free
#include <stdint.h>  // uint16_t, int64_t
#include <string.h>  // memset, memcpy
#include <stdbool.h>
#include "intcode.h"

enum vmstate {
    VM_STATE_INI = 0,  // default state, needs to be initialised (calloc, fresh code)
    VM_STATE_OK  = 1,  // ready to run
    VM_STATE_RUN = 2,  // running
    VM_STATE_SUS = 3,  // suspended, waiting for INP queue to fill or OUT queue to clear
    VM_STATE_HLT = 4,  // halted after HLT
    VM_STATE_ERR = 5,  // halted after error
};

enum vmopcode {
    VM_OP_ADD =  1,  // add params and store
    VM_OP_MUL =  2,  // multiply params and store
    VM_OP_INP =  3,  // input & store value
    VM_OP_OUT =  4,  // output value
    VM_OP_JNZ =  5,  // if par0 != 0 then ip = par1
    VM_OP_JZ  =  6,  // if par0 == 0 then ip = par1
    VM_OP_LT  =  7,  // par2 = par0  < par1
    VM_OP_EQ  =  8,  // par2 = par0 == par1
    VM_OP_RBO =  9,  // relative base offset: adjust base
    VM_OP_HLT = 99,  // halt program
};

enum vmparam {
    VM_PAR_POS = 0,
    VM_PAR_IMM = 1,
    VM_PAR_REL = 2,
    VM_PAR_SIZE
};

struct vmqueue {
    VMType q[VM_QSIZE];  // fixed size for now
    int cap, len, pop, ins;
};

struct vm {
    VMType *mem;
    int ip, base, size, id;
    VMState state;
    VMQueue inp;
    VMQueue out;
};

// Opcodes can have read and/or write parameters, up to 3 in total
// Read parameter can be positional (0), immediate (1), or relative (2)
// Write parameter can be positional (0) or relative (2)
const uint16_t vm_decode[] = {
    [    1] = 0x0103,  // 00000001 0000 0011  ADD: 2x read, 1x write
    [  101] = 0x0107,  // 00000001 0000 0111
    [  201] = 0x010B,  // 00000001 0000 1011
    [ 1001] = 0x0113,  // 00000001 0001 0011
    [ 1101] = 0x0117,  // 00000001 0001 0111
    [ 1201] = 0x011B,  // 00000001 0001 1011
    [ 2001] = 0x0123,  // 00000001 0010 0011
    [ 2101] = 0x0127,  // 00000001 0010 0111
    [ 2201] = 0x012B,  // 00000001 0010 1011
    [20001] = 0x0183,  // 00000001 1000 0011
    [20101] = 0x0187,  // 00000001 1000 0111
    [20201] = 0x018B,  // 00000001 1000 1011
    [21001] = 0x0193,  // 00000001 1001 0011
    [21101] = 0x0197,  // 00000001 1001 0111
    [21201] = 0x019B,  // 00000001 1001 1011
    [22001] = 0x01A3,  // 00000001 1010 0011
    [22101] = 0x01A7,  // 00000001 1010 0111
    [22201] = 0x01AB,  // 00000001 1010 1011
    [    2] = 0x0203,  // 00000010 0000 0011  MUL: 2x read, 1x write
    [  102] = 0x0207,  // 00000010 0000 0111
    [  202] = 0x020B,  // 00000010 0000 1011
    [ 1002] = 0x0213,  // 00000010 0001 0011
    [ 1102] = 0x0217,  // 00000010 0001 0111
    [ 1202] = 0x021B,  // 00000010 0001 1011
    [ 2002] = 0x0223,  // 00000010 0010 0011
    [ 2102] = 0x0227,  // 00000010 0010 0111
    [ 2202] = 0x022B,  // 00000010 0010 1011
    [20002] = 0x0283,  // 00000010 1000 0011
    [20102] = 0x0287,  // 00000010 1000 0111
    [20202] = 0x028B,  // 00000010 1000 1011
    [21002] = 0x0293,  // 00000010 1001 0011
    [21102] = 0x0297,  // 00000010 1001 0111
    [21202] = 0x029B,  // 00000010 1001 1011
    [22002] = 0x02A3,  // 00000010 1010 0011
    [22102] = 0x02A7,  // 00000010 1010 0111
    [22202] = 0x02AB,  // 00000010 1010 1011
    [    3] = 0x0301,  // 00000011 0000 0001  INP: 1x write
    [  203] = 0x0309,  // 00000011 0000 1001
    [    4] = 0x0401,  // 00000100 0000 0001  OUT: 1x read
    [  104] = 0x0405,  // 00000100 0000 0101
    [  204] = 0x0409,  // 00000100 0000 1001
    [    5] = 0x0502,  // 00000101 0000 0010  JNZ: 2x read
    [  105] = 0x0506,  // 00000101 0000 0110
    [  205] = 0x050A,  // 00000101 0000 1010
    [ 1005] = 0x0512,  // 00000101 0001 0010
    [ 1105] = 0x0516,  // 00000101 0001 0110
    [ 1205] = 0x051A,  // 00000101 0001 1010
    [ 2005] = 0x0522,  // 00000101 0010 0010
    [ 2105] = 0x0526,  // 00000101 0010 0110
    [ 2205] = 0x052A,  // 00000101 0010 1010
    [    6] = 0x0602,  // 00000110 0000 0010  JZ: 2x read
    [  106] = 0x0606,  // 00000110 0000 0110
    [  206] = 0x060A,  // 00000110 0000 1010
    [ 1006] = 0x0612,  // 00000110 0001 0010
    [ 1106] = 0x0616,  // 00000110 0001 0110
    [ 1206] = 0x061A,  // 00000110 0001 1010
    [ 2006] = 0x0622,  // 00000110 0010 0010
    [ 2106] = 0x0626,  // 00000110 0010 0110
    [ 2206] = 0x062A,  // 00000110 0010 1010
    [    7] = 0x0703,  // 00000111 0000 0011  LT: 2x read, 1x write
    [  107] = 0x0707,  // 00000111 0000 0111
    [  207] = 0x070B,  // 00000111 0000 1011
    [ 1007] = 0x0713,  // 00000111 0001 0011
    [ 1107] = 0x0717,  // 00000111 0001 0111
    [ 1207] = 0x071B,  // 00000111 0001 1011
    [ 2007] = 0x0723,  // 00000111 0010 0011
    [ 2107] = 0x0727,  // 00000111 0010 0111
    [ 2207] = 0x072B,  // 00000111 0010 1011
    [20007] = 0x0783,  // 00000111 1000 0011
    [20107] = 0x0787,  // 00000111 1000 0111
    [20207] = 0x078B,  // 00000111 1000 1011
    [21007] = 0x0793,  // 00000111 1001 0011
    [21107] = 0x0797,  // 00000111 1001 0111
    [21207] = 0x079B,  // 00000111 1001 1011
    [22007] = 0x07A3,  // 00000111 1010 0011
    [22107] = 0x07A7,  // 00000111 1010 0111
    [22207] = 0x07AB,  // 00000111 1010 1011
    [    8] = 0x0803,  // 00001000 0000 0011  EQ: 2x read, 1x write
    [  108] = 0x0807,  // 00001000 0000 0111
    [  208] = 0x080B,  // 00001000 0000 1011
    [ 1008] = 0x0813,  // 00001000 0001 0011
    [ 1108] = 0x0817,  // 00001000 0001 0111
    [ 1208] = 0x081B,  // 00001000 0001 1011
    [ 2008] = 0x0823,  // 00001000 0010 0011
    [ 2108] = 0x0827,  // 00001000 0010 0111
    [ 2208] = 0x082B,  // 00001000 0010 1011
    [20008] = 0x0883,  // 00001000 1000 0011
    [20108] = 0x0887,  // 00001000 1000 0111
    [20208] = 0x088B,  // 00001000 1000 1011
    [21008] = 0x0893,  // 00001000 1001 0011
    [21108] = 0x0897,  // 00001000 1001 0111
    [21208] = 0x089B,  // 00001000 1001 1011
    [22008] = 0x08A3,  // 00001000 1010 0011
    [22108] = 0x08A7,  // 00001000 1010 0111
    [22208] = 0x08AB,  // 00001000 1010 1011
    [    9] = 0x0901,  // 00001001 0000 0001  RBO: 1x read
    [  109] = 0x0905,  // 00001001 0000 0101
    [  209] = 0x0909,  // 00001001 0000 1001
    [   99] = 0x6300,  // 01100011 0000 0000  HLT: no parameters
};

// Parse one-line CSV of signed ints from input file to VM data
int vm_parse(const char *const input, VMType *const code)
{
    int n = 0;
    for (const char *c = input; *c; ++c) {
        const VMType sgn = *c == '-' ? (c++, -1) : 1;
        VMType x = 0;
        while (*c >= '0' && *c <= '9')
            x = x * 10 + (*c++ & 15);
        code[n++] = x * sgn;
    }
    return n;
}

// Resize (only larger) or free (size=0)
bool vm_resize(VM *vm, const int newsize)
{
    if (newsize < 0)
        return false;
    if (newsize == 0) {
        free(vm->mem);
        vm->mem = NULL;
        vm->size = 0;
        return true;
    }
    if (newsize <= vm->size)
        return true;
    if (vm->mem) {
        void *t = realloc(vm->mem, newsize * sizeof (VMType));
        if (t == NULL)
            return false;
        memset(t + vm->size * sizeof (VMType), 0, (newsize - vm->size) * sizeof (VMType));
        vm->mem = t;
        vm->size = newsize;
        return true;
    }
    void *t = calloc(newsize, sizeof (VMType));
    if (t == NULL)
        return false;
    vm->mem = t;
    vm->size = newsize;
    return true;
}

// Full VM initialisation
bool vm_init(VM *const vm, const VMType *const code, const int codesize)
{
    if (codesize <= 0)
        return false;
    if (codesize > vm->size && !vm_resize(vm, codesize))
        return false;
    memcpy(vm->mem, code, codesize * sizeof *code);  // fresh copy of intcode program
    // vm->ip, vm->base, vm->id static init = 0
    vm->state = VM_STATE_OK;  // initialisation done, ready to run
}

// Enqueue = push onto the head of the queue
bool vm_enq(VMQueue *const vmq, const VMType val)
{
    if (vmq->len >= vmq->cap)
        return false;
    vmq->len++;
    vmq->q[vmq->ins++] = val;
    if (vmq->ins >= vmq->cap)
        vmq->ins = 0;
    return true;
}

// Dequeue = pop off the tail of the queue
bool vm_deq(VMQueue *const vmq, VMType *const val)
{
    if (vmq->len <= 0)
        return false;
    vmq->len--;
    *val = vmq->q[vmq->pop++];
    if (vmq->pop >= vmq->cap)
        vmq->pop = 0;
    return true;
}

// Provide input value to VM
bool vm_write_input(VM *const vm, const VMType inputval)
{
    return vm_enq(&vm->inp, inputval);
}

// Retrieve output value from VM
bool vm_read_output(VM *const vm, VMType *const outputval)
{
    return vm_deq(&vm->out, outputval);
}

int run(VM *const vm)
{
    VMType *p[VM_PAR_SIZE] = {0};  // max 3 instruction parameters
    vm->state = VM_STATE_RUN;
    for (; vm->ip >= 0 && vm->ip < vm->size; ) {  // TODO: check specs
        const uint16_t instr = vm_decode[vm->mem[vm->ip++]];  // avoid decimal decoding

        const int parcount = instr & 3;  // param count in 2 LSBs
        if (vm->ip + parcount > vm->size)  // TODO: check specs
            return (vm->state = VM_STATE_ERR);  // param fetch beyond size
        for (int i = 0, k = 2; i < parcount; ++i, k += 2) {
            VMType *const ix = &vm->mem[vm->ip++];
            VMType addr = *ix;
            switch (instr >> k & 3) {
                case VM_PAR_REL:
                    addr += vm->base;
                    /* fall-through */
                case VM_PAR_POS:
                    if (addr < 0)
                        return (vm->state = VM_STATE_ERR);  // param fetch below zero
                    if (addr >= vm->size && !vm_resize(vm, addr + 1))
                        return (vm->state = VM_STATE_ERR);  // resize failed
                    p[i] = &vm->mem[addr];
                    break;
                case VM_PAR_IMM:
                    p[i] = ix;
                    break;
                default:
                    return (vm->state = VM_STATE_ERR);  // unknown parmode
            }
        }

        switch (instr >> 8) {  // MSByte = opcode
            case VM_OP_ADD: *p[2] = *p[0] + *p[1]; break;
            case VM_OP_MUL: *p[2] = *p[0] * *p[1]; break;
            case VM_OP_INP:
                if (vm_deq(&vm->inp, p[0]))
                    break;
                vm->ip -= 2;  // back to INP
                return (vm->state = VM_STATE_SUS);  // input queue empty
            case VM_OP_OUT:
                if (vm_enq(&vm->out, *p[0]))
                    break;
                vm->ip -= 2;  // back to OUT
                return (vm->state = VM_STATE_SUS);  // output queue full
            case VM_OP_JNZ: if (*p[0] != 0) vm->ip = *p[1]; break;
            case VM_OP_JZ : if (*p[0] == 0) vm->ip = *p[1]; break;
            case VM_OP_LT : *p[2] = *p[0] <  *p[1]; break;
            case VM_OP_EQ : *p[2] = *p[0] == *p[1]; break;
            case VM_OP_RBO: vm->base += *p[0]; break;
            case VM_OP_HLT: return (vm->state = VM_STATE_HLT);
            default: return (vm->state = VM_STATE_ERR);  // unknown opcode
        }
    }
    return (vm->state = VM_STATE_ERR);  // instruction fetch beyond size
}
