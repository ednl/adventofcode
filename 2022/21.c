/**
 * Advent of Code 2022
 * Day 21: Monkey Math
 * https://adventofcode.com/2022/day/21
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -Ofast -march=native -Wall -Wextra 21.c ../startstoptimer.c
 *    gcc   -std=gnu17 -Ofast -march=native -Wall -Wextra 21.c ../startstoptimer.c
 * Get minimum runtime:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * Minimum runtime:
 *     Mac Mini 2020 (M1 3.2 GHz)          :   ? µs
 *     Raspberry Pi 5 (2.4 GHz)            :   ? µs
 *     iMac 2013 (i5 Haswell 4570 3.2 GHz) :  99 µs
 *     Raspberry Pi 4 (1.8 GHz)            :   ? µs
 */

#include <stdio.h>
#include <stdlib.h>  // qsort
#include <stdint.h>
#include "../startstoptimer.h"

#define EXAMPLE 0
#if EXAMPLE == 1
#define NAME "../aocinput/2022-21-example.txt"
#define N 15
#else
#define NAME "../aocinput/2022-21-input.txt"
#define N 2351
#endif

typedef struct monkey {
    int32_t id;
    union {
        int64_t val;
        struct { int32_t a, b; };
    };
    char op;
} Monkey;

static Monkey monkey[N];

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

// Much faster than atoi() but only valid for this data because every number is followed by a newline.
// Slightly more general but still dependent on ASCII: while ((*s & 0xf0) == 0x30) ...;
// Best and still faster: while (*s >= '0' && *s <= '9') n = n*10 + (*s++ - '0');
static int readnum(const char *s)
{
    int n = 0;
    while (*s != '\n')
        n = n * 10 + (*s++ & 15);
    return n;
}

// Interpret 4 chars as a 32-bit int (LSB or MSB doesn't matter).
// Compile with -Wno-cast-align to avoid warning.
static int32_t hash(const char *s)
{
    return *(const int32_t *)s;
}

static int cmp_monkies(const void *p, const void *q)
{
    if (((const Monkey *)p)->id < ((const Monkey *)q)->id) return -1;
    if (((const Monkey *)p)->id > ((const Monkey *)q)->id) return  1;
    return 0;
}

static void parse(void)
{
    FILE* f = fopen(NAME, "r");
    if (!f) return;
    char line[32];
    Monkey *m = monkey;
    for (int i = 0; i < N && fgets(line, sizeof line, f); ++i, ++m) {
        m->id = hash(line);
        const char *s = line + 6;
        if (*s >= 'a') {
            m->a = hash(s);
            m->b = hash(s + 7);
            m->op = *(s + 5);
        } else {
            m->val = readnum(s);
            m->op = '=';
        }
    }
    fclose(f);
    qsort(monkey, sizeof monkey / sizeof *monkey, sizeof *monkey, cmp_monkies);
    for (int i = 0; i < N; ++i) {
        if (monkey[i].op != '=') {
            monkey[i].a = binsearch(monkey[i].a);
            monkey[i].b = binsearch(monkey[i].b);
        }
    }
}

static int64_t getval(const int index)
{
    const Monkey *const m = &monkey[index];
    if (m->op == '=') return m->val;
    int64_t a = getval(m->a);
    const int64_t b = getval(m->b);
    switch (m->op) {
        case '+': a += b; break;
        case '-': a -= b; break;
        case '*': a *= b; break;
        case '/': a /= b; break;
    }
    return a;
}

int main(void)
{
    parse();
    starttimer();

    const char *rootname = "root";
    const int32_t rootid = hash(rootname);
    const int rootindex = binsearch(rootid);
    printf("Part 1: %lld\n", getval(rootindex));  // example: 152, input: 21120928600114

    monkey[rootindex].op = '-';
    const char *humnname = "humn";
    const int32_t humnid = hash(humnname);
    const int humnindex = binsearch(humnid);
    int64_t x0 = monkey[humnindex].val, y0 = getval(rootindex);
    int64_t x1 = monkey[humnindex].val = 0, y1 = getval(rootindex);
    while (y1) {
        int64_t tmp = x1;
        x1 = monkey[humnindex].val = (int64_t)(x0 - y0 * ((double)(x1 - x0) / (y1 - y0)));
        x0 = tmp;
        y0 = y1;
        y1 = getval(rootindex);
    }
    printf("Part 2: %lld\n", x1);  // example: 301, input: 3453748220116

    printf("Time: %.0f us\n", stoptimer_us());
    return 0;
}
