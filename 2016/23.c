#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define MEMSIZE 64
#define REGCOUNT 4

typedef enum Op {
    NOP, INC, DEC, CPY, JNZ, TGL
} Op;

typedef enum Mode {
    NONE, IMM, REG
} Mode;

typedef struct Assembunny {
    Op op;
    int p[2];
    Mode m[2];
} Assembunny;

static Assembunny mem[MEMSIZE], bak[MEMSIZE];
static int64_t reg[REGCOUNT];
static int memsize, ip;

static int parse(void)
{
    FILE *f = fopen("../aocinput/2016-23-input.txt", "r");
    if (!f)
        return 0;
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

static void list(const Assembunny * const bank, const int count)
{
    static const char *ins[] = {"nop", "inc", "dec", "cpy", "jnz", "tgl"};
    puts("----------------");
    for (int i = 0; i < count; ++i) {
        printf("%s", ins[bank[i].op]);
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

static bool ix(const int i)
{
    return i >= 0 && i < REGCOUNT;
}

static bool mode(const Assembunny * const a, const Mode mx, const Mode my)
{
    return a->m[0] == mx && a->m[1] == my && (mx != REG || ix(a->p[0])) && (my != REG || ix(a->p[1]));
}

static int64_t run(const int rega)
{
    int64_t t;
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
                t = -1;
                if (mode(a, REG, NONE))
                    t = ip + reg[a->p[0]];
                else if (mode(a, IMM, NONE))
                    t = ip + a->p[0];
                if (t >= 0 && t < memsize) {
                    // putchar('.');  // part 1: 4x, part 2: 4x
                    if (mem[t].m[1] == NONE)
                        mem[t].op = mem[t].op == INC ? DEC : INC;
                    else
                        mem[t].op = mem[t].op == JNZ ? CPY : JNZ;
                } // else
                    // putchar('!');  // part 1: 1x, part 2: 6x
                ++ip;
                break;
        }
    }
    return reg[0];
}

int main(void)
{
    memsize = parse();
    list(bak, memsize);
    printf("Part 1: %lld\n", run(7));  // 11424
    list(mem, memsize);
    printf("Part 2: %lld\n", run(12));  // 479007984
    list(mem, memsize);
    return 0;
}
