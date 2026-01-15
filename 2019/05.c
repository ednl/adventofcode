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
 *     m=9999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
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
#define VMSIZE 1024  // virtual machine base memory size

typedef enum opcode {
    ADD = 1,  // 1: add params and store
    MUL,      // 2: multiply params and store
    INP,      // 3: input and store
    OUT,      // 4: output param
    JNZ,      // 5: jump non-zero (if true)
    JZ,       // 6: jump zero (if false)
    LT,       // 7: is less than, store 0/1
    EQ,       // 7: is equal, store 0/1
    RET = 99  // 99: halt program
} OpCode;

typedef enum mode {
    POS = 0,  // positional (=indirect, by reference)
    IMM = 1,  // immediate (=direct, by value)
} Mode;

typedef struct lang {
    OpCode opcode;
    int read, write;  // number of read/write parameters
    int len;  // total instruction length: opcode+params
} Lang;

static const Lang lang[] = {
    [ADD] = {ADD, 2, 1, 4},
    [MUL] = {MUL, 2, 1, 4},
    [INP] = {INP, 0, 1, 2},
    [OUT] = {OUT, 1, 0, 2},
    [JNZ] = {JNZ, 2, 0, 3},
    [JZ ] = {JZ , 2, 0, 3},
    [LT ] = {LT , 2, 1, 4},
    [EQ ] = {EQ , 2, 1, 4},
    [RET] = {RET, 0, 0, 1},
};

typedef struct vm {
    int64_t *mem;
    int len, cap;
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
    if (vm->mem == NULL) {  // not allocated yet: ignore .cap
        void *t = calloc(newsize, sizeof *(vm->mem));
        if (t == NULL)
            return false;
        *vm = (VM){.mem = t, .len = 0, .cap = newsize};
        return true;
    }
    if (newsize == vm->cap)  // no change
        return true;
    void *t = realloc(vm->mem, newsize * sizeof *(vm->mem));
    if (t == NULL)  // out of memory or other weirdness
        return false;
    vm->mem = t;
    if (newsize > vm->cap)  // grown: initialise new part to zero
        memset(vm->mem + vm->cap, 0, (newsize - vm->cap) * sizeof *(vm->mem));
    else if (vm->len > newsize)  // shrunk
        vm->len = newsize;
    vm->cap = newsize;
    return true;
}

static bool vm_grow(VM *vm)
{
    if (vm == NULL)
        return false;
    const int newsize = vm->mem == NULL || vm->cap <= 0 ? VMSIZE : vm->cap << 1;
    return vm_resize(vm, newsize);
}

static bool vm_csv(VM *vm, const char *fname)
{
    if (vm == NULL || fname == NULL)
        return false;
    if ((vm->mem == NULL || vm->cap <= 0) && !vm_grow(vm))
        return false;
    FILE *f = fopen(fname, "r");
    if (f == NULL)
        return false;
    int64_t x = 0;
    int i = 0, s = 1, c = 0;
    while (c != EOF && i < vm->cap) {
        if ((c = fgetc(f)) == '-') {
            s = -1;
            c = fgetc(f);
        } else
            s = 1;
        x = 0;
        while (c >= '0' && c <= '9') {
            x = x * 10 + (c & 15);
            c = fgetc(f);
        }
        vm->mem[i++] = x * s;
        if (i == vm->cap && c != EOF)
            vm_grow(vm);
    }
    fclose(f);
    vm->len = i;
    return true;
}

static bool vm_init(VM *vm, const VM *app)
{
    if (vm == NULL || app == NULL || app->mem == NULL || app->len <= 0)
        return false;
    if (vm->cap < app->len && !vm_resize(vm, app->len))
        return false;
    memcpy(vm->mem, app->mem, app->len * sizeof *(app->mem));
    vm->len = app->len;
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

static void vm_output(const int64_t val)
{
    printf("%"PRId64"\n", val);
}

static int64_t vm_run(VM *vm)
{
    int64_t *const m = vm->mem;
    int64_t p[3];
    for (int ip = 0; ip >= 0 && ip < vm->len; ) {
        OpCode opcode = m[ip] % 100;
        int mode = m[ip++] / 100;
        int i = 0;
        for (int j = 0; j < lang[opcode].read; ++j, mode /= 10)
            switch (mode % 10) {
                case 0: p[i++] = m[m[ip++]]; break;  // positional
                case 1: p[i++] = m[ip++];    break;  // immediate
            }
        if (lang[opcode].write)
            p[i++] = m[ip++];  // always positional but not dereferenced yet
        switch (opcode) {
            case ADD: m[p[2]] = p[0] + p[1];    break;
            case MUL: m[p[2]] = p[0] * p[1];    break;
            case INP: m[p[0]] = vm_input();     break;
            case OUT: vm_output(p[0]);          break;
            case JNZ: if (p[0] != 0) ip = p[1]; break;
            case JZ : if (p[0] == 0) ip = p[1]; break;
            case LT : m[p[2]] = p[0] <  p[1];   break;
            case EQ : m[p[2]] = p[0] == p[1];   break;
            case RET: return 0;
            default : return -1;
        }
    }
    return -RET;
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
