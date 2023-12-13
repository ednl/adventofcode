/**
 * Advent of Code 2023
 * Day 12: Hot Springs
 * https://adventofcode.com/2023/day/12
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -Ofast -march-native -Wall 12.c ../startstoptimer.c
 *    gcc   -std=gnu17 -Ofast -march-native -Wall 12.c ../startstoptimer.c
 * Get minimum runtime:
 *     m=50000;for((i=0;i<1000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * Minimum runtime:
 *     Apple M1 Mac Mini      : 12 ms
 *     Raspberry Pi 5 2.4 GHz : ?? ms
 */

#include <stdio.h>     // fopen, fclose, fgets, printf
#include <stdlib.h>    // malloc, free
#include <string.h>    // memmove, memcpy
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRId64
#include <stdbool.h>   // bool
#include "../startstoptimer.h"

#define EXAMPLE 0
#if EXAMPLE
#define NAME "../aocinput/2023-12-example.txt"
#define N 6
#else
#define NAME "../aocinput/2023-12-input.txt"
#define N 1000
#endif
#define PLEN 128  // max needed 20+1+1, 100+4+1+1
#define GLEN 32   // max needed 6, 30

typedef struct springs {
    char pat[PLEN];
    int  grp[GLEN], all[GLEN];
    int  plen, glen;
} Springs;

typedef struct hashentry {
    int64_t key, val;
} Hashentry;

static Springs springs[N];
static Hashentry* hashtable;
static size_t hashcount = 0, hashsize = 1024;

static int hashkey(const int ipat, const int igrp)
{
    return (igrp << 8) | ipat;
}

static bool hashfind(const int ipat, const int igrp, int64_t* value)
{
    if (!hashcount)
        return false;
    const int key = hashkey(ipat, igrp);
    size_t l = 0, r = hashcount - 1;
    if (key < hashtable[0].key || key > hashtable[r].key)
        return false;
    if (key == hashtable[0].key) {
        *value = hashtable[0].val;
        return true;
    }
    if (key == hashtable[r].key) {
        *value = hashtable[r].val;
        return true;
    }
    // Now always true: h[l] < key < h[r]
    while (r - l != 1) {
        size_t m = ((l + 1) >> 1) + (r >> 1);  // avoid index overflow
        if (key > hashtable[m].key) l = m;
        else if (key < hashtable[m].key) r = m;
        else {
            *value = hashtable[m].val;
            return true;
        }
    }
    return false;
}

// Insert into sorted hash table with index array.
// Return false if failed (or not needed) to insert.
static bool hashinsert(const int ipat, const int igrp, const int64_t value)
{
    const int key = hashkey(ipat, igrp);
    if (!hashcount) {
        hashtable[0] = (Hashentry){key, value};
        hashcount = 1;
        return true;
    }
    size_t l = 0, r = hashcount - 1;
    if (key == hashtable[0].key || key == hashtable[r].key)
        return false;
    if (hashcount == hashsize) {
        Hashentry* p = realloc(hashtable, (hashsize <<= 1) * sizeof *hashtable);
        if (!p)
            return false;
        hashtable = p;
    }
    if (key < hashtable[0].key) {
        memmove(hashtable + 1, hashtable, hashcount++ * sizeof *hashtable);
        hashtable[0] = (Hashentry){key, value};
        return true;
    }
    if (key > hashtable[r].key) {
        hashtable[hashcount++] = (Hashentry){key, value};
        return true;
    }
    // Now always true: h[l] < key < h[r]
    while (r - l != 1) {
        size_t m = ((l + 1) >> 1) + (r >> 1);  // avoid index overflow
        if (key > hashtable[m].key) l = m;
        else if (key < hashtable[m].key) r = m;
        else return false;
    }
    memmove(hashtable + r + 1, hashtable + r, (hashcount++ - r) * sizeof *hashtable);
    hashtable[r] = (Hashentry){key, value};
    return true;
}

