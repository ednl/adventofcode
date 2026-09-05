/**
 * Advent of Code 2023
 * Day 5: If You Give A Seed A Fertilizer
 * https://adventofcode.com/2023/day/5
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 05.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 05.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 32.4 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 51.4 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 95.7 µs
 *     iMac 2013 (i5 4570 3.2 GHz)   :    ? µs
 */

#include <stdio.h>
#include <stdlib.h>    // qsort
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRIu64
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2023-05-input.txt"
#define FSIZE 6400  // needed for my input: 6380
#define SEEDS 20    // 20 seeds on first line of input
#define MAPS  7     // translations from cat(n) to cat(n+1)
#define CATS  8     // categories: seed, soil, fertilizer, water, light, temperature, humidity, location
#define CONVS 48    // max 46 conversion ranges per map in my input
#define SSIZE 16    // max stack size (needed for my input: 11)

typedef struct conv {  // a conversion is an offset for range [lo..hi)
    int64_t lo, hi, ofs;
} Conv;
typedef struct map {  // a map is a list of conversions
    Conv conv[CONVS];
    size_t len;  // actual number of conversions used
} Map;
typedef struct range {  // seed range to be converted from mapindex onwards
    size_t mapindex;
    int64_t lo, hi;
} Range;

static char input[FSIZE];
static int64_t seed[SEEDS];
static Map map[MAPS];
static Range stack[SSIZE];
static size_t ssize;

static int64_t min(const int64_t a, const int64_t b)
{
    return a < b ? a : b;
}

static int64_t max(const int64_t a, const int64_t b)
{
    return a > b ? a : b;
}

static int64_t parseint(const char **s)
{
    int64_t x = *(*s)++ & 15;
    while (**s & 16)  // every number followed by space or newline
        x = x * 10 + (*(*s)++ & 15);
    (*s)++;  // skip space or newline
    return x;
}

// Qsort helper: sort by Conv::lo ascending
static int lo_asc(const void *p, const void *q)
{
    const int64_t a = ((const Conv *)p)->lo;
    const int64_t b = ((const Conv *)q)->lo;
    if (a < b) return -1;
    if (a > b) return  1;
    return 0;
}

// Convert a single seed number n to a location number
// by traversing all maps in order.
static int64_t convert1(int64_t n)
{
    const Map *m = map;
    for (int i = 0; i < MAPS; ++m, ++i) {
        const Conv *l = &m->conv[0];
        const Conv *r = &m->conv[m->len - 1];
        while (l <= r) {
            if (n <  l->lo) break;  // outside any range = no conversion
            if (n >= r->hi) break;
            if (n <  l->hi) { n += l->ofs; break; }  // use this conversion
            if (n >= r->lo) { n += r->ofs; break; }
            ++l;
            --r;
        }
    }
    return n;
}

static bool push(const Range r)
{
    // if (ssize == SSIZE) { fputs("Stack overflow.\n", stderr); return false; }
    stack[ssize++] = r;
    return true;
}

static bool pop(Range *const r)
{
    if (!ssize) return false;
    *r = stack[--ssize];
    return true;
}

static int64_t convert2(Range range)
{
    int64_t minloc = INT64_MAX;
    push(range);
    while (pop(&range)) {
        if (range.mapindex == MAPS) {
            minloc = min(minloc, range.lo);
            continue;
        }
        const Map *m = &map[range.mapindex];
        const Conv *l = &m->conv[0];
        const Conv *r = &m->conv[m->len - 1];
        while (l <= r) {
            if (range.lo < l->lo) {
                push((Range){range.mapindex + 1, range.lo, min(range.hi, l->lo)});  // no conversion left of l range
                if ((range.lo = l->lo) >= range.hi) break;  // clip range, check if no more conversion needed
            }
            if (range.hi > r->hi) {
                push((Range){range.mapindex + 1, max(range.lo, r->hi), range.hi});  // no conversion right of r range
                if (range.lo >= (range.hi = r->hi)) break;  // clip range, check if no more conversion needed
            }
            if (range.lo < l->hi) {
                push((Range){range.mapindex + 1, max(range.lo, l->lo) + l->ofs, min(range.hi, l->hi) + l->ofs});  // conversion inside l range
                if ((range.lo = l->hi) >= range.hi) break;  // clip range, check if no more conversion needed
            }
            if (range.hi > r->lo) {
                push((Range){range.mapindex + 1, max(range.lo, r->lo) + r->ofs, min(range.hi, r->hi) + r->ofs});  // conversion inside r range
                if (range.lo >= (range.hi = r->lo)) break;  // clip range, check if no more conversion needed
            }
            ++l;  // remaining range handled in next loop with narrower conversion
            --r;
        }
    }
    return minloc;
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");  // fread requires binary mode
    if (!f) return 1;
    fread(input, 1, FSIZE, f);  // read single bytes until EOF
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
    for (int i = 0; i < MAPS; ++i)
        map[i].len = 0;
#endif

    const char *c = input + 7;  // skip "seeds: "
    for (int i = 0; i < SEEDS; ++i)  // read seed numbers
        seed[i] = parseint(&c);
    for (int i = 0; i < MAPS; ++i) {
        c += 18; // skip shortest map description "\nseed-to-soil map:"
        while (*c != '\n')  // skip longer map descriptions
            c++;
        c++;  // skip newline
        do {
            const int64_t dst = parseint(&c);
            const int64_t src = parseint(&c);
            const int64_t len = parseint(&c);
            map[i].conv[map[i].len++] = (Conv){src, src + len, dst - src};
            qsort(map[i].conv, map[i].len, sizeof (Conv), lo_asc);  // sort to enable l-r search
        } while (*c & 16);  // until newline or null
    }

    int64_t part1 = INT64_MAX;
    for (int i = 0; i < SEEDS; ++i)
        part1 = min(part1, convert1(seed[i]));
    printf("%"PRIu64" ", part1);  // 836040384

    int64_t part2 = INT64_MAX;
    for (int i = 0; i < SEEDS; i += 2)
        part2 = min(part2, convert2((Range){0, seed[i], seed[i] + seed[i + 1]}));
    printf("%"PRIu64"\n", part2);  // 10834440

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
