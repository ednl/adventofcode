/**
 * Advent of Code 2023
 * Day 15: Lens Library
 * https://adventofcode.com/2023/day/15
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -O3 -march=native -Wall 15.c ../startstoptimer.c
 *    gcc   -std=gnu17 -O3 -march=native -Wall 15.c ../startstoptimer.c
 * Get minimum runtime:
 *     m=999999;for((i=0;i<5000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * Minimum runtime:
 *     Raspberry Pi 5 (2.4 GHz)            :  311 µs
 *     Mac Mini 2020 (M1 3.2 GHz)          :  501 µs
 *     Raspberry Pi 4 (1.8 GHz)            :  662 µs
 *     iMac 2013 (i5 Haswell 4570 3.2 GHz) : 1020 µs
 */

#include <stdio.h>    // fopen, fclose, fgets, printf
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
    int32_t id;
    uint8_t focal;
} Lens;

typedef struct box {
    Lens lens[M];
    uint8_t count;
} Box;

static Box box[N];

// For my input, label is max. 6 chars long, regex=[a-z]{1,6}
static int32_t label2id(const char *s)
{
    int32_t id = 0;
    while (*s >= 'a' && *s <= 'z')
        id = id * 26 + *s++;
    return id;
}

static uint8_t hash(const char *s)
{
    uint8_t h = 0;
    while (*s) {
        h += (uint8_t)*s++;
        h *= 17;
    }
    return h;
}

// Remove lens from box
// Return true if found and removed, false if box empty or lens not in box
static bool rem(const char *label)
{
    Box *b = &box[hash(label)];
    if (!b->count)
        return false;
    const int32_t id = label2id(label);
    const Lens *end = b->lens + b->count;
    for (Lens *lens = b->lens; lens != end; ++lens)
        if (lens->id == id) {
            const Lens *next = lens + 1;
            if (next != end)
                memcpy(lens, next, (size_t)(end - next) * sizeof *lens);
            --b->count;
            return true;
        }
    return false;
}

// Add lens to box
// Return true if replaced or appended, false for memory allocation failure
static bool add(const char *label, const uint8_t focal)
{
    Box *b = &box[hash(label)];
    const int32_t id = label2id(label);
    const Lens *end = b->lens + M;
    Lens *tail = b->lens + b->count;
    for (Lens *lens = b->lens; lens != tail; ++lens)
        if (lens->id == id) {
            lens->focal = focal;  // replace
            return true;
        }
    if (tail == end)
        return false;
    *tail = (Lens){id, focal};  // append
    ++b->count;
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

int main(void)
{
    starttimer();
    FILE *f = fopen(NAME, "r");
    if (!f) { fputs("File not found.\n", stderr); return 1; }

    char buf[16], *s = buf;
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
    printf("Part 1: %d\n", part1);    // example: 1320, input: 514394
    printf("Part 2: %d\n", power());  // example:  145, input: 236358
    printf("Time: %.0f us\n", stoptimer_us());
    return 0;
}
