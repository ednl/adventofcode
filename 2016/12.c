/**
 * Advent of Code 2016
 * Day 12: Leonardo's Monorail
 * https://adventofcode.com/2016/day/12
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile with warnings:
 *     cc -std=c17 -Wall -Wextra -pedantic 12.c -lm
 * Compile for speed, with timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 12.c -lm
 * Compile with warnings, no floating point maths:
 *     cc -std=c17 -Wall -Wextra -pedantic -DNOFPU 12.c
 * Compile for speed, with timer, no floating point maths:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c -DNOFPU 12.c
 * Get minimum runtime from timer output in bash:
 *     m=9999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements with result output but without reading/parsing input file:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  2.04 µs (same as NOFPU!)
 *     Mac Mini 2020 (M1 3.2 GHz)    :  3.46 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 11.6  µs
 * Minimum runtime measurements with result output but without reading/parsing input file, NOFPU:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  2.04 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :  3.17 µs
 *     Raspberry Pi 5 (2.4 GHz)      :  7.33 µs
 */

#include <stdio.h>
#include <string.h>  // memset
#ifndef NOFPU
    #include <math.h>    // exp, round
    #define LOGPHI   0.4812118250596029
    #define LOGSQRT5 0.8047189562170503
#endif
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2016-12-input.txt"
#define MEMSIZE 32  // needed for my input: 23
#define REGCOUNT 4

typedef enum op {
    NOP, INC, DEC, STO, CPY, JMP, JNZ, FIB
} Op;

typedef struct assembunny {
    Op op;
    int a, b;
} Assembunny;

static Assembunny mem[MEMSIZE];
static int reg[REGCOUNT];
static int memsize, ip;

#ifdef NOFPU
// n'th Fibonacci number from iteration
static int fib(int n)
{
    int a = 0, b = 1;
    for (; n > 1; n -= 2) {
        a += b;
        b += a;
    }
    return n ? b : a;
}
#else
// n'th Fibonacci number from golden ratio approximation
static int fib(const int n)
{
    return (int)round(exp(LOGPHI * n - LOGSQRT5));
}
#endif

static int parse(void)
{
    FILE *f = fopen(FNAME, "r");
    if (!f) {
        fprintf(stderr, "File not found: "FNAME"\n");
        return 0;  // return linecount=0
    }
    char buf[16];
    int n = 0, a, b;
    char c, d;
    while (n < MEMSIZE && fgets(buf, sizeof buf, f)) {
        switch (buf[0]) {
            case 'i':
            case 'd':
                mem[n++] = (Assembunny){buf[0] == 'i' ? INC : DEC, buf[4] - 'a', 0};
                break;
            case 'c':
                if (buf[4] >= 'a') {
                    sscanf(&buf[4], "%c %c", &c, &d);
                    mem[n++] = (Assembunny){CPY, c - 'a', d - 'a'};
                } else {
                    sscanf(&buf[4], "%d %c", &a, &c);
                    mem[n++] = (Assembunny){STO, a, c - 'a'};
                }
                break;
            case 'j':
                if (buf[4] >= 'a') {
                    sscanf(&buf[4], "%c %d", &c, &b);
                    mem[n++] = (Assembunny){JNZ, c - 'a', b};
                } else {
                    sscanf(&buf[4], "%d %d", &a, &b);
                    mem[n++] = a ? (Assembunny){JMP, b, 0} : (Assembunny){NOP, 0, 0};
                }
                break;
        }
    }
    fclose(f);
    // Reverse engineered from input file
    // reg d is countdown for consecutive Fibonacci numbers in reg a
    // reg a goes: 1,2,3,5,... so starts with Fib[2] (skips Fib[0]=0, Fib[1]=1)
    // which means reg d starts with n-2 for n'th Fib number ending up in reg a
    // => instructions in mem[9..15] can be replaced by: a=fib(d+2), jmp +6
    mem[ 9] = (Assembunny){FIB, 'd' - 'a', 2};  // a=fib(d+2) (a is implied)
    mem[10] = (Assembunny){JMP, 6, 0};  // jump over old Fib calculation
    return n;
}

#if DEBUG
static void list(void)
{
    static const char *ins[] = {"nop", "inc", "dec", "sto", "cpy", "jmp", "jnz", "fib"};
    for (int i = 0; i < memsize; ++i) {
        printf("%s ", ins[mem[i].op]);
        switch (mem[i].op) {
            case NOP: break;
            case INC: /* fall-through */
            case DEC: printf("%c", 'a' + mem[i].a);                    break;
            case STO: printf("%d %c", mem[i].a, 'a' + mem[i].b);       break;
            case CPY: printf("%c %c", 'a' + mem[i].a, 'a' + mem[i].b); break;
            case JMP: printf("%d", mem[i].a);                          break;
            case JNZ: printf("%c %d", 'a' + mem[i].a, mem[i].b);       break;
            case FIB: printf("%c+%d", 'a' + mem[i].a, mem[i].b);       break;
        }
        putchar('\n');
    }

}
#endif

static int run(const int regc)
{
    memset(reg, 0, sizeof reg);
    reg[2] = regc;
    ip = 0;
    while (ip < memsize) {
        switch (mem[ip].op) {
            case NOP: ip++; break;
            case INC: reg[mem[ip].a]++; ip++; break;
            case DEC: reg[mem[ip].a]--; ip++; break;
            case STO: reg[mem[ip].b] = mem[ip].a;      ip++; break;
            case CPY: reg[mem[ip].b] = reg[mem[ip].a]; ip++; break;
            case JMP: ip += mem[ip].a;                      break;
            case JNZ: ip += reg[mem[ip].a] ? mem[ip].b : 1; break;
            case FIB: reg[0] = fib(reg[mem[ip].a] + mem[ip].b); ip++; break;  // reverse engineered
        }
    }
    return reg[0];  // reg a
}

int main(void)
{
    memsize = parse();
#if DEBUG
    list();
#endif
#ifdef TIMER
    starttimer();
#endif
    printf("%d %d\n", run(0), run(1));  // 318009 9227663
#ifdef TIMER
    printf("Time: %.0f ns\n", stoptimer_ns());
#endif
}
