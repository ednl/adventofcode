#include <stdio.h>
#include <stdbool.h>

// Maximum size of the program
#define N 1000

// One instruction = opcode, argument, execution order
typedef struct {
    char op[4];
    int arg, ord;
} Instr;

// Virtual Machine and memory size, list of instruction addresses to debug
static Instr vm[N];
static int debug[N], mem;

// Run the VM once
// Returns: accumulator value
// Sets: whether the program looped or ended
static int run(bool *const loop)
{
    int acc = 0, tick = 0, ip = 0;

    for (int i = 0; i < mem; ++i)
        vm[i].ord = 0;
    while (ip != mem && vm[ip].ord == 0) {
        vm[ip].ord = ++tick;
        switch (vm[ip].op[0]) {
            case 'a': acc += vm[ip++].arg; break;
            case 'j': ip  += vm[ip].arg;   break;
            default : ++ip;                break;
        }
    }
    *loop = (ip != mem);
    return acc;
}

// Flip the jmp or nop instruction at address i
// (only the first char is used when executing)
static inline void flip(const int i)
{
    vm[i].op[0] ^= 4;  // j->n, n->j
}

int main(void)
{
    bool loop;

    FILE *fp = fopen("../aocinput/2020-08-input.txt", "r");
    if (!fp)
        return 1;
    // Read the input and parse into the VM, set mem size
    while (mem < N && fscanf(fp, "%s %d ", vm[mem].op, &vm[mem].arg) == 2)
        ++mem;
    fclose(fp);
    printf("Part 1: %d\n", run(&loop));  // 1331

    // Save addresses of jmp and nop instructions that got executed
    int k = 0;
    for (int i = 0; i < mem; ++i)
        if (vm[i].ord && (vm[i].op[0] & 8))  // j or n, not a
            debug[k++] = i;

    // Run the VM once with every debug instruction flipped
    // until the program ends (instead of loops)
    int acc = 0;
    while (loop && k--) {
        flip(debug[k]);
        acc = run(&loop);
        flip(debug[k]);
    }
    printf("Part 2: %d\n", acc);  // 1121
    return 0;
}
