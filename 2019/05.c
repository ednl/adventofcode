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
 * Get minimum runtime from timer output in bash:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 3.75 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : ? µs
 */

#include <stdio.h>
#include <stdlib.h>    // calloc, realloc, free
#include <string.h>    // memcpy
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRId64
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2019-05-input.txt"

typedef enum vmopcode {
    VM_ADD = 1,  // 1: add params and store
    VM_MUL,      // 2: multiply params and store
    VM_INP,      // 3: input and store
    VM_OUT,      // 4: output param
    VM_JNZ,      // 5: jump non-zero (if true)
    VM_JZ,       // 6: jump zero (if false)
    VM_LT,       // 7: is less than, store 0/1
    VM_EQ,       // 8: is equal, store 0/1
    VM_RBO,      // 9: relative base offset (=set base)
    VM_RET = 99  // 99: halt program
} VMOpCode;

typedef enum vmparmode {
    VM_POS,  // 0: positional (=indirect, by reference)
    VM_IMM,  // 1: immediate (=direct, by value),
    VM_REL   // 2: relative = positional with base offset
} VMParMode;

typedef struct vmlang {
    VMOpCode opcode;
    int read, write;  // number of read/write parameters
    int len;  // total instruction length: opcode+params
} VMLang;

static const VMLang vmlang[] = {
    [VM_ADD] = {VM_ADD, 2, 1, 4},
    [VM_MUL] = {VM_MUL, 2, 1, 4},
    [VM_INP] = {VM_INP, 0, 1, 2},
    [VM_OUT] = {VM_OUT, 1, 0, 2},
    [VM_JNZ] = {VM_JNZ, 2, 0, 3},
    [VM_JZ ] = {VM_JZ , 2, 0, 3},
    [VM_LT ] = {VM_LT , 2, 1, 4},
    [VM_EQ ] = {VM_EQ , 2, 1, 4},
    [VM_RBO] = {VM_RBO, 1, 0, 2},
    [VM_RET] = {VM_RET, 0, 0, 1},
};

typedef struct vm {
    int64_t  *mem;
    int ip, base, size;
} VM;

static bool vm_resize(VM *vm, const int newsize)
{
    if (vm == NULL || newsize < 0)
        return false;
    if (newsize == 0) {  // delete
        free(vm->mem);
        *vm = (VM){0};
        return true;
    }
    // Round up to multiple of 1024
    const int chunk = (((newsize - 1) >> 10) + 1) << 10;
    if (vm->mem == NULL) {  // not allocated yet
        void *mem = calloc(chunk, sizeof *vm->mem);
        if (mem == NULL)
            return false;
        *vm = (VM){.mem = mem, .ip = 0, .base = 0, .size = chunk};
        return true;
    }
    if (chunk <= vm->size)  // no resize needed
        return true;
    void *newmem = realloc(vm->mem, chunk * sizeof *vm->mem);
    if (newmem == NULL)
        return false;
    vm->mem = newmem;
    memset(vm->mem + vm->size, 0, (chunk - vm->size) * sizeof *vm->mem);
    vm->size = chunk;
    return true;
}

static bool vm_grow(VM *vm)
{
    if (vm == NULL)
        return false;
    const int newsize = vm->mem == NULL || vm->size <= 0 ? 1 : vm->size + 1;  // rely on rounding up
    return vm_resize(vm, newsize);
}

static int vm_csv(VM *vm, const char *fname)
{
    if (vm == NULL || fname == NULL)
        return 0;
    if ((vm->mem == NULL || vm->size <= 0) && !vm_grow(vm))
        return 0;
    FILE *f = fopen(fname, "r");
    if (f == NULL)
        return 0;
    int64_t x = 0;
    int count = 0, sign = 1, c = 0;
    while (c != EOF && count < vm->size) {
        if ((c = fgetc(f)) == '-') {
            sign = -1;
            c = fgetc(f);
        } else
            sign = 1;
        x = 0;
        while (c >= '0' && c <= '9') {
            x = x * 10 + (c & 15);
            c = fgetc(f);
        }
        vm->mem[count++] = x * sign;
        if (count == vm->size && c != EOF)
            vm_grow(vm);
    }
    fclose(f);
    return count;
}

