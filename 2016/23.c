/**
 * Advent of Code 2016
 * Day 23: Safe Cracking
 * https://adventofcode.com/2016/day/23
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile with warnings:
 *     cc -std=c17 -Wall -Wextra -pedantic 23.c
 * Compile for speed, with timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 23.c
 * Get minimum runtime from timer output in bash:
 *     m=9999999;for((i=0;i<10;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements including result output:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 3.2 s
 *     Mac Mini 2020 (M1 3.2 GHz)    :   ? s
 *     Raspberry Pi 5 (2.4 GHz)      :   ? s
 */

#include <stdio.h>
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRId64
#include <string.h>    // memcpy, memset
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2016-23-input.txt"
#define MEMSIZE 32  // needed for my input: 26
#define REGCOUNT 4

typedef enum opcode {
    NOP, INC, DEC, CPY, JNZ, TGL
} Opcode;

typedef enum mode {
    NONE, IMM, REG
} Mode;

typedef struct Assembunny {
    Opcode op;
    int p[2];
    Mode m[2];
} Assembunny;

static Assembunny mem[MEMSIZE], bak[MEMSIZE];
static int64_t reg[REGCOUNT];
static int memsize, ip;

static int parse(void)
{
    FILE *f = fopen(FNAME, "r");
    if (!f) {
        fprintf(stderr, "File not found: "FNAME"\n");
        return 0;  // zero lines read
    }
    char buf[16];
    int n = 0, x, y;
    char c, d;
    while (n < MEMSIZE && fgets(buf, sizeof buf, f)) {
        switch (buf[0]) {
            case 'i': bak[n++] = (Assembunny){INC, {buf[4] - 'a', 0}, {REG, NONE}}; break;
            case 'd': bak[n++] = (Assembunny){DEC, {buf[4] - 'a', 0}, {REG, NONE}}; break;
            case 't':  // tgl
                if (buf[4] >= 'a')
                    bak[n++] = (Assembunny){TGL, {buf[4] - 'a', 0}, {REG, NONE}};
                else if (sscanf(&buf[4], "%d", &x) == 1)
                    bak[n++] = (Assembunny){TGL, {x, 0}, {IMM, NONE}};
                else
                    bak[n++] = (Assembunny){NOP, {0}, {0}};
                break;
            case 'c':  // cpy
                if (buf[4] >= 'a')
                    bak[n++] = (Assembunny){CPY, {buf[4] - 'a', buf[6] - 'a'}, {REG, REG}};
                else if (sscanf(&buf[4], "%d %c", &x, &d) == 2)
                    bak[n++] = (Assembunny){CPY, {x, d - 'a'}, {IMM, REG}};
                else
                    bak[n++] = (Assembunny){NOP, {0}, {0}};
                break;
            case 'j':  // jnz
                if (sscanf(&buf[4], "%d %d", &x, &y) == 2)
                    bak[n++] = (Assembunny){JNZ, {x, y}, {IMM, IMM}};
                else if (sscanf(&buf[4], "%d %c", &x, &d) == 2)
                    bak[n++] = (Assembunny){JNZ, {x, d - 'a'}, {IMM, REG}};
                else if (sscanf(&buf[4], "%c %d", &c, &y) == 2)
                    bak[n++] = (Assembunny){JNZ, {c - 'a', y}, {REG, IMM}};
                else if (sscanf(&buf[4], "%c %c", &c, &d) == 2)
                    bak[n++] = (Assembunny){JNZ, {c - 'a', d - 'a'}, {REG, REG}};
                else
                    bak[n++] = (Assembunny){NOP, {0}, {0}};
                break;
        }
    }
    fclose(f);
    return n;
}

#ifdef DEBUG
static void list(const Assembunny *const bank, const int count)
{
    static const char *instr[] = {"nop", "inc", "dec", "cpy", "jnz", "tgl"};
    puts("----------------");
    for (int i = 0; i < count; ++i) {
        printf("%2d: %s", i, instr[bank[i].op]);
        for (int j = 0; j < 2; ++j)
            switch (bank[i].m[j]) {
                case NONE: break;
                case IMM: printf(" %d", bank[i].p[j]); break;
                case REG: printf(" %c", 'a' + bank[i].p[j]); break;
            }
        putchar('\n');
    }
    puts("----------------");
}
#endif

static bool ix(const int i)
{
    return i >= 0 && i < REGCOUNT;
}

static bool mode(const Assembunny *const a, const Mode mx, const Mode my)
{
    return a->m[0] == mx && a->m[1] == my && (mx != REG || ix(a->p[0])) && (my != REG || ix(a->p[1]));
}

static int64_t run(const int rega)
{
    memcpy(mem, bak, sizeof mem);
    memset(reg, 0, sizeof reg);
    reg[0] = rega;
    ip = 0;
    while (ip >= 0 && ip < memsize) {
        Assembunny *a = &mem[ip];
        switch (a->op) {
            case NOP:
                ++ip;
                break;
            case INC:
                if (mode(a, REG, NONE))
                    ++reg[a->p[0]];
                ++ip;
                break;
            case DEC:
                if (mode(a, REG, NONE))
                    --reg[a->p[0]];
                ++ip;
                break;
            case CPY:
                if (mode(a, REG, REG))
                    reg[a->p[1]] = reg[a->p[0]];
                else if (mode(a, IMM, REG))
                    reg[a->p[1]] = a->p[0];
                ++ip;
                break;
            case JNZ:
                if (mode(a, REG, REG))
                    ip += reg[a->p[0]] ? reg[a->p[1]] : 1;
                else if (mode(a, REG, IMM))
                    ip += reg[a->p[0]] ? a->p[1] : 1;
                else if (mode(a, IMM, REG))
                    ip += a->p[0] ? reg[a->p[1]] : 1;
                else if (mode(a, IMM, IMM))
                    ip += a->p[0] ? a->p[1] : 1;
                break;
            case TGL:
            #ifdef DEBUG
                printf("%lld %lld %lld %lld\n", reg[0], reg[1], reg[2], reg[3]);
            #endif
                {
                    int64_t t = -1;
                    if (mode(a, REG, NONE))
                        t = ip + reg[a->p[0]];
                    else if (mode(a, IMM, NONE))
                        t = ip + a->p[0];
                    if (t >= 0 && t < memsize) {
                        if (mem[t].m[1] == NONE)
                            mem[t].op = mem[t].op == INC ? DEC : INC;
                        else
                            mem[t].op = mem[t].op == JNZ ? CPY : JNZ;
                    }
                }
                ++ip;
                break;
        }
    }
    return reg[0];
}

int main(void)
{
    memsize = parse();

#ifdef TIMER
    starttimer();
#endif

#ifdef DEBUG
    list(bak, memsize);
#endif
    printf("Part 1: %"PRId64"\n", run(7));  // 11424
#ifdef DEBUG
    list(mem, memsize);
#endif
    printf("Part 2: %"PRId64"\n", run(12));  // 479007984
#ifdef DEBUG
    list(mem, memsize);
#endif

#ifdef TIMER
    printf("Time: %.0f ms\n", stoptimer_ms());
#endif
}
