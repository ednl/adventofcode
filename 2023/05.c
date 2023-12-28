/**
 * Advent of Code 2023
 * Day 5: If You Give A Seed A Fertilizer
 * https://adventofcode.com/2023/day/5
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -Ofast -march=native -Wall -Wextra 05.c ../startstoptimer.c
 *    gcc   -std=gnu17 -Ofast -march=native -Wall -Wextra 05.c ../startstoptimer.c
 * Get minimum runtime:
 *     m=999999;for((i=0;i<5000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * Minimum runtime:
 *     Apple M1 Mac Mini 2020 (3.2 GHz)               :  88 µs
 *     Raspberry Pi 5 (2.4 GHz)                       : 141 µs
 *     Apple iMac 2013 (Core i5 Haswell 4570 3.2 GHz) : 174 µs
 *     Raspberry Pi 4 (1.8 GHz)                       : 303 µs
 */

#include <stdio.h>     // fopen, fclose, fscanf, printf
#include <stdlib.h>    // qsort
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRId64
#include <stdbool.h>   // bool
#include "../startstoptimer.h"

// Input parameters
#define EXAMPLE 0
#if EXAMPLE == 1
    #define NAME "../aocinput/2023-05-example.txt"
    #define SEEDS  4
    #define CONVS  5  // max 4 conversion ranges per cat in example
#else
    #define NAME "../aocinput/2023-05-input.txt"
    #define SEEDS 20
    #define CONVS 50  // max 46 conversion ranges per cat in my input
#endif
#define CATS 8  // categories: seed, soil, fertilizer, water, light, temperature, humidity, location
#define MAPS (CATS - 1)  // translation from cat(n) to cat(n+1)
#define SSIZE 16  // max for my input = 11

// Data structures
typedef int64_t i64;
typedef struct Conv {  // a conversion is an offset for range [lo..hi)
    i64 lo, hi, ofs;
} Conv;
typedef struct Map {  // a map is a list of conversions
    Conv conv[CONVS];
    size_t len;  // actual number of conversions used
} Map;
typedef struct range {  // seed range to be converted from mapindex onwards
    size_t mapindex;
    i64 lo, hi;
} Range;

// Global data
static i64 seed[SEEDS];
static Map map[MAPS];
static Range stack[SSIZE];
static size_t ssize;

static i64 min(const i64 a, const i64 b)
{
    return a < b ? a : b;
}

static i64 max(const i64 a, const i64 b)
{
    return a > b ? a : b;
}

// Qsort helper: sort by Conv::lo ascending
static int lo_asc(const void* p, const void* q)
{
    const i64 a = ((const Conv*)p)->lo;
    const i64 b = ((const Conv*)q)->lo;
    if (a < b) return -1;
    if (a > b) return  1;
    return 0;
}

// Convert a single seed number n to a location number
// by traversing all maps in order.
static i64 convert1(i64 n)
{
    const Map* m = map;
    for (int i = 0; i < MAPS; ++i, ++m) {
        const Conv* l = &m->conv[0];
        const Conv* r = &m->conv[m->len - 1];
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
    if (ssize == SSIZE) { fputs("Stack overflow.\n", stderr); return false; }
    stack[ssize++] = r;
    return true;
}

static bool pop(Range* const r)
{
    if (!ssize) return false;
    *r = stack[--ssize];
    return true;
}

static i64 convert2(Range range)
{
    i64 minloc = INT64_MAX;
    push(range);
    while (pop(&range)) {
        if (range.mapindex == MAPS) {
            minloc = min(minloc, range.lo);
            continue;
        }
        const Map* m = &map[range.mapindex];
        const Conv* l = &m->conv[0];
        const Conv* r = &m->conv[m->len - 1];
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
    starttimer();
    FILE* f = fopen(NAME, "r");
    if (!f) { fputs("File not found.\n", stderr); return 1; }

    while (fgetc(f) != ':');  // skip to first seed number
    for (int i = 0; i < SEEDS; ++i)  // read seed numbers
        fscanf(f, "%"PRId64, &seed[i]);
    while (fgetc(f) != ':');  // skip to first conversion of first map
    for (int i = 0; i < MAPS; ++i) {
        i64 dst, src, len;
        while (map[i].len < CONVS && fscanf(f, "%"PRId64" %"PRId64" %"PRId64, &dst, &src, &len) == 3)
            map[i].conv[map[i].len++] = (Conv){src, src + len, dst - src};
        qsort(map[i].conv, map[i].len, sizeof (Conv), lo_asc);  // sort to enable l-r search
        while (!feof(f) && fgetc(f) != ':');  // skip to first conversion of next map
    }
    fclose(f);

    i64 part1 = INT64_MAX;
    for (int i = 0; i < SEEDS; ++i)
        part1 = min(part1, convert1(seed[i]));
    printf("Part 1: %"PRId64"\n", part1);  // example: 35, input: 836040384

    i64 part2 = INT64_MAX;
    for (int i = 0; i < SEEDS; i += 2)
        part2 = min(part2, convert2((Range){0, seed[i], seed[i] + seed[i + 1]}));
    printf("Part 2: %"PRId64"\n", part2);  // example: 46, input: 10834440

    printf("Time: %.0f us\n", stoptimer_us());
    return 0;
}
