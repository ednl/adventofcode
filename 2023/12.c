/**
 * Advent of Code 2023
 * Day 12:
 * https://adventofcode.com/2023/day/12
 * By: E. Dronkert https://github.com/ednl
 */

#include <stdio.h>     // fopen, fclose, fgets, printf
// #include <stdint.h>    // int64_t
// #include <inttypes.h>  // PRId64
#include <stdbool.h>   // bool

#define EXAMPLE 0
#if EXAMPLE
#define NAME "../aocinput/2023-12-example.txt"
#define N 6
#else
#define NAME "../aocinput/2023-12-input.txt"
#define N 1000
#endif
#define PLEN 32
#define GLEN 8

typedef struct springs {
    char pat[PLEN];
    int  grp[GLEN], all[GLEN];
    int  plen, glen;
} Springs;

static Springs springs[N];

static int arrangements(const Springs* const row, int ipat, int igrp)
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

    // Start with damaged or unknown spring
    // If unknown, first assume it's operational
    int count = row->pat[ipat] == '#' ? 0 : arrangements(row, ipat + 1, igrp);

    // Remember that, now it's damaged or assume it is
    const int end = ipat + row->grp[igrp];  // index 1 past current group
    for (int i = ipat + 1; i < end; ++i)
        if (row->pat[i] == '.')
            return count;
    if (row->pat[end] == '#')
        return count;
    return count + arrangements(row, end + 1, igrp + 1);  // group fits here
}

int main(void)
{
    FILE* f = fopen(NAME, "r");
    if (!f)
        return 1;

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

    int sum = 0;
    for (int i = 0; i < n; ++i) {
        const int count = arrangements(&springs[i], 0, 0);
        sum += count;
        #if EXAMPLE || defined(DEBUG)
        printf("%3d: %3d %4d %22s (%2d) %d", i, count, sum, springs[i].pat, springs[i].plen, springs[i].grp[0]);
        for (int j = 1; j < springs[i].glen; ++j)
            printf(",%d", springs[i].grp[j]);
        printf(" | %d", springs[i].all[0]);
        for (int j = 1; j < springs[i].glen; ++j)
            printf(",%d", springs[i].all[j]);
        printf("\n");
        #endif
    }
    printf("Part 1: %d\n", sum);  // example: 21, input: 7705
    return 0;
}
