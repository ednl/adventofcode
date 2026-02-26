/**
 * Advent of Code 2016
 * Day 25: Clock Signal
 * https://adventofcode.com/2016/day/25
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile with warnings:
 *     cc -std=c17 -Wall -Wextra -pedantic 25.c
 * Compile for speed, with timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 25.c
 * Get minimum runtime from timer output in bash:
 *     m=9999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements including result output:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  2.29 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :     ? µs
 *     Raspberry Pi 5 (2.4 GHz)      :     ? µs
 */

#include <stdio.h>
#include <stdint.h>  // int64_t
#include <string.h>  // memset
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2016-25-input.txt"
#define MEMSIZE 32  // needed for my input: 30
#define REGCOUNT 4

// NOP: no operation
// STO: store value in register
// JMP: jump to memory address
// MUL: D += B * C
// DIV: B = A % 2; A /= 2
typedef enum opcode {
    NOP, INC, DEC, STO, CPY, MUL, DIV, JMP, JNZ, OUT
} Opcode;

typedef struct assembunny {
    Opcode op;
    int x, y;
} Assembunny;

static Assembunny mem[MEMSIZE];
static int64_t reg[REGCOUNT];
static int memsize;

static int parse(void)
{
    FILE *f = fopen(FNAME, "r");
    if (!f) {
        fprintf(stderr, "File not found: "FNAME"\n");
        return 0;  // zero lines read
    }
    char buf[16];
    int n = 0;
    int x, y;
    char c;
    while (n < MEMSIZE && fgets(buf, sizeof buf, f)) {
        switch (buf[0]) {
            case 'i':
            case 'd':
                mem[n++] = (Assembunny){buf[0] == 'i' ? INC : DEC, buf[4] - 'a', 0};
                break;
            case 'c':
                if (buf[4] >= 'a') {
                    mem[n++] = (Assembunny){CPY, buf[4] - 'a', buf[6] - 'a'};
                } else {
                    sscanf(&buf[4], "%d %c", &x, &c);
                    mem[n++] = (Assembunny){STO, x, c - 'a'};
                }
                break;
            case 'j':
                if (buf[4] >= 'a') {
                    sscanf(&buf[6], "%d", &y);
                    mem[n++] = (Assembunny){JNZ, buf[4] - 'a', y};
                } else {
                    sscanf(&buf[4], "%d %d", &x, &y);
                    mem[n] = x ? (Assembunny){JMP, n + y, 0} : (Assembunny){NOP, 0, 0};
                    ++n;
                }
                break;
            case 'o':
                mem[n++] = (Assembunny){OUT, buf[4] - 'a', 0};
                break;
        }
    }
    fclose(f);
    // Reverse engineered by looking realll goowd
    mem[3] = (Assembunny){MUL, 0, 0};  // D += B * C
    mem[9] = (Assembunny){DIV, 0, 0};  // B = A % 2; A /= 2
    return n;
}

#ifdef DEBUG
static void list(void)
{
    static const char *ins[] = {"nop", "inc", "dec", "sto", "cpy", "mul", "div", "jmp", "jnz", "out"};
    for (int i = 0; i < memsize; ++i) {
        printf("%2d: %s", i, ins[mem[i].op]);
        switch (mem[i].op) {
            case NOP: /* fall-through */
            case MUL: /* fall-through */
            case DIV: break;
            case INC: /* fall-through */
            case DEC: /* fall-through */
            case OUT: printf(" %c", 'a' + mem[i].x);                    break;
            case STO: printf(" %d %c", mem[i].x, 'a' + mem[i].y);       break;
            case CPY: printf(" %c %c", 'a' + mem[i].x, 'a' + mem[i].x); break;
            case JMP: printf(" %d", mem[i].x);                          break;
            case JNZ: printf(" %c %+d", 'a' + mem[i].x, mem[i].y);      break;
        }
        putchar('\n');
    }
    putchar('\n');
}
#endif

static int run(int first)
{
reset:;
    int ip = 0, good = 0;
    int64_t prev = 1;  // previous clock signal
    memset(reg, 0, sizeof reg);
    reg[0] = first;
    while (ip >= 0 && ip < memsize) {
        switch (mem[ip].op) {
            case NOP:                                        ip++; break;
            case INC: reg[mem[ip++].x]++;                          break;
            case DEC: reg[mem[ip++].x]--;                          break;
            case STO: reg[mem[ip].y] = mem[ip].x;            ip++; break;
            case CPY: reg[mem[ip].y] = reg[mem[ip].x];       ip++; break;
            case MUL: reg[3] += reg[1] * reg[2];          ip += 5; break;
            case DIV: reg[1] = reg[0] & 1; reg[0] >>= 1; ip += 18; break;
            case JMP: ip = mem[ip].x;                              break;
            case JNZ: ip += reg[mem[ip].x] ? mem[ip].y : 1;        break;
            case OUT:
                if ((prev ^ reg[mem[ip].x]) != 1) {
                    ++first;
                    goto reset;
                }
                if (++good == 8)  // min threshold that works for my input = 8
                    return first;
                prev = reg[mem[ip++].x];
                break;
        }
    }
    return 0;
}

int main(void)
{
    memsize = parse();
#ifdef DEBUG
    list();
#endif

#ifdef TIMER
    starttimer();
#endif

    printf("%d\n", run(1));  // 196

#ifdef TIMER
    printf("Time: %.0f ns\n", stoptimer_ns());
#endif
}
