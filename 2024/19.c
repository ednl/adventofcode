/**
 * Advent of Code 2024
 * Day 19: Linen Layout
 * https://adventofcode.com/2024/day/19
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 19.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 19.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 1870 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : ? µs
 */

#include <stdio.h>
#include <stdlib.h>    // qsort
#include <string.h>    // memset
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRIu64
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2024-19-input.txt"
#define FSIZE (3<<13)  // =24576, needed for my input: 23152 +1
#define TOWELS  447
#define DESIGNS 400
#define COLOURS 5
#define DMAXLEN 60

typedef struct str {
    const char *s;
    int len;
} Str;

typedef struct towelbycolour {
    Str *beg, *end;
} TowelByColour;

static const uint8_t hash[] = {['b' & 31]=0, ['g' & 31]=1, ['r' & 31]=2, ['u' & 31]=3, ['w' & 31]=4};

static char input[FSIZE];
static Str towel[TOWELS];
static Str design[DESIGNS];
static TowelByColour selection[COLOURS];
static int64_t cache[DMAXLEN];

static int colourid(const char c)
{
    return hash[c & 31];
}

// Sort towels by initial colour, then length
static int cmp(const void *p, const void *q)
{
    const Str *a = p, *b = q;
    if (*a->s < *b->s) return -1;
    if (*a->s > *b->s) return 1;
    if (a->len < b->len) return -1;
    if (a->len > b->len) return 1;
    return 0;
}

// Does str start with pre?
// str and pre are null-terminated C strings
static bool startswith(const char *str, const char *pre)
{
    for (; *str && *str == *pre; str++, pre++);
    return *pre == '\0';
}

static int64_t match(const Str dsgn)
{
    if (dsgn.len == 0)
        return 1;
    if (cache[dsgn.len] != -1)
        return cache[dsgn.len];
    int64_t n = 0;
    const TowelByColour *const sel = &selection[colourid(dsgn.s[0])];
    for (const Str *twl = sel->beg; twl != sel->end && twl->len <= dsgn.len; twl++)
        if (startswith(dsgn.s, twl->s))
            n += match((Str){dsgn.s + twl->len, dsgn.len - twl->len});
    cache[dsgn.len] = n;
    return n;
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) { fputs("File not found: "FNAME, stderr); return 1; }
    fread(input, 1, FSIZE, f);  // read single bytes until EOF
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    // Parse
    char *s = input;
    for (Str *t = towel;; *s = '\0', s += 2, t++) {
        t->s = s;
        while (*s & 64)
            s++;
        t->len = s - t->s;
        if (*(s + 1) != ' ')
            break;
    }
    *s = '\0';
    s += 2;
    for (Str *d = design; *s; *s++ = '\0', d++) {
        d->s = s;
        while (*s & 64)
            s++;
        d->len = s - d->s;
    }

    // Index towels by initial colour
    qsort(towel, TOWELS, sizeof *towel, cmp);
    for (int i = 0, j; i < TOWELS; i = j) {
        const char c = towel[i].s[0];
        for (j = i + 1; j < TOWELS && towel[j].s[0] == c; j++);
        selection[colourid(c)] = (TowelByColour){.beg = &towel[i], .end = &towel[j]};
    }

    int part1 = 0;
    int64_t part2 = 0;
    for (int i = 0; i < DESIGNS; i++) {
        memset(cache, -1, sizeof cache);
        const int64_t k = match(design[i]);
        part1 += k > 0;
        part2 += k;
    }
    printf("%u %"PRIu64"\n", part1, part2);  // 300 624802218898092

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f us\n", stoptimer_ms());  // 1000 loops: µs=ns
#endif
}
