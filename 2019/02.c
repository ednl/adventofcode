/**
 * Advent of Code 2019
 * Day 2: 1202 Program Alarm
 * https://adventofcode.com/2019/day/2
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 02.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 02.c
 * Get minimum runtime from timer output in bash:
 *     m=9999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 1.50 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :    ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : 7.91 µs
 */

#include <stdio.h>
#include <stdlib.h>    // calloc, free
#include <string.h>    // memcpy
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRId64
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2019-02-input.txt"
#define VMSIZE 128
#define TARGET 19690720

typedef enum opcode {
    ADD = 1,  // add params and store
    MUL = 2,  // multiply params and store
    RET = 99  // halt program
} OpCode;

typedef struct vm {
    int64_t *app;
    int64_t *mem;
    int cap, len;
} VM;

static int csv(const char *fname, int64_t *const list, const int size)
{
    FILE *f = fopen(fname, "r");
    if (!f) return 0;
    const int64_t *const end = list + size;
    int64_t *x = list;
    *x = 0;
    int s = 1;
    for (int c = fgetc(f); c != EOF && x != end; c = fgetc(f)) {
        if (c >= '0' && c <= '9')
            *x = *x * 10 + (c & 15);
        else if (c == ',') {
            *x++ *= s;
            *x = 0;
            s = 1;
        } else if (c == '-')
            s = -1;
    }
    fclose(f);
    *x++ *= s;
    return x - list;
}

static int64_t run(VM *vm, const int noun, const int verb)
{
    int64_t *const m = vm->mem;
    memcpy(m, vm->app, vm->len * sizeof *m);
    m[1] = noun;
    m[2] = verb;
    for (int ip = 0;; ip += 4)
        switch (m[ip]) {
            case ADD: m[m[ip + 3]] = m[m[ip + 1]] + m[m[ip + 2]]; break;
            case MUL: m[m[ip + 3]] = m[m[ip + 1]] * m[m[ip + 2]]; break;
            case RET: return m[0];
        }
}

int main(void)
{
    VM vm = {
        .app = calloc(VMSIZE * 2, sizeof *vm.app),
        .cap = VMSIZE
    };
    vm.mem = vm.app + VMSIZE;
    vm.len = csv(FNAME, vm.app, vm.cap);

#ifdef TIMER
    starttimer();
#endif

    // Part 1
    printf("%"PRId64"\n", run(&vm, 12, 2));  // 3085697

    // Part 2
    const int base  = run(&vm, 0, 0);
    const int dnoun = run(&vm, 1, 0) - base;
    const int dverb = run(&vm, 0, 1) - base;
    if (dverb == 1) {
        const int target = TARGET - base;
        const int noun = target / dnoun;
        const int verb = target % dnoun;
        printf("%d\n", noun * 100 + verb);  // 9425
    }

#ifdef TIMER
    printf("Time: %.0f ns\n", stoptimer_ns());
#endif
    free(vm.app);
}
