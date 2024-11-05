#include <stdio.h>     // printf
#include <stdlib.h>    // malloc, free
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRIi64
#include <string.h>    // strtok, memcpy

#define MEMSIZE (41)
#define REGBASE ('a')
#define REGSTOP ('z')
#define REGSIZE (REGSTOP - REGBASE + 1)

typedef enum opcode {
    SET,
    ADD,
    MUL,
    MOD,
    JGZ,
    SND,
    RCV,
    NOP,  // unknown/invalid opcode
} OpCode;

typedef struct dict {
    OpCode id;
    char  *name;
} Dict, *pDict;

static const Dict cmd[] = {
    { .id = SET, .name = "set" },
    { .id = ADD, .name = "add" },
    { .id = MUL, .name = "mul" },
    { .id = MOD, .name = "mod" },
    { .id = JGZ, .name = "jgz" },
    { .id = SND, .name = "snd" },
    { .id = RCV, .name = "rcv" },
};
static const int cmdsize = sizeof cmd / sizeof *cmd;

typedef struct instr {
    OpCode  op;
    int     r0, r1;
    int64_t v0, v1;
} Instr, *pInstr;

typedef struct list List, *pList;
struct list {
    int64_t val;
    pList   next;
};

typedef struct prog {
    int     pid, ip, tick, sent;
    pList   mqhead, mqtail;
    int64_t reg[REGSIZE];
    Instr   mem[MEMSIZE];
} Prog, *pProg;

static inline OpCode name2id(const char * s)
{
    for (size_t i = 0; i < cmdsize; ++i) {
        if (!strcmp(cmd[i].name, s)) {
            return cmd[i].id;
        }
    }
    return NOP;
}

static inline char * id2name(const OpCode id)
{
    if (cmd[id].id == id) {
        return cmd[id].name;
    }
    for (size_t i = 0; i < cmdsize; ++i) {
        if (cmd[i].id == id) {
            return cmd[i].name;
        }
    }
    return NULL;
}

static inline Instr assemble(char *code)
{
    Instr ins = {NOP, -1, -1, 0, 0};  // if reg == -1 => use val
    int field = 0;
    char *s = strtok(code, " ");
    while (s) {
        if (field == 0) {
            ins.op = name2id(s);
        } else if (field == 1) {
            if (*s >= REGBASE && *s <= REGSTOP) {
                ins.r0 = *s - REGBASE;
            } else {
                ins.v0 = atoll(s);  // leave r0 = -1
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

// Init & load program p, use program q as source if provided
static void load(pProg p, pProg q)
{
    // Runtime
    p->pid    = 0;
    p->ip     = 0;
    p->tick   = 0;
    p->sent   = 0;
    p->mqhead = NULL;
    p->mqtail = NULL;

    // Registers
    for (int i = 0; i < REGSIZE; ++i) {
        p->reg[i] = 0;
    }

    // Program
    if (q) {
        p->pid = 1;
        p->reg['p' - REGBASE] = 1;
        memcpy(p->mem, q->mem, sizeof(q->mem));
    } else {
        FILE *f = fopen("../aocinput/2017-18-input.txt", "r");
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
}

static void print(pProg p, int listing)
{
    int i;
    int64_t k;
    pList mq;

    // Runtime parameters
    printf("PID  : %i\n", p->pid);
    printf("IP   : %i\n", p->ip);
    printf("Tick : %i\n", p->tick);
    printf("Sent : %i\n", p->sent);
    for (i = 0; i < REGSIZE; ++i) {
        if ((k = p->reg[i]) != 0) {
            printf("Rg %c : %"PRIi64"\n", REGBASE + i, k);
        }
    }

    // Message queue
    printf("MsgQ :");
    i = 0;
    mq = p->mqhead;
    while (mq) {
        printf(" %"PRIi64, mq->val);
        mq = mq->next;
        ++i;
    }
    printf(" (len=%i)\n", i);

    // Program listing
    if (listing) {
        for (i = 0; i < MEMSIZE; ++i) {
            printf("  %2i: ", i);
            if (p->mem[i].op != NOP) {
                printf("%s", id2name(p->mem[i].op));
                if (p->mem[i].r0 != -1) {
                    printf(" %c", REGBASE + p->mem[i].r0);
                } else {
                    printf(" %"PRIi64, p->mem[i].v0);
                }
                if (p->mem[i].op != SND && p->mem[i].op != RCV) {
                    if (p->mem[i].r1 != -1) {
                        printf(" %c", REGBASE + p->mem[i].r1);
                    } else {
                        printf(" %"PRIi64, p->mem[i].v1);
                    }
                }
                printf("\n");
            } else {
                printf("?\n");
            }
        }
    }
    printf("\n");
}

// Run program p, use program q as message queue
static void run(pProg p, pProg q, int part)
{
    pList mq;
    while (p->ip >= 0 && p->ip < MEMSIZE) {
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
        case ADD:
            p->reg[i->r0] += i->v1;
            break;
        case MUL:
            p->reg[i->r0] *= i->v1;
            break;
        case MOD:
            p->reg[i->r0] %= i->v1;
            break;
        case JGZ:
            if (i->v0 > 0) {
                p->ip += i->v1;
                p->tick++;
                continue;  // skip updates at end of loop
            }
            break;
        case SND:
            if ((mq = (pList)malloc(sizeof(List)))) {
                mq->val = i->v0;
                mq->next = NULL;
                if (p->mqhead) {  // only check head because tail is never reset to NULL
                    p->mqtail->next = mq;
                } else {
                    p->mqhead = mq;
                }
                p->mqtail = mq;
            }
            p->sent++;
            break;
        case RCV:
            switch (part) {
            case 1:
                if (i->v0) {
                    return;
                }
                break;
            case 2:
                if ((mq = q->mqhead)) {
                    p->reg[i->r0] = mq->val;
                    q->mqhead = mq->next;  // NB: tail is never reset to NULL
                    free(mq);
                } else {
                    return;
                }
                break;
            }
            break;
        case NOP:
            break;
        }
        p->ip++;
        p->tick++;
    }
}

int main(void)
{
    Prog p, q;

    // Part 1
    load(&p, NULL);
    run(&p, NULL, 1);
    print(&p, 0);
    printf("Part 1: %"PRIi64"\n\n", p.mqtail->val);

    // Part 2
    load(&q, &p);
    while (p.mqhead) {
        run(&q, &p, 2);
        run(&p, &q, 2);
    }
    print(&p, 0);
    print(&q, 0);
    printf("Part 2: %i\n", q.sent);

    // Clean up (should not be necessary)
    while (q.mqhead) {
        pList t = q.mqhead->next;
        free(q.mqhead);
        q.mqhead = t;
    }
    return 0;
}
