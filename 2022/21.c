/**
 * Advent of Code 2022
 * Day 21: Monkey Math
 * https://adventofcode.com/2022/day/21
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 21.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 21.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  76 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 230 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 450 µs
 */

#include <stdio.h>
#include <stdlib.h>    // qsort
#include <string.h>    // memcpy to avoid cast align warning
#include <stdint.h>    // int64_t, int32_t
#include <inttypes.h>  // PRId64
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define EXAMPLE 0
#if EXAMPLE == 1
    #define FNAME "../aocinput/2022-21-example.txt"
    #define FSIZE 256  // needed for example: 191 (+ '\0' sentinel)
    #define N 15
#else
    #define FNAME "../aocinput/2022-21-input.txt"
    #define FSIZE 32768  // needed for my input: 30740 (+ '\0' sentinel)
    #define N 2351
#endif

typedef struct monkey {
    int32_t id;
    char op;
    union {
        struct { int32_t a, b; };
        int64_t val;
    };
} Monkey;

static char input[FSIZE];
static Monkey monkey[N];

// Parse positive int followed by newline
// skip number and newline
static int parseint(const char **s)
{
    int x = *(*s)++ & 15;
    while (**s != '\n')
        x = x * 10 + (*(*s)++ & 15);
    (*s)++;  // skip newline
    return x;
}

// Interpret 4 chars as a 32-bit int (LSB or MSB doesn't matter).
static int32_t hash(const char *s)
{
    int32_t h;
    memcpy(&h, s, sizeof h);
    return h;
}

// Look up monkey by id
static int binsearch(const int32_t id)
{
    int l = 0, r = N - 1;
    if (id == monkey[l].id) return l;
    if (id == monkey[r].id) return r;
    // Now always true: monkey[l].id < id < monkey[r].id
    while (r - l > 1) {  // adjacent means not found
        int m = ((l + 1) >> 1) + (r >> 1);  // avoid index overflow
        if      (id > monkey[m].id) l = m;
        else if (id < monkey[m].id) r = m;
        else return m;
    }
    return -1;  // should not happen for this data (famous last words)
}

// Helper for qsort, sort monkies by id
static int cmp_monkies(const void *p, const void *q)
{
    if (((const Monkey *)p)->id < ((const Monkey *)q)->id) return -1;
    if (((const Monkey *)p)->id > ((const Monkey *)q)->id) return  1;
    return 0;
}

static int64_t getval(const int index)
{
    const Monkey *const m = &monkey[index];
    if (m->op == '=') return m->val;
    int64_t a = getval(m->a);
    const int64_t b = getval(m->b);
    switch (m->op) {
        case '*': a *= b; break;
        case '+': a += b; break;
        case '-': a -= b; break;
        case '/': a /= b; break;
    }
    return a;
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    fread(input, 1, FSIZE, f);  // read single bytes until EOF or size=FSIZE
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif


    // Parse input
    Monkey *m = monkey;
    for (const char *c = input; *c; m++) {
        m->id = hash(c);
        c += 6;
        if (*c >= 'a') {
            m->a = hash(c);
            m->b = hash(c + 7);
            m->op = *(c + 5);
            c += 12;
        } else {
            m->val = parseint(&c);  // also skips line
            m->op = '=';
        }
    }

    // Transform monkey array: use direct array index instead of "hash table" id
    qsort(monkey, sizeof monkey / sizeof *monkey, sizeof *monkey, cmp_monkies);
    for (int i = 0; i < N; ++i)
        if (monkey[i].op != '=') {
            monkey[i].a = binsearch(monkey[i].a);
            monkey[i].b = binsearch(monkey[i].b);
        }

    // Part 1
    const int root = binsearch(hash("root"));
    printf("%"PRId64" ", getval(root));  // example: 152, input: 21120928600114

    // Part 2
    monkey[root].op = '-';  // difference should be zero
    int64_t *const humn = &monkey[binsearch(hash("humn"))].val;
    int64_t x0 = *humn, y0 = getval(root);      // first try at humn=<value from input file>
    int64_t x1 = *humn = 0, y1 = getval(root);  // second try at humn=0
    while (y1) {  // "gradient descent" by Newton's method; takes two steps for my input
        *humn = x0 - (int64_t)((double)(x1 - x0) / (y1 - y0) * y0);  // needs fraction or it cycles back & forth
        x0 = x1;
        y0 = y1;
        x1 = *humn;
        y1 = getval(root);
    }
    printf("%"PRId64"\n", x1);  // example: 301, input: 3453748220116

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