static bool vm_init(VM *dst, const VM *src)
{
    if (dst == NULL || src == NULL || src->mem == NULL || src->size <= 0)
        return false;
    if (dst->size < src->size && !vm_resize(dst, src->size))
        return false;
    memcpy(dst->mem, src->mem, src->size * sizeof *src->mem);
    if (dst->size > src->size)
        memset(dst->mem + src->size, 0, (dst->size - src->size) * sizeof *dst->mem);
    return true;
}

// Day 5 manual input automated: part 1: 1, part 2: 5
static int vm_input(void)
{
    static const int inp[] = {1, 5};
    static int i = 0;
    return inp[i++];
    // char buf[32] = {0};
    // int64_t val;
    // while (1) {
    //     printf("? ");
    //     fgets(buf, sizeof buf, stdin);
    //     if (sscanf(buf, "%"PRId64, &val) == 1)
    //         return val;
    // }
}

// ASCII output (or newline) if possible
static void vm_output(const int64_t val)
{
    static int i = 0;
    i++;
    if ((val >= 32 && val < 127) || val == '\n')
        printf("%c", val);
    else
        printf("%d: %"PRId64"\n", i, val);
}

static int vm_mode_error(const int64_t *mem, const int ip, const int mode)
{
    fprintf(stderr, "Unknown mode %d at address %d: %"PRId64"\n", mode, ip, mem[ip]);
    return -1;
}

static int vm_segfault(const VM *vm)
{
    fprintf(stderr, "Segmentation fault: ip=%d base=%d size=%d\n", vm->ip, vm->base, vm->size);
    return -1;
}

static int64_t vm_run(VM *vm)
{
    // int64_t *ip = &vm->mem[vm->ip];
    int64_t p[2], *q;  // max 2 read params, max 1 write param
    for (; vm->ip >= 0 && vm->ip < vm->size; ) {

        const VMOpCode opcode = vm->mem[vm->ip] % 100;
        int modes = vm->mem[vm->ip] / 100;
        vm->ip++;

        for (int i = 0; i < vmlang[opcode].read; ++i) {
            const VMParMode mode = modes % 10;
            modes /= 10;
            switch (mode) {
                case 0: p[i] = vm->mem[vm->mem[vm->ip]];            break;  // positional
                case 1: p[i] = vm->mem[vm->ip];                     break;  // immediate
                case 2: p[i] = vm->mem[vm->mem[vm->ip] + vm->base]; break;  // offset
                default: return vm_mode_error(vm->mem, vm->ip - i - 1, mode);
            }
            vm->ip++;
        }
        if (vmlang[opcode].write) {
            switch (modes % 10) {
                case 0: q = &vm->mem[*vm->ip];  break;  // positional
                case 2: q = &vm->base[*vm->ip]; break;  // offset
                default: return vm_mode_error(vm->mem, vm->ip - lang[opcode].read - 1, modes);
            }
            vm->ip++;
        }
        switch (opcode) {
            case VM_ADD: *q = p[0] + p[1];  break;
            case VM_MUL: *q = p[0] * p[1];  break;
            case VM_INP: *q = vm_input();   break;
            case VM_OUT: vm_output(p[0]);   break;
            case VM_JNZ: if (p[0] != 0) vm->ip = &vm->mem[p[1]]; break;
            case VM_JZ : if (p[0] == 0) vm->ip = &vm->mem[p[1]]; break;
            case VM_LT : *q = p[0] <  p[1]; break;
            case VM_EQ : *q = p[0] == p[1]; break;
            case VM_RBO: vm->base += p[0];  break;
            case VM_RET: return 0;
        }
    }
    return vm_segfault(vm);
}

int main(void)
{
    VM app = {0}, vm = {0};
    if (!vm_csv(&app, FNAME))
        return 1;

#ifdef TIMER
    starttimer();
#endif

    // Part 1: input 1
    vm_init(&vm, &app);
    vm_run(&vm);  // 13547311

    // Part 2: input 5
    vm_init(&vm, &app);  // reset is necessary
    vm_run(&vm);  // 236453

#ifdef TIMER
    printf("Time: %.0f ns\n", stoptimer_ns());
#endif
    vm_resize(&app, 0);
    vm_resize(&vm, 0);
}
