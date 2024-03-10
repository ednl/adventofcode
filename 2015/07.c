#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define ORD(X) ((unsigned)((X) - 'a' + 1))  // 'a'=1, 'z'=26
#define A ORD('z')                          // 26 letters
#define N (A * ORD('m') + ORD('a') + 1u)    // 26 * 13 + 1 + 1 = 340 (max wire name "ma"=339)

typedef enum func {
    EQ, NOT, AND, OR, SHL, SHR
} Func;

typedef struct wire {
    Func fun;
    unsigned x[2];
    bool iswire[2];
} Wire;

static Wire wire[N];

// a..z = 1..26
// aa..az = 27..52
// ba=53, ma=339, zz=702
static unsigned namehash(const char* s)
{
    unsigned id = 0;
    while (*s >= 'a' && *s <= 'z')
        id = id * A + ORD(*s++);
    return id;
}

static unsigned intval(const char* s)
{
    unsigned n = 0;
    while (*s >= '0' && *s <= '9')
        n = n * 10 + (unsigned)(*s++ - '0');
    return n;
}

static unsigned eval(unsigned index)
{
    Wire* const w = &wire[index];
    if (w->fun == EQ && !w->iswire[0])
        return w->x[0];
    for (unsigned i = 0; i < 2; ++i)
        if (w->iswire[i]) {
            w->x[i] = eval(w->x[i]);
            w->iswire[i] = false;
        }
    unsigned val = w->x[0];
    switch (w->fun) {
        case EQ : return val;
        case NOT: val = ~val;      break;
        case AND: val &=  w->x[1]; break;
        case OR : val |=  w->x[1]; break;
        case SHL: val <<= w->x[1]; break;
        case SHR: val >>= w->x[1]; break;
    }
    w->fun = EQ;
    w->x[0] = val;
    return val;
}

int main(void)
{
    FILE* f = fopen("../aocinput/2015-07-input.txt", "r");
    if (!f) return 1;
    char line[32];
    while (fgets(line, sizeof line, f)) {
        printf("%s", line);
        //TODO
    }
    fclose(f);
    return 0;
}
