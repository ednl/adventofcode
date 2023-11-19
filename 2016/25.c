#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define MEMSIZE 64
#define REGCOUNT 4

typedef enum Op {
    NOP, INC, DEC, STO, CPY, JMP, JNZ, OUT
} Op;

typedef struct Assembunny {
    Op op;
    int x, y;
} Assembunny;

static Assembunny mem[MEMSIZE];
static int64_t reg[REGCOUNT];
static int memsize;

static int parse(void)
{
    FILE *f = fopen("../aocinput/2016-25-input.txt", "r");
    if (!f)
        return 0;
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
    return n;
}

static void list(void)
{
    static const char *ins[] = {"nop", "inc", "dec", "sto", "cpy", "jmp", "jnz", "out"};
    for (int i = 0; i < memsize; ++i) {
        printf("%2d: %s", i, ins[mem[i].op]);
        switch (mem[i].op) {
            case NOP:                                                  break;
            case INC:
            case DEC:
            case OUT: printf(" %c", 'a' + mem[i].x);                    break;
            case STO: printf(" %d %c", mem[i].x, 'a' + mem[i].y);       break;
            case CPY: printf(" %c %c", 'a' + mem[i].x, 'a' + mem[i].y); break;
            case JMP: printf(" %d", mem[i].x);                          break;
            case JNZ: printf(" %c %+d", 'a' + mem[i].x, mem[i].y);      break;
        }
        putchar('\n');
    }
    putchar('\n');
}

static int run(int first)
{
reset:;
    int ip = 0, good = 0;
    int64_t prev = 1;  // previous clock signal
    memset(reg, 0, sizeof reg);
    reg[0] = first;
    while (ip >= 0 && ip < memsize) {
        switch (mem[ip].op) {
            case NOP:                                  ++ip; break;
            case INC: ++reg[mem[ip++].x];                    break;
            case DEC: --reg[mem[ip++].x];                    break;
            case STO: reg[mem[ip].y] = mem[ip].x;      ++ip; break;
            case CPY: reg[mem[ip].y] = reg[mem[ip].x]; ++ip; break;
            case JMP: ip = mem[ip].x;                        break;
            case JNZ: ip += reg[mem[ip].x] ? mem[ip].y : 1;  break;
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
    // list();
    printf("Part 1: %d\n", run(1));
    return 0;
}
