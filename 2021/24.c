#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#define REGCOUNT  ('z' - 'w' + 1)
#define STACKSIZE 14
#define MEMSIZE   256

static const char *lang[] = {"nop", "inp", "add", "mul", "div", "mod", "eql"};
typedef enum {
    NOP = 0, INP, ADD, MUL, DIV, MOD, EQL
} OpCode;

typedef struct {
    OpCode op;
    int    arg[2];
    bool   reg;
} Instr;

typedef struct {
    Instr *ip;
    int   *sp;
    int    reg[REGCOUNT];
    int    stack[STACKSIZE];
    Instr  mem[MEMSIZE];
} VM;

static VM vm = {0};

static bool run(int64_t model)
{
    vm.sp = vm.stack;
    for (int i = 0; i < STACKSIZE; ++i) {
        *(vm.sp++) = model % 9 + 1;
        model /= 9;
    }
    for (int i = 0; i < REGCOUNT; ++i)
        vm.reg[i] = 0;
    for (vm.ip = vm.mem;; vm.ip++) {
        int r = vm.ip->arg[0], val = vm.ip->reg ? vm.reg[vm.ip->arg[1]] : vm.ip->arg[1];
        switch (vm.ip->op) {
            case NOP: return vm.reg['z' - 'w'] == 0;
            case INP: vm.reg[r] = *--vm.sp; break;
            case ADD: vm.reg[r] += val; break;
            case MUL: vm.reg[r] *= val; break;
            case DIV: vm.reg[r] /= val; break;
            case MOD: vm.reg[r] %= val; break;
            case EQL: vm.reg[r] = vm.reg[r] == val; break;
        }
    }
}

static int64_t model(int64_t base10)
{
    static const int64_t dec[STACKSIZE] = {1,10,100,1000,10000,100000,1000000,10000000,100000000,1000000000,10000000000,100000000000,1000000000000,10000000000000};
    int64_t n = 0;
    for (int i = 0; i < STACKSIZE; ++i) {
        n += dec[i] * (base10 % 9 + 1);
        base10 /= 9;
    }
    return n;
}

static int64_t base10(int64_t model)
{
    static const int64_t non[STACKSIZE] = {1,9,81,729,6561,59049,531441,4782969,43046721,387420489,3486784401,31381059609,282429536481,2541865828329};
    int64_t n = 0;
    for (int i = 0; i < STACKSIZE; ++i) {
        n += non[i] * (model % 10 - 1);
        model /= 10;
    }
    return n;
}

int main(int argc, char *argv[])
{
    FILE *f = fopen("../aocinput/2021-24-input.txt", "r");
    int c, ip = 0;
    for (; ip < MEMSIZE && (c = fgetc(f)) != EOF; ++ip) {
        printf("%d:", ip);

        OpCode op = NOP;
        int arg0 = 0, arg1 = 0;
        bool reg = false;

        int d = fgetc(f);
        switch (c) {
            case 'a': op = ADD; break;
            case 'd': op = DIV; break;
            case 'e': op = EQL; break;
            case 'i': op = INP; break;
            case 'm': op = d == 'u' ? MUL : MOD; break;
        }
        fgetc(f);
        printf(" %s", lang[op]);

        if ((c = fgetc(f)) == ' ') {  // there's more?
            arg0 = fgetc(f) - 'w';
            printf(" %c", arg0 + 'w');

            if ((c = fgetc(f)) == ' ') {  // there's more?
                c = fgetc(f);
                if (c >= 'w' && c <= 'z') {
                    arg1 = c - 'w';
                    reg = true;
                    printf(" %c", arg1 + 'w');
                    while ((c = fgetc(f)) != '\n');
                } else if (c >= '-' && c <= '9') {
                    char buf[4] = {(char)c};
                    int i = 1;
                    while ((c = fgetc(f)) != '\n')
                        buf[i++] = (char)c;
                    buf[i] = '\0';
                    arg1 = atoi(buf);
                    printf(" %d", arg1);
                }
            }
        }
        vm.mem[ip] = (Instr){op, {arg0, arg1}, reg};
        printf("\n");
    }
    fclose(f);
    if (ip < MEMSIZE) {
        vm.mem[ip].op = NOP;
        printf("%d: %s\n", ip, lang[NOP]);
    }

    int64_t minmodel = 0, maxmodel = 0;
    for (int i = 0; i < STACKSIZE; ++i) {
        minmodel = minmodel * 10 + 1;
        maxmodel = maxmodel * 10 + 9;
    }

    if (argc > 1 && argv && argv[1])
        minmodel = atoll(argv[1]);
    printf("min=%lld\nmax=%lld\n", minmodel, maxmodel);

    // int64_t val = base10(maxmodel) + 1;
    // while (!run(--val));
    // printf("Part 1: %lld\n", model(val));

    int64_t val = base10(minmodel) - 1;
    while (!run(++val));
    printf("Part 2: %lld\n", model(val));
    return 0;
}