static int64_t arrangements(int ipat, int igrp, const Springs* const row)
{
    if (igrp >= row->glen) {  // no more groups
        for (int i = ipat; i < row->plen; ++i)
            if (row->pat[i] == '#')  // stil a damaged spring to allocate?
                return 0;
        return 1;  // all good
    }

    // Skip all operational springs
    while (ipat < row->plen && row->pat[ipat] == '.')
        ++ipat;

    // Enough pattern for all groups?
    if (ipat + row->all[igrp] > row->plen)
        return 0;

    int64_t cache;
    if (hashfind(ipat, igrp, &cache))
        return cache;

    // Start with damaged or unknown spring
    // If unknown, first assume it's operational
    int64_t count = row->pat[ipat] == '#' ? 0 : arrangements(ipat + 1, igrp, row);

    // Remember that, now it's damaged or assume it is
    const int end = ipat + row->grp[igrp];  // index 1 past current group
    for (int i = ipat + 1; i < end; ++i)
        if (row->pat[i] == '.')
            goto nofit;
    if (row->pat[end] == '#')
        goto nofit;
    count += arrangements(end + 1, igrp + 1, row);  // group fits here
nofit:
    hashinsert(ipat, igrp, count);
    return count;
}

static int64_t sumarr(const int n)
{
    int64_t sum = 0;
    for (int i = 0; i < n; ++i) {
        hashcount = 0;  // clear hashtable for every row because they are different
        const int64_t count = arrangements(0, 0, &springs[i]);
        sum += count;
        #if EXAMPLE || defined(DEBUG)
        printf("%3d: %6lld %6lld %22s (%2d) %d", i, count, sum, springs[i].pat, springs[i].plen, springs[i].grp[0]);
        for (int j = 1; j < springs[i].glen; ++j)
            printf(",%d", springs[i].grp[j]);
        printf(" | %d", springs[i].all[0]);
        for (int j = 1; j < springs[i].glen; ++j)
            printf(",%d", springs[i].all[j]);
        printf("\n");
        #endif
    }
    return sum;
}

// Backwards cumulative sum of group lengths = how much to go
static void togocount(Springs* const row)
{
    for (int i = row->glen - 1, prev = 0; i >= 0; --i)
        prev = (row->all[i] = row->grp[i] + prev + 1);  // +1 = group separator
}

static int read(const char* fname)
{
    FILE* f = fopen(fname, "r");
    if (!f)
        return 0;
    int n = 0;
    char buf[64];
    while (n < N && fgets(buf, sizeof buf, f)) {
        const char* src = buf;
        char* dst = springs[n].pat;
        int len = 0;
        while (*src != ' ' && len < PLEN - 1) {
            *dst++ = *src++;  // copy pattern
            ++len;
        }
        *dst++ = '.';  // add one operational spring as gap behind last damaged one
        *dst = '\0';
        springs[n].plen = len + 1;
        ++src;  // skip space
        while (*src) {
            int val = 0;
            while (*src >= '0' && *src <= '9')  // until ',' or '\n'
                val = val * 10 + (*src++ & 15);
            springs[n].grp[springs[n].glen++] = val;
            ++src;  // skip ',' or '\n'
        }
        togocount(&springs[n]);  // helper data to bail earlier from arrangements()
        ++n;
    }
    fclose(f);
    return n;
}

int main(void)
{
    starttimer();
    const int rows = read(NAME);
    if (!rows)
        return 1;

    hashtable = malloc(hashsize * sizeof *hashtable);
    printf("Part 1: %"PRId64"\n", sumarr(rows));  // example: 21, input: 7705

    // Add 4 copies to pattern (sep='?') and groups
    for (int i = 0; i < rows; ++i) {
        const char* psrc = springs[i].pat;
        const size_t plen = (size_t)(springs[i].plen - 1);  // remove added '.'
        char* pdst = springs[i].pat + plen;

        const int* gsrc = (int*)springs[i].grp;
        const size_t glen = (size_t)springs[i].glen;
        const size_t gsize = glen * sizeof *gsrc;
        int* gdst = (int*)springs[i].grp + glen;

        for (int j = 0; j < 4; ++j, pdst += plen, gdst += glen) {
            *pdst++ = '?';
            memcpy(pdst, psrc, plen);
            memcpy(gdst, gsrc, gsize);
        }
        *pdst++ = '.';  // add back '.' at the end
        *pdst = '\0';

        springs[i].plen = (int)((plen + 1) * 5);  // 5x as long, add 4 separators, add one '.'
        springs[i].glen = (int)(glen * 5);  // 5x as long
        togocount(&springs[i]);  // new cumulative sum
    }
    printf("Part 2: %"PRId64"\n", sumarr(rows));  // example: 525152, input: 50338344809230
    free(hashtable);

    printf("Time: %.0f ms\n", stoptimer_ms());
    return 0;
}
