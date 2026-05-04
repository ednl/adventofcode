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
 * Test output with timer enabled:
 *     ./a.out | tail -n2
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 0.35 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 0.47 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 0.66 µs
 */

#include <stdio.h>
#include <stdlib.h>  // div
#include <string.h>  // memcpy
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME  "../aocinput/2019-02-input.txt"
#define FSIZE  288       // needed for my input: 273
#define VMSIZE 128       // needed for my input: 109
#define TARGET 19690720  // date of the first moon landing

typedef enum opcode {
    ADD = 1,  // add params and store
    MUL = 2,  // multiply params and store
    RET = 99  // halt program
} OpCode;

typedef struct vm {
    int app[VMSIZE];  // ROM code, save for next run
    int mem[VMSIZE];  // RAM, run self-modifying program
    int len;
} VM;

static char input[FSIZE];
static VM vm;

static int run(const int noun, const int verb)
{
    memcpy(vm.mem, vm.app, vm.len * sizeof *vm.mem);
    vm.mem[1] = noun;
    vm.mem[2] = verb;
    for (int *m = vm.mem;; m += 4)
        switch (*m) {
            case ADD: vm.mem[*(m + 3)] = vm.mem[*(m + 1)] + vm.mem[*(m + 2)]; break;
            case MUL: vm.mem[*(m + 3)] = vm.mem[*(m + 1)] * vm.mem[*(m + 2)]; break;
            case RET: return vm.mem[0];
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
    vm.len = 0;
#endif

    // Parse CSV to vm.app
    for (const char *c = input; *c; ++c) {
        int x = *c++ & 15;
        while (*c >= '0')
            x = x * 10 + (*c++ & 15);
        vm.app[vm.len++] = x;
    }

    // Part 1
    printf("%d\n", run(12, 2));  // part 1: 3085697

    // Part 2
    const int base  = run(0, 0);
    const int dnoun = run(1, 0) - base;
    // const int dverb = run(0, 1) - base;  // assume dverb==1
    const div_t sol = div(TARGET - base, dnoun);
    printf("%d\n", sol.quot * 100 + sol.rem);  // part 2: 9425

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());
#endif
}
