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
 *     Macbook Pro 2024 (M4 4.4 GHz) :  93 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :   ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : 235 µs
 */

#include <stdio.h>
#include <stdlib.h>  // qsort
#include <string.h>  // memset
#include <stdint.h>  // uint32_t, UINT32_C, uint8_t
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2020-07-input.txt"
#define FSIZE 48000  // needed for my input: 44374
#define BAGS    594  // bags (= lines in input file)
#define MAXINSIDE 4  // max number of different bags inside

typedef struct bag {
    uint32_t key;  // hashed name
    uint32_t inside[MAXINSIDE];
    uint8_t  amount[MAXINSIDE];
} Bag;

static const char *mybag = "shiny gold";
static uint32_t mybagindex;  // TBD at runtime

static char input[FSIZE];
static Bag bag[BAGS];

// Memoization
static int hasmybag[BAGS];  // -1=unknown, 0=no, 1=yes
static int bagcount[BAGS];  // -1=unknown, 0+=bags inside count

static int cmp_key_asc(const void *p, const void *q)
{
    const Bag *a = p;
    const Bag *b = q;
    if (a->key < b->key) return -1;
    if (a->key > b->key) return +1;
    return 0;
}

static uint32_t binsearch(const uint32_t key)
{
    uint32_t l = 0, r = BAGS - 1;
    if (key == bag[l].key) return l;
    if (key == bag[r].key) return r;
    // Now always true: bag[l].key < key < bag[r].key
    while (r - l > 1) {  // adjacent means not found
        uint32_t m = ((l + 1) >> 1) + (r >> 1);  // avoid index overflow
        if      (key > bag[m].key) l = m;
        else if (key < bag[m].key) r = m;
        else return m;
    }
    return -1;  // should not happen for this data (famous last words)
}

// Fowler–Noll–Vo 32-bit hash function for null-terminated string, version 1a
// https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function
static uint32_t fnv1a_hash(const char *s)
{
    uint32_t h = UINT32_C(0x811c9dc5);
    while (*s) {
        h ^= *s++;
        h *= UINT32_C(0x01000193);
    }
    return h;
}

// Bag at index has my bag: 1=yes, 0=no
static int isinside(const int index)
{
    if (hasmybag[index] != -1)
        return hasmybag[index];
    for (int i = 0; i < MAXINSIDE && bag[index].amount[i]; ++i)
        if (bag[index].inside[i] == mybagindex)
            return (hasmybag[index] = 1);
    for (int i = 0; i < MAXINSIDE && bag[index].amount[i]; ++i)
        if (isinside(bag[index].inside[i]))
            return (hasmybag[index] = 1);
    return (hasmybag[index] = 0);
}

// Count all bags inside this one recursively
static int countinside(const int index)
{
    if (bagcount[index] != -1)
        return bagcount[index];
    int sum = 0;
    for (int i = 0; i < MAXINSIDE && bag[index].amount[i]; ++i)
        sum += (countinside(bag[index].inside[i]) + 1) * bag[index].amount[i];
    return (bagcount[index] = sum);
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

    // Parse
    char *c = input;  // not const because bag names tokenized
    for (int n = 0; n < BAGS; ++n) {
        const char *s = c;              // save start of container name
        for (int i = 0; i < 2; i += *c++ == ' ');  // skip container name
        *(c - 1) = '\0';                // terminate container name
        bag[n].key = fnv1a_hash(s);     // hash container name
        c += 13;                        // skip "bags contain "
        if (*c != 'n')
            for (int k = 0; k < MAXINSIDE; ++k) {
                bag[n].amount[k] = *c & 15;        // 1-digit number
                s = (c += 2);                      // save start of content name
                for (int i = 0; i < 2; i += *c++ == ' ');  // skip content name
                *(c - 1) = '\0';                   // terminate content name
                bag[n].inside[k] = fnv1a_hash(s);  // hash content name
                c += 4 + (bag[n].amount[k] > 1);   // skip "bag(s)(,|.)"
                if (*c++ == '\n')                  // newline? (or space)
                    break;
            }
        else
            c += 15;  // skip "no other bags.\n"
    }

    // Convert hash to index
    qsort(bag, BAGS, sizeof *bag, cmp_key_asc);
    for (int i = 0; i < BAGS; ++i)
        for (int j = 0; j < MAXINSIDE && bag[i].amount[j]; ++j)
            bag[i].inside[j] = binsearch(bag[i].inside[j]);
    mybagindex = binsearch(fnv1a_hash(mybag));

    // Part 1
    memset(hasmybag, -1, sizeof hasmybag);
    int sum = 0;
    for (int i = 0; i < BAGS; ++i)
        sum += isinside(i);
    printf("%d\n", sum);  // 259

    // Part 2
    memset(bagcount, -1, sizeof bagcount);
    printf("%d\n", countinside(mybagindex));  // 45018

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
}
