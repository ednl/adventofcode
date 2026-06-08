/**
 * Advent of Code 2020
 * Day 8: Handheld Halting
 * https://adventofcode.com/2020/day/8
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic -Wno-char-subscripts 08.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 08.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  3.44 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :  ? µs
 *     Raspberry Pi 5 (2.4 GHz)      :  ? µs
 */

#include <stdio.h>
#include <string.h>  // memset
#include <stdint.h>
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2020-08-input.txt"
#define FSIZE 5058
#define MEM 623  // instructions (= lines in input file)

typedef struct instr {
    char op;      // a=acc, j=jmp, n=nop
    int16_t arg;  // my input: -599..+562
} Instr;

static char input[FSIZE];

// Virtual Machine and memory size, list of instruction addresses to debug
static Instr vm[MEM];
static int debug[MEM];  // index of nop or jmp (that are not 0 or +1)
static bool seen[MEM];  // visited this instruction? separate for easy reset

// Run the VM once
// Returns: accumulator value
// Sets: whether the program looped or ended
static int run(bool *const loop)
{
    int ip = 0, acc = 0;
    memset(seen, 0, sizeof seen);
    do {
        seen[ip] = true;
        switch (vm[ip].op) {
            case 'a': acc += vm[ip++].arg; break;
            case 'j': ip += vm[ip].arg;    break;
            default : ip++;                break;
        }
    } while (ip != MEM && !seen[ip]);
    *loop = (ip != MEM);
    return acc;
}

static void flip(const int i)
{
    vm[i].op ^= 4;  // j->n, n->j
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    fread(input, FSIZE, 1, f);
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    const char *c = input;
    for (int i = 0; i < MEM; ++c, ++i) {
        vm[i].op = *c;
        c += 4;
        const int sgn = *c++ == '+' ? 1 : -1;
        int x = *c++ & 15;
        while (*c != '\n')
            x = x * 10 + (*c++ & 15);
        vm[i].arg = sgn * x;
    }

    // Part 1
    bool loop;
    printf("%d", run(&loop));  // part 1: 1331

    // Part 2
    // Find all possibly corrupt instructions = jmp/nop, not +1
    int k = 0;
    for (int i = 0; i < MEM; ++i)
        if (seen[i] && (vm[i].op & 8) && vm[i].arg != 1)
            debug[k++] = i;
    // Run the VM once with every debug instruction flipped
    // until the program ends (instead of loops)
    int acc = 0;
    while (loop && k--) {
        flip(debug[k]);
        acc = run(&loop);
        flip(debug[k]);
    }
    printf(" %d\n", acc);  // part 2: 1121

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
