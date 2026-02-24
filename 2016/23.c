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
 *     m=9999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements including result output:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 1.12 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :    ? µs
 *     Raspberry Pi 5 (2.4 GHz)      :    ? µs
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
#define REGCOUNT 4  // registers A..D

// Expanded from spec: NOP, ADD, MUL
typedef enum opcode {
    NOP, INC, DEC, CPY, ADD, MUL, JNZ, TGL
} Opcode;

// Parameter mode: none, immediate (=number), register (=index 0..3)
typedef enum mode {
    NONE, IMM, REG
} Mode;

typedef struct assembunny {
    Opcode op;
    int p[2];   // parameter
    Mode m[2];  // mode
} Assembunny;

static Assembunny mem[MEMSIZE], src[MEMSIZE];
static int64_t reg[REGCOUNT];
static int memsize, ip;  // program size, instruction pointer

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
            case 'i': src[n++] = (Assembunny){INC, {buf[4] - 'a', 0}, {REG, NONE}}; break;
            case 'd': src[n++] = (Assembunny){DEC, {buf[4] - 'a', 0}, {REG, NONE}}; break;
            case 't':  // tgl
                if (buf[4] >= 'a')
                    src[n++] = (Assembunny){TGL, {buf[4] - 'a', 0}, {REG, NONE}};
                else if (sscanf(&buf[4], "%d", &x) == 1)
                    src[n++] = (Assembunny){TGL, {x, 0}, {IMM, NONE}};
                else
                    src[n++] = (Assembunny){NOP, {0}, {0}};
                break;
            case 'c':  // cpy
                if (buf[4] >= 'a')
                    src[n++] = (Assembunny){CPY, {buf[4] - 'a', buf[6] - 'a'}, {REG, REG}};
                else if (sscanf(&buf[4], "%d %c", &x, &d) == 2)
                    src[n++] = (Assembunny){CPY, {x, d - 'a'}, {IMM, REG}};
                else
                    src[n++] = (Assembunny){NOP, {0}, {0}};
                break;
            case 'j':  // jnz
                if (sscanf(&buf[4], "%d %d", &x, &y) == 2)
                    src[n++] = (Assembunny){JNZ, {x, y}, {IMM, IMM}};
                else if (sscanf(&buf[4], "%d %c", &x, &d) == 2)
                    src[n++] = (Assembunny){JNZ, {x, d - 'a'}, {IMM, REG}};
                else if (sscanf(&buf[4], "%c %d", &c, &y) == 2)
                    src[n++] = (Assembunny){JNZ, {c - 'a', y}, {REG, IMM}};
                else if (sscanf(&buf[4], "%c %c", &c, &d) == 2)
                    src[n++] = (Assembunny){JNZ, {c - 'a', d - 'a'}, {REG, REG}};
                else
                    src[n++] = (Assembunny){NOP, {0}, {0}};
                break;
        }
    }
    fclose(f);
    // Reverse engineer dec+jnz as add, twice as mul
    // after executing, set loop registers to zero
    for (int i = 0; i < n; ++i) {
        if (src[i].op == JNZ && src[i].p[1] == -2) {
            if (src[i + 2].p[1] == -5) {
                // Multiply: A += C * D
                src[i - 2] = (Assembunny){MUL, {0}, {0}};  // params always the same, handled in exec loop
                src[i - 1] = (Assembunny){0};  // NOP
                src[i    ] = (Assembunny){0};
                src[i + 1] = (Assembunny){0};
                src[i + 2] = (Assembunny){0};
            } else {
                // Add: reg[accum] += reg[count]
                const int counter = src[i].p[0];
                const int accu = src[i - 1].p[0] == counter ? src[i - 2].p[0] : src[i - 1].p[0];
                src[i - 2] = (Assembunny){ADD, {accu, counter}, {REG, REG}};
                src[i - 1] = (Assembunny){0};  // NOP
                src[i    ] = (Assembunny){0};
            }
        }
    }
    return n;
}

#ifdef DEBUG
static void list(const Assembunny *const bank, const int count)
{
    static const char *instr[] = {"nop", "inc", "dec", "cpy", "add", "mul", "jnz", "tgl"};
    puts("-----------------");
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
    puts("-----------------");
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
    memcpy(mem, src, sizeof mem);
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
            case ADD:
                reg[a->p[0]] += reg[a->p[1]];  // reg[p[0]] += reg[p[1]]
                reg[a->p[1]] = 0;              // reg[p[1]] = 0
                ip += 3;
                break;
            case MUL:
                reg[0] += reg[2] * reg[3];  // A += C * D
                reg[2] = reg[3] = 0;        // C = D = 0
                ip += 5;
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
    list(src, memsize);
#endif
    printf("Part 1: %"PRId64"\n", run(7));   // 11424
    printf("Part 2: %"PRId64"\n", run(12));  // 479007984
#ifdef DEBUG
    list(mem, memsize);
#endif

#ifdef TIMER
    printf("Time: %.0f ns\n", stoptimer_ns());
#endif
}
