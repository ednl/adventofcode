/**
 * Advent of Code 2023
 * Day 12:
 * https://adventofcode.com/2023/day/12
 * By: E. Dronkert https://github.com/ednl
 */

#include <stdio.h>     // fopen, fclose, fgets, printf
#include <stdlib.h>    // malloc, free
#include <string.h>    // memmove
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRId64
#include <stdbool.h>   // bool

#define EXAMPLE 0
#if EXAMPLE
#define NAME "../aocinput/2023-12-example.txt"
#define N 6
#else
#define NAME "../aocinput/2023-12-input.txt"
#define N 1000
#endif
#define PLEN 128
#define GLEN 32

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
    if (hashfind(ipat, igrp, &cache)) {
        // printf("%d,%d=%d <- %lld\n", ipat, igrp, hashkey(ipat, igrp), cache);
        return cache;
    }

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
    if (hashinsert(ipat, igrp, count)) {
        // printf("(%d,%d)", ipat, igrp);
        // for (size_t i = 0; i < hashcount; ++i)
        //     printf(" %zu:%lld->%lld", i, hashtable[i].key, hashtable[i].val);
        // printf("\n");
    }
    return count;
}

int main(void)
{
    FILE* f = fopen(NAME, "r");
    if (!f)
        return 1;

    hashtable = malloc(hashsize * sizeof *hashtable);

    int n = 0;
    char buf[64];
    while (n < N && fgets(buf, sizeof buf, f)) {
        const char *src = buf;
        char *dst = springs[n].pat;
        int len = 0;
        while (*src != ' ' && len < PLEN - 1) {
            *dst++ = *src++;  // copy pattern
            ++len;
        }
        *dst++ = '.';  // add one operational spring as gap behind the last damaged one
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
        // Backwards cumulative sum of group lengths = how much to go
        for (int i = springs[n].glen - 1, prev = 0; i >= 0; --i)
            prev = (springs[n].all[i] = springs[n].grp[i] + prev + 1);
        ++n;
    }
    fclose(f);

    int64_t sum = 0;
    for (int i = 0; i < n; ++i) {
        hashcount = 0;
        const int64_t count = arrangements(0, 0, &springs[i]);
        sum += count;
        #if EXAMPLE || defined(DEBUG)
        printf("%3d: %3lld %4lld %22s (%2d) %d", i, count, sum, springs[i].pat, springs[i].plen, springs[i].grp[0]);
        for (int j = 1; j < springs[i].glen; ++j)
            printf(",%d", springs[i].grp[j]);
        printf(" | %d", springs[i].all[0]);
        for (int j = 1; j < springs[i].glen; ++j)
            printf(",%d", springs[i].all[j]);
        printf("\n");
        #endif
    }
    printf("Part 1: %"PRId64"\n", sum);  // example: 21, input: 7705
    // printf("%zu %zu\n", hashcount, hashsize);
    free(hashtable);
    return 0;
}
