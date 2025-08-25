/**
 * Advent of Code 2021
 * Day 22: Reactor Reboot
 * https://adventofcode.com/2021/day/22
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -Wall -Wextra 22.c
 *    gcc   -std=gnu17 -Wall -Wextra 22.c
 * Enable timer:
 *    clang -DTIMER -O3 -march=native 22.c ../startstoptimer.c
 *    gcc   -DTIMER -O3 -march=native 22.c ../startstoptimer.c
 * Get minimum runtime:
 *     m=999999;for((i=0;i<5000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime:
 *     Macbook Pro 2024 (M4 4.4 GHz)                    :  8.03 ms
 *     Mac Mini 2020 (M1 3.2 GHz)                       :  ?    ms
 *     iMac 2013 (Core i5 Haswell 4570 3.2 GHz)         :  ?    ms
 *     Raspberry Pi 5 (2.4 GHz)                         :  ?    ms
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

static const char *fname = "../aocinput/2021-22-input.txt";
#define L   20  // number of lines in input file for part 1
#define N 4500  // max 4199 for my input
#define M  350  // max  337 for my input
typedef struct {
    int x0, x1, y0, y1, z0, z1, count;
} Cube;
static Cube cube[N] = {0};
static Cube temp[M] = {0};
static int len = 0;  // length of cube[]

// Intersect cube a with b, save result in c if non-empty, return whether non-empty
static bool intersect(const Cube *const a, const Cube *const b, Cube *const c)
{
    if (a->x0 <= b->x1 && a->x1 >= b->x0 &&
        a->y0 <= b->y1 && a->y1 >= b->y0 &&
        a->z0 <= b->z1 && a->z1 >= b->z0) {
        *c = (Cube){
            a->x0 >= b->x0 ? a->x0 : b->x0, a->x1 <= b->x1 ? a->x1 : b->x1,
            a->y0 >= b->y0 ? a->y0 : b->y0, a->y1 <= b->y1 ? a->y1 : b->y1,
            a->z0 >= b->z0 ? a->z0 : b->z0, a->z1 <= b->z1 ? a->z1 : b->z1,
            -a->count};  // opposite count to first cube to neutralise the intersection
        return true;
    }
    return false;
}

// Find cube b in cube counter array a, return pointer to cube in a, or NULL
static Cube *find(Cube *const a, const Cube *const b, const int alen)
{
    int i = 0;
    Cube *c = a;
    for (; i < alen && (
        c->x0 != b->x0 || c->x1 != b->x1 ||
        c->y0 != b->y0 || c->y1 != b->y1 ||
        c->z0 != b->z0 || c->z1 != b->z1); ++i, ++c);
    if (i < alen)
        return c;
    return NULL;
}

// Add one cube b to cube counter array a, return new length of array
static int add(Cube *const a, const Cube *const b, const int alen, const int maxlen)
{
    if (!b->count)               // only add cube with non-zero count
        return alen;
    Cube *c = find(a, b, alen);  // find cube b in cube counter array a
    if (c) {
        c->count += b->count;    // add count to existing cube counter already in array
        return alen;
    }
    if (alen < maxlen) {
        a[alen] = *b;            // append new cube to cube counter array
        return alen + 1;         // length increased by 1
    }
    fprintf(stderr, "Cube counter array overflow while adding (max=%d)\n", maxlen);
    return -1;
}

// Merge two cube counter arrays, return new length of first array
static int merge(Cube *const a, const Cube *const b, const int alen, const int blen, const int maxlen)
{
    int n = alen;                            // start appending from here
    for (int i = 0; i < blen; ++i)
        // n = add(a, &b[i], n, maxlen);     // one-liner if b needs to be checked for duplicate cubes
        // But faster if b contains unique cubes already:
        if (b[i].count) {                    // only merge cubes with non-zero count
            Cube *c = find(a, &b[i], alen);  // only check to old length = assumes b is already unique
            if (c)
                c->count += b[i].count;      // add count to existing cube counter already in array
            else if (n < maxlen)
                a[n++] = b[i];               // append new cube from b to cube counter array a
            else {
                fprintf(stderr, "Cube counter array overflow while merging (max=%d)\n", maxlen);
                return -1;
            }
        }
    return n;
}

// Process instructions from input file, read 'lines' lines or all if lines=0
// Return number of cells that are on
static int64_t read(FILE *f, const int lines)
{
    char buf[4];
    int x0, x1, y0, y1, z0, z1;
    for (int i = 0; (!lines || i < lines) && fscanf(f, "%3s x=%d..%d,y=%d..%d,z=%d..%d ", buf, &x0, &x1, &y0, &y1, &z0, &z1) == 7; ++i) {
        Cube b, a = (Cube){x0, x1, y0, y1, z0, z1, buf[2] == '\0' ? 1 : -1};
        int m = 0;
        for (int j = 0; j < len; ++j)         // check against every existing cube
            if (intersect(&cube[j], &a, &b))  // (b->count is opposite of cube[j].count)
                m = add(temp, &b, m, M);      // save intersection for merge later
        if (a.count == 1)                     // instruction is "on"?
            m = add(temp, &a, m, M);          // also save new cube for merge
        len = merge(cube, temp, len, m, N);   // merge temp array with cube array
    }
    int64_t on = 0;
    Cube *a = cube;
    for (int i = 0; i < len; ++i, ++a)        // increment pointer by sizeof(Cube)
        on += (int64_t)a->count * (a->x1 - a->x0 + 1) * (a->y1 - a->y0 + 1) * (a->z1 - a->z0 + 1);
    return on;
}

int main(void)
{
#ifdef TIMER
    starttimer();
#endif
    FILE *f = fopen(fname, "r");
    printf("Part 1: %lld\n", read(f, L));  // 547648
    printf("Part 2: %lld\n", read(f, 0));  // 1206644425246111
    fclose(f);
#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
    return 0;
}
