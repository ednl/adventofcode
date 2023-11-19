#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define MEMSIZE 64
#define REGCOUNT 4

typedef enum Op {
    NOP, INC, DEC, STO, CPY, JMP, JNZ
} Op;

typedef struct Assembunny {
    Op op;
    int a, b;
} Assembunny;

static Assembunny mem[MEMSIZE];
static int64_t reg[REGCOUNT];
static int memsize, ip;

static int parse(void)
{
    FILE *f = fopen("../aocinput/2016-12-input.txt", "r");
    if (!f)
        return 0;
    char buf[16];
    int i = 0, a, b;
    char c, d;
    while (i < MEMSIZE && fgets(buf, sizeof buf, f)) {
        switch (buf[0]) {
            case 'i':
            case 'd':
                mem[i++] = (Assembunny){buf[0] == 'i' ? INC : DEC, buf[4] - 'a', 0};
                break;
            case 'c':
                if (buf[4] >= 'a') {
                    sscanf(&buf[4], "%c %c", &c, &d);
                    mem[i++] = (Assembunny){CPY, c - 'a', d - 'a'};
                } else {
                    sscanf(&buf[4], "%d %c", &a, &c);
                    mem[i++] = (Assembunny){STO, a, c - 'a'};
                }
                break;
            case 'j':
                if (buf[4] >= 'a') {
                    sscanf(&buf[4], "%c %d", &c, &b);
                    mem[i++] = (Assembunny){JNZ, c - 'a', b - 1};  // compensate -1 for ip incr
                } else {
                    sscanf(&buf[4], "%d %d", &a, &b);
                    if (a)
                        mem[i++] = (Assembunny){JMP, b - 1, 0};  // compensate -1 for ip incr
                    else
                        mem[i++] = (Assembunny){NOP, 0, 0};
                }
                break;
        }
    }
    fclose(f);
    return i;
}

static void list(void)
{
    static const char *ins[] = {"nop", "inc", "dec", "sto", "cpy", "jmp", "jnz"};
    for (int i = 0; i < memsize; ++i) {
        printf("%s ", ins[mem[i].op]);
        switch (mem[i].op) {
            case NOP:                                                  break;
            case INC:
            case DEC: printf("%c", 'a' + mem[i].a);                    break;
            case STO: printf("%d %c", mem[i].a, 'a' + mem[i].b);       break;
            case CPY: printf("%c %c", 'a' + mem[i].a, 'a' + mem[i].b); break;
            case JMP: printf("%d", mem[i].a + 1);                      break;
            case JNZ: printf("%c %d", 'a' + mem[i].a, mem[i].b + 1);   break;
        }
        putchar('\n');
    }

}

static int64_t run(const int regc)
{
    memset(reg, 0, sizeof reg);
    reg[2] = regc;
    ip = 0;
    while (ip < memsize) {
        switch (mem[ip].op) {
            case NOP:                                      break;
            case INC: ++reg[mem[ip].a];                    break;
            case DEC: --reg[mem[ip].a];                    break;
            case STO: reg[mem[ip].b] = mem[ip].a;          break;
            case CPY: reg[mem[ip].b] = reg[mem[ip].a];     break;
            case JMP: ip += mem[ip].a;                     break;
            case JNZ: if (reg[mem[ip].a]) ip += mem[ip].b; break;
        }
        ++ip;
    }
    return reg[0];
}

int main(void)
{
    memsize = parse();
    list();
    printf("Part 1: %lld\n", run(0));  // 318009
    printf("Part 2: %lld\n", run(1));  // 9227663
    return 0;
}
