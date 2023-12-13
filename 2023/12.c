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
 *     Apple M1 Mac Mini      :  8 ms
 *     Raspberry Pi 5 2.4 GHz : 18 ms
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
static size_t hashcount, hashsize = 640;  // 640 = max hashcount for my input

// Unique hash value for 0<=ipat<128, 0<=igrp<32
// Return: number in range [0..4096).
static int hashkey(const int ipat, const int igrp)
{
    return (igrp << 7) | ipat;
}

// Searches for key in "hash table".
// Return true if key exists (then also return value via reference).
// WARNING: not a real hash table, just a binary search in sorted table.
static bool hashfind(const int ipat, const int igrp, int64_t* val)
{
    if (!hashcount)  // empty table?
        return false;
    const int key = hashkey(ipat, igrp);
    size_t l = 0, r = hashcount - 1;  // first and last index
    if (key < hashtable[0].key || key > hashtable[r].key)  // outside range of existing keys?
        return false;
    if (key == hashtable[0].key) {  // key at index 0?
        *val = hashtable[0].val;
        return true;
    }
    if (key == hashtable[r].key) {  // key at index r?
        *val = hashtable[r].val;
        return true;
    }
    // Now always true: t[l].k < key < t[r].k
    while (r - l != 1) {
        size_t m = ((l + 1) >> 1) + (r >> 1);    // avoid index overflow
        if (key > hashtable[m].key) l = m;       // proceed with top half
        else if (key < hashtable[m].key) r = m;  // proceed with bottom half
        else {                                   // key exists here
            *val = hashtable[m].val;
            return true;
        }
    }
    return false;  // not found
}

// Insert [key,val] into "hash table" at index, adjusts hashsize and hashcount if necessary.
// Must be true: index <= hashcount, hashsize > 0, hashtable already allocated.
// WARNING: not a real hash table, just a binary search & memmove to make room.
static inline bool hashinsert(size_t index, const int key, const int64_t val)
{
    if (hashcount == hashsize) {  // table full?
        Hashentry* p = realloc(hashtable, (hashsize <<= 1) * sizeof *hashtable);  // double the size
        if (!p)
            return false;
        hashtable = p;
    }
    memmove(hashtable + index + 1, hashtable + index, (hashcount++ - index) * sizeof *hashtable);
    hashtable[index] = (Hashentry){key, val};
    return true;
}

// Insert into sorted "hash table" if needed.
// Return false if failed (or not needed) to insert.
// WARNING: not a real hash table, just a binary search & memmove to make room.
static bool hashsave(const int ipat, const int igrp, const int64_t val)
{
    const int key = hashkey(ipat, igrp);
    if (!hashcount)
        return hashinsert(0, key, val);
    size_t l = 0, r = hashcount - 1;  // first and last index
    if (key == hashtable[0].key || key == hashtable[r].key)
        return false;  // already in table, no need to insert
    if (key < hashtable[0].key)
        return hashinsert(0, key, val);  // insert at start of table
    if (key > hashtable[r].key)
        return hashinsert(r + 1, key, val);  // add to end of table
    // Now always true: t[l].k < key < t[r].k
    while (r - l != 1) {
        size_t m = ((l + 1) >> 1) + (r >> 1);    // avoid index overflow
        if (key > hashtable[m].key) l = m;       // proceed with top half
        else if (key < hashtable[m].key) r = m;  // proceed with bottom half
        else return false;  // already in table, no need to insert
    }
    // Now r-l=1, so insert at index r.
    return hashinsert(r, key, val);  // insert into table
}

// Does group fit from index start?
static inline bool itfits(const int start, const int end, const char* const pat)
{
    for (int i = start + 1; i < end; ++i)
        if (pat[i] == '.')
            return false;
    return pat[end] != '#';
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
        return cache;  // 68513 hits for my input

    // Skipped all operational springs so now start with damaged or unknown spring.
    int64_t count = 0;
    const int end = ipat + row->grp[igrp];  // pattern index after current group
    if (itfits(ipat, end, row->pat))
        count += arrangements(end + 1, igrp + 1, row);  // first spring is (or assume it was) '#'
    if (row->pat[ipat] == '?')
        count += arrangements(ipat + 1, igrp, row);  // assume it was '.' and skip it

    hashsave(ipat, igrp, count);  // save count for new combination of [ipat,igrp]
    return count;
}

static int64_t sumarr(const int n)
{
    const Springs* row = springs;
    int64_t sum = 0, count;
    for (int i = 0; i < n; ++i, ++row) {
        hashcount = 0;  // clear hashtable for every row
        sum += (count = arrangements(0, 0, row));
        #if EXAMPLE || defined(DEBUG)
        printf("%3d: %6lld %6lld %22s (%2d) %d", i, count, sum, row->pat, row->plen, row->grp[0]);
        for (int j = 1; j < row->glen; ++j)
            printf(",%d", row->grp[j]);
        printf(" | %d", row->all[0]);
        for (int j = 1; j < row->glen; ++j)
            printf(",%d", row->all[j]);
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

// Parse input file, return lines read.
static int read(const char* fname)
{
    FILE* f = fopen(fname, "r");
    if (!f)
        return 0;
    char buf[64];
    int i = 0;
    Springs* row = springs;
    while (i < N && fgets(buf, sizeof buf, f)) {
        const char* src = buf;
        char* dst = row->pat;
        int len = 0;
        while (*src != ' ' && len < PLEN - 1) {
            *dst++ = *src++;  // copy pattern
            ++len;
        }
        *dst++ = '.';  // add one operational spring as gap behind last damaged one
        *dst = '\0';
        row->plen = len + 1;
        ++src;  // skip space
        while (*src) {
            int val = 0;
            while (*src >= '0' && *src <= '9')  // until ',' or '\n'
                val = val * 10 + (*src++ & 15);
            row->grp[row->glen++] = val;
            ++src;  // skip ',' or '\n'
        }
        togocount(row);  // helper data to bail earlier from arrangements()
        ++i;
        ++row;
    }
    fclose(f);
    return i;
}

int main(void)
{
    starttimer();
    const int rows = read(NAME);
    if (!rows)
        return 1;

    hashtable = malloc(hashsize * sizeof *hashtable);
    printf("Part 1: %"PRId64"\n", sumarr(rows));  // example: 21, input: 7705

    // Add 4 copies to pattern (with sep='?') and groups
    Springs* row = springs;
    for (int i = 0; i < rows; ++i, ++row) {
        const char* psrc = row->pat;
        const size_t plen = (size_t)(row->plen - 1);  // remove added '.'
        char* pdst = row->pat + plen;

        const int* gsrc = (int*)row->grp;
        const size_t glen = (size_t)row->glen;
        const size_t gsize = glen * sizeof *gsrc;
        int* gdst = (int*)row->grp + glen;

        for (int j = 0; j < 4; ++j, pdst += plen, gdst += glen) {
            *pdst++ = '?';
            memcpy(pdst, psrc, plen);
            memcpy(gdst, gsrc, gsize);
        }
        *pdst++ = '.';  // add back '.' at the end
        *pdst = '\0';

        row->plen = (int)((plen + 1) * 5);  // 5x as long, add 4 separators, add one '.'
        row->glen = (int)(glen * 5);  // 5x as long
        togocount(row);  // new cumulative sum
    }
    printf("Part 2: %"PRId64"\n", sumarr(rows));  // example: 525152, input: 50338344809230
    free(hashtable);

    printf("Time: %.0f ms\n", stoptimer_ms());
    return 0;
}
