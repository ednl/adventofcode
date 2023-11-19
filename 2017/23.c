#include <stdio.h>     // printf
#include <stdlib.h>    // malloc, free
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRIi64
#include <string.h>    // strtok, memcpy
#include <stdbool.h>

#define MEMSIZE (32)   // number of lines in input
#define REGBASE ('a')
#define REGSTOP ('h')
#define REGSIZE (REGSTOP - REGBASE + 1)

typedef enum opcode {
    SET,
    SUB,
    MUL,
    JNZ,
    JMP,
    NOP,
} OpCode;

typedef struct dict {
    OpCode id;
    char  *name;
} Dict, *pDict;

static const Dict cmd[] = {
    { .id = SET, .name = "set" },
    { .id = SUB, .name = "sub" },
    { .id = MUL, .name = "mul" },
    { .id = JNZ, .name = "jnz" },
    { .id = JMP, .name = "jmp" },
    { .id = NOP, .name = "nop" },
};
static const int cmdsize = sizeof cmd / sizeof *cmd;

typedef struct instr {
    OpCode  op;
    int     r0, r1;
    int64_t v0, v1;
} Instr, *pInstr;

typedef struct prog {
    int      ip;
    int64_t  tick;
    int64_t  reg[REGSIZE];
    Instr    mem[MEMSIZE];
} Prog, *pProg;

static inline size_t name2index(const char * s)
{
    for (size_t i = 0; i < cmdsize; ++i) {
        if (!strcmp(cmd[i].name, s)) {
            return i;
        }
    }
    return 0;
}

static inline size_t id2index(const OpCode id)
{
    if (cmd[id].id == id) {
        return (size_t)id;
    }
    for (size_t i = 0; i < cmdsize; ++i) {
        if (cmd[i].id == id) {
            return i;
        }
    }
    return 0;
}

static inline Instr assemble(char *code)
{
    Instr ins = (Instr){
        .op = NOP,
        .r0 = -1, .r1 = -1,
        .v0 =  0, .v1 =  0,
    };
    int field = 0;
    char *s = strtok(code, " ");
    while (s) {
        if (field == 0) {
            size_t i = name2index(s);
            ins.op = cmd[i].id;
        } else if (field == 1) {
            if (*s >= REGBASE && *s <= REGSTOP) {
                ins.r0 = *s - REGBASE;
            } else {
                ins.v0 = atoll(s);  // leave r0 = -1
                if (ins.op == JNZ) {
                    ins.op = ins.v0 == 0 ? NOP : JMP;
                }
            }
        } else if (field == 2) {
            if (*s >= REGBASE && *s <= REGSTOP) {
                ins.r1 = *s - REGBASE;
            } else {
                ins.v1 = atoll(s);  // leave r1 = -1
            }
        }
        ++field;
        s = strtok(NULL, " ");
    }
    return ins;
}

// Init & load program p
static void load(pProg p)
{
    // Runtime
    p->ip     = 0;
    p->tick   = 0;

    // Registers
    for (size_t i = 0; i < REGSIZE; ++i) {
        p->reg[i] = 0;
    }

    // Program
    FILE *f = fopen("../aocinput/2017-23-input.txt", "r");
    if (f == NULL) {
        fprintf(stderr, "File not found");
        exit(1);
    }
    char *buf = NULL;
    size_t buflen = 0, line = 0;
    ssize_t len;
    while ((len = getline(&buf, &buflen, f)) != -1 && line < MEMSIZE) {
        p->mem[line++] = assemble(buf);
    }
    fclose(f);
    free(buf);
}

static void print(pProg p, bool listing)
{
    int i;
    int64_t k;

    // Runtime parameters
    printf("IP   : %i\n", p->ip);
    printf("Tick : %"PRId64"\n", p->tick);
    for (i = 0; i < REGSIZE; ++i) {
        if ((k = p->reg[i]) != 0) {
            printf("Rg %c : %"PRId64"\n", REGBASE + i, k);
        }
    }

    // Program listing
    if (listing) {
        for (i = 0; i < MEMSIZE; ++i) {
            size_t j = id2index(p->mem[i].op);
            printf("  %2i: %s", i, cmd[j].name);
            if (p->mem[i].r0 != -1) {
                printf(" %c", REGBASE + p->mem[i].r0);
            } else {
                printf(" %"PRId64, p->mem[i].v0);
            }
            printf("\n");
        }
    }
    printf("\n");
}

// Run program p, use program q as message queue
static int64_t run(pProg p, bool ispart1)
{
    int64_t mulcount = 0;
    while (p->ip >= 0 && p->ip < MEMSIZE) {
        if (!ispart1) {
            for (int i = 'a'; i <= 'h'; ++i) {
                printf(" %8"PRId64, p->reg[i - REGBASE]);
            }
            printf("\n");
        }
        pInstr i = &(p->mem[p->ip]);
        if (i->r0 != -1) {
            i->v0 = p->reg[i->r0];
        }
        if (i->r1 != -1) {
            i->v1 = p->reg[i->r1];
        }
        switch (i->op) {
        case SET:
            p->reg[i->r0] = i->v1;
            break;
        case SUB:
            p->reg[i->r0] -= i->v1;
            break;
        case MUL:
            p->reg[i->r0] *= i->v1;
            ++mulcount;
            break;
        case JNZ:
            if (i->v0 != 0) {
                p->ip += i->v1;
                p->tick++;
                continue;  // skip updates at end of loop
            }
            break;
        case JMP:
            p->ip += i->v1;
            p->tick++;
            continue;  // skip updates at end of loop
        case NOP:
            break;
        }
        p->ip++;
        p->tick++;
    }
    return ispart1 ? mulcount : p->reg['h' - REGBASE];
}

int main(void)
{
    Prog p;

    // Part 1
    load(&p);
    printf("Part 1: %"PRId64"\n", run(&p, true));

    // Part 2
    p.ip = 0;
    p.tick = 0;
    p.reg[0] = 1;
    for (size_t i = 1; i < REGSIZE; ++i) {
        p.reg[i] = 0;
    }
    print(&p, false);
    printf("Part 2: %"PRId64"\n", run(&p, false));

    return 0;
}
