/**
 * Advent of Code 2023
 * Day 15: ???
 * https://adventofcode.com/2023/day/15
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -Ofast -march-native -Wall 15.c ../startstoptimer.c
 *    gcc   -std=gnu17 -Ofast -march-native -Wall 15.c ../startstoptimer.c
 * Get minimum runtime:
 *     m=999999;for((i=0;i<2000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * Minimum runtime:
 *     Apple M1 Mac Mini      : 507 µs
 *     Raspberry Pi 5 2.4 GHz : ?? µs
 */

#include <stdio.h>    // fopen, fclose, fgets, printf
#include <stdlib.h>   // realloc, free
#include <string.h>   // memcpy
#include <stdint.h>   // int64_t
#include <stdbool.h>  // bool
#include "../startstoptimer.h"

#define EXAMPLE 0
#if EXAMPLE
#define NAME "../aocinput/2023-15-example.txt"
#else
#define NAME "../aocinput/2023-15-input.txt"
#endif
#define N 256  // number of boxes
#define M 8    // max number of lenses per box (my input: 6)

typedef struct lens {
    int64_t id;
    int focal;
} Lens;

typedef struct box {
    Lens* lens;
    size_t size, count;
} Box;

static Box box[N];

// For my input, label is max. 6 chars long, regex=[a-z]{1,6}
static int64_t label2id(const char* s)
{
    int64_t id = 0;
    while (*s >= 'a' && *s <= 'z')
        id = id << 8 | *s++;
    return id;
}

static int hash(const char* s)
{
    int h = 0;
    while (*s) {
        h += *s++;
        h *= 17;
    }
    return h & 0xff;
}

static bool grow(Lens** arr, size_t* oldsize)
{
    size_t newsize = *oldsize ? *oldsize << 1 : M;
    Lens* p = realloc(*arr, newsize * sizeof **arr);
    if (!p)
        return false;
    *arr = p;
    *oldsize = newsize;
    return true;
}

// Remove lens from box
// Return true if found and removed, false if box empty or lens not in box
static bool rem(const char* label)
{
    Box* b = &box[hash(label)];
    if (!b->lens || !b->size || !b->count)
        return false;
    const int64_t id = label2id(label);
    const Lens* end = b->lens + b->count;
    for (Lens* a = b->lens; a != end; ++a)
        if (a->id == id) {
            const Lens* next = a + 1;
            if (next != end)
                memcpy(a, next, (size_t)(end - next) * sizeof *a);
            --b->count;
            return true;
        }
    return false;
}

// Add lens to box
// Return true if replaced or appended, false for memory allocation failure
static bool add(const char* label, const int focal)
{
    Box* b = &box[hash(label)];
    if ((!b->lens || !b->size) && !grow(&b->lens, &b->size))
        return false;
    const int64_t id = label2id(label);
    for (size_t i = 0; i < b->count; ++i)
        if (b->lens[i].id == id) {
            b->lens[i].focal = focal;  // replace
            return true;
        }
    if (b->count == b->size && !grow(&b->lens, &b->size))
        return false;
    b->lens[b->count++] = (Lens){id, focal};  // append
    return true;
}

// Focussing power of final lens configuration
static int power(void)
{
    int sum = 0;
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < (int)box[i].count; ++j)
            sum += (i + 1) * (j + 1) * box[i].lens[j].focal;
    return sum;
}

static void clean(void)
{
#if EXAMPLE || defined(DEBUG)
    size_t used = 0, maxc = 0, maxs = 0;
    for (int i = 0; i < N; ++i) {
        if (box[i].count > maxc) maxc = box[i].count;
        if (box[i].size > maxs) maxs = box[i].size;
        if (box[i].lens) {
            ++used;
            printf("%d: %3zu %4zu %p\n", i, box[i].count, box[i].size, (void*)box[i].lens);
            free(box[i].lens);
        }
    }
    printf("used= %zu maxcount=%zu maxsize=%zu\n", used, maxc, maxs);
#else
    for (int i = 0; i < N; ++i)
        free(box[i].lens);
#endif
}

int main(void)
{
    starttimer();
    FILE* f = fopen(NAME, "r");
    if (!f)
        return 1;
    char buf[32], *s = buf;
    int c, part1 = 0;
    while ((c = fgetc(f)) != EOF)
        switch (c) {
            case '\n':
            case ',' : *s = '\0'; part1 += hash(buf); s = buf; break;
            case '-' : *s = '\0'; rem(buf); *s++ = '-'; break;
            case '=' : *s = '\0'; add(buf, (c = fgetc(f)) & 15); *s++ = '='; *s++ = (char)c; break;
            default  : *s++ = (char)c;
        }
    fclose(f);
    printf("Part 1: %d\n", part1);  // example: 1320, input: 514394
    printf("Part 2: %d\n", power());  // example: 145, input: 236358
    clean();
    printf("Time: %.0f us\n", stoptimer_us());
    return 0;
}
