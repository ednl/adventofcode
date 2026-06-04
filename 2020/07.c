/**
 * Advent of Code 2020
 * Day 7: Handy Haversacks
 * https://adventofcode.com/2020/day/7
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 07.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 07.c
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  97 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :   ? µs
 *     Raspberry Pi 5 (2.4 GHz)      :   ? µs
 */

#include <stdio.h>
#include <stdlib.h>  // qsort
#include <string.h>  // memcpy, memset
#include <stdint.h>  // uint32_t, UINT32_C, uint8_t
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2020-07-input.txt"
#define FSIZE 48000  // needed for my input: 44374
#define BAGS 594  // bags (lines in input file)
#define CONT 4    // max number of different bags inside

typedef struct bag {
    uint32_t id;  // hashed name
    uint32_t inside[CONT];
    uint8_t  amount[CONT];
} Bag;

static const char *mybag = "shiny gold";
static uint32_t mybagindex;
static char input[FSIZE];
static Bag bag[BAGS];
static int hasmybag[BAGS];  // -1=unknown, 0=no, 1=yes
static int contains[BAGS];  // -1=unknown, 0+=bags inside count

static int cmp_id_asc(const void *p, const void *q)
{
    const Bag *a = p;
    const Bag *b = q;
    if (a->id < b->id) return -1;
    if (a->id > b->id) return +1;
    return 0;
}

static uint32_t binsearch(const uint32_t id)
{
    uint32_t l = 0, r = BAGS - 1;
    if (id == bag[l].id) return l;
    if (id == bag[r].id) return r;
    // Now always true: bag[l].id < id < bag[r].id
    while (r - l > 1) {  // adjacent means not found
        uint32_t m = ((l + 1) >> 1) + (r >> 1);  // avoid index overflow
        if      (id > bag[m].id) l = m;
        else if (id < bag[m].id) r = m;
        else return m;
    }
    return -1;  // should not happen for this data (famous last words)
}

// Fowler–Noll–Vo 32-bit hash function for null-terminated string, version 1a
// https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function#FNV-1a_hash
// https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function#FNV_hash_parameters
static uint32_t fnv1a(const char *s)
{
    uint32_t h = UINT32_C(0x811c9dc5);
    while (*s) {
        h ^= *s++;
        h *= UINT32_C(0x01000193);
    }
    return h;
}

static int lookinside(const int index)
{
    if (hasmybag[index] != -1)
        return hasmybag[index];
    for (int i = 0; i < CONT && bag[index].amount[i]; ++i)
        if (bag[index].inside[i] == mybagindex)
            return (hasmybag[index] = 1);
    for (int i = 0; i < CONT && bag[index].amount[i]; ++i)
        if (lookinside(bag[index].inside[i]))
            return (hasmybag[index] = 1);
    return (hasmybag[index] = 0);
}

static int countinside(const int index)
{
    if (contains[index] != -1)
        return contains[index];
    int sum = 0;
    for (int i = 0; i < CONT && bag[index].amount[i]; ++i)
        sum += (countinside(bag[index].inside[i]) + 1) * bag[index].amount[i];
    return (contains[index] = sum);
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    fread(input, 1, FSIZE, f);  // read unknown size per byte
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    char *c = input;
    for (int n = 0; n < BAGS; ++n) {
        const char *s = c;              // save start of container name
        for (int i = 0; i < 2; i += *c++ == ' ');  // skip container name
        *(c - 1) = '\0';                // terminate container name
        bag[n].id = fnv1a(s);           // hash container name
        // memcpy(bag[n].name, s, c - s);  // save container name (incl. '\0')
        c += 13;                        // skip "bags contain "
        if (*c != 'n')
            for (int k = 0; k < CONT; ++k) {
                bag[n].amount[k] = *c & 15;       // 1-digit number
                s = (c += 2);                     // save start of content name
                for (int i = 0; i < 2; i += *c++ == ' ');  // skip content name
                *(c - 1) = '\0';                  // terminate content name
                bag[n].inside[k] = fnv1a(s);      // hash content name
                c += 4 + (bag[n].amount[k] > 1);  // skip "bag(s)(,|.)"
                if (*c++ == '\n')                 // newline? (or else: space)
                    break;
            }
        else
            c += 15;  // skip "no other bags.\n"
    }

    qsort(bag, BAGS, sizeof *bag, cmp_id_asc);
    for (int i = 0; i < BAGS; ++i)
        for (int j = 0; j < CONT && bag[i].amount[j]; ++j)
            bag[i].inside[j] = binsearch(bag[i].inside[j]);
    mybagindex = binsearch(fnv1a(mybag));

    // Part 1
    memset(hasmybag, -1, sizeof hasmybag);
    for (int i = 0; i < BAGS; ++i)
        hasmybag[i] = lookinside(i);
    int sum = 0;
    for (int i = 0; i < BAGS; ++i)
        sum += hasmybag[i];
    printf("%d\n", sum);  // 259

    // Part 2
    memset(contains, -1, sizeof contains);
    printf("%d\n", countinside(mybagindex));  // 45018

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
}
