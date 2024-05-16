/**
 * Advent of Code 2022
 * Day 21: Monkey Math
 * https://adventofcode.com/2022/day/21
 * By: E. Dronkert https://github.com/ednl
 *
 * Benchmark with the internal timer on a Mac Mini M1 using this Bash oneliner:
 *   m=50000;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * gives a shortest runtime for my input file (not the example) of [TBD] µs.
 * On a Raspberry Pi 4 with the CPU in performance mode: [TBD] µs.
 *   echo performance | sudo tee /sys/devices/system/cpu/cpufreq/policy0/scaling_governor
 *   /boot/config.txt: arm_boost=1, no overclock
 */

#include <stdio.h>
#include <stdlib.h>  // qsort, bsearch
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
    uint32_t id, index;
    union {
        int64_t val;
        struct { uint32_t a, b; };
    };
    char op;
} Monkey;

static Monkey monkey[N];

static unsigned readnum(const char *s)
{
    unsigned n = 0;
    while (*s >= '0' && *s <= '9')
        n = n * 10 + (*s++ & 15);
    return n;
}

static uint32_t hash(const char *s)
{
    return *(const uint32_t *)s;
}

static int cmp_monkies(const void *p, const void *q)
{
    if (((const Monkey *)p)->id < ((const Monkey *)q)->id) return -1;
    if (((const Monkey *)p)->id > ((const Monkey *)q)->id) return  1;
    return 0;
}

static Monkey* getmonkey(const uint32_t id)
{
    const Monkey key = {.id = id};
    return bsearch(&key, monkey, sizeof monkey / sizeof *monkey, sizeof *monkey, cmp_monkies);
}

static void parse(void)
{
    FILE* f = fopen(NAME, "r");
    if (!f) return;
    char line[32];
    const Monkey *const end = &monkey[N];
    for (Monkey *m = monkey; m != end && fgets(line, sizeof line, f); ++m) {
        m->id = hash(line);
        const char *s = line + 6;
        if (*s >= 'a') {
            m->a = hash(s);
            m->b = hash(s + 7);
            m->op = *(s + 5);
        } else {
            m->val = readnum(s);
            m->op = ':';
        }
    }
    fclose(f);
    qsort(monkey, sizeof monkey / sizeof *monkey, sizeof *monkey, cmp_monkies);
#if EXAMPLE
    for (size_t i = 0; i < sizeof monkey / sizeof *monkey; ++i) {
        printf("%3zu  %.4s: ", i + 1, (char *)&monkey[i].id);
        if (monkey[i].op == ':')
            printf("%lld\n", monkey[i].val);
        else
            printf("%.4s %c %.4s\n", (char *)&monkey[i].a, monkey[i].op, (char *)&monkey[i].b);
    }
#endif
    // for (uint32_t i = 0; i < N; ++i)
    //     monkey[i].index = i;
    // for (uint32_t i = 0; i < N; ++i) {
    //     if (monkey[i].op != ':') {
    //         Monkey *m = getmonkey(monkey[i].a);
    //         if (m) monkey[i].a = m->index;
    //         m = getmonkey(monkey[i].b);
    //         if (m) monkey[i].b = m->index;
    //     }
    // }
}

static int64_t getval(const uint32_t id)
{
    Monkey *m = getmonkey(id);
    if (!m) return 0;
    if (m->op == ':') return m->val;
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
    starttimer();
    parse();
    const char *rootname = "root";
    const uint32_t rootid = hash(rootname);
    printf("Part 1: %lld\n", getval(rootid));  // example: 152, input: 21120928600114

    Monkey *const root = getmonkey(rootid);
    root->op = '-';
    const char *humnname = "humn";
    Monkey *const humn = getmonkey(hash(humnname));
    int64_t x0 = humn->val, y0 = getval(rootid);
    int64_t x1 = humn->val = 0, y1 = getval(rootid);
    while (y1) {
        double grad = (double)(y1 - y0) / (x1 - x0);
        int64_t tmp = x1;
        x1 = humn->val = (int64_t)(x0 - y0 / grad);
        x0 = tmp;
        y0 = y1;
        y1 = getval(hash(rootname));
    }
    printf("Part 2: %lld\n", x1);  // example: 301, input: 3453748220116
    printf("Time: %.0f us\n", stoptimer_us());
    return 0;
}
