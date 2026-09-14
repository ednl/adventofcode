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
 *     m=99999999;for((i=0;i<5000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * Minimum runtime:
 *     Raspberry Pi 5 (2.4 GHz)      : 30.0 µs
 *     Macbook Pro 2024 (M4 4.4 GHz) : ? µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : ? µs
 */

#include <stdio.h>
#include <string.h>  // memcpy
#include <stdint.h>  // int64_t
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2023-15-input.txt"
#define FSIZE ((1<<14)|(1<<13))  // needed for my input: 22846
#define N 256  // number of boxes
#define M 8    // max number of lenses per box, needed for my input: 6

typedef struct lens {
    int32_t id;
    uint8_t focal;
} Lens;

typedef struct box {
    Lens lens[M];
    uint8_t count;
} Box;

static char input[FSIZE];
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
    FILE *f = fopen(FNAME, "rb");
    if (!f) { fputs("File not found: "FNAME, stderr); return 1; }
    fread(input, 1, sizeof input, f);
    fclose(f);

#ifdef TIMER
starttimer();
for (unsigned TIMERLOOP = 1000; TIMERLOOP--; ) {
#endif

    char buf[16], *s = buf;
    int part1 = 0;
    for (const char *c = input; *c; c++)
        switch (*c) {
            case '\n':
            case ',' : *s = '\0'; part1 += hash(buf); s = buf; break;
            case '-' : *s = '\0'; rem(buf); *s++ = '-'; break;
            case '=' : *s = '\0'; add(buf, *++c & 15); *s++ = '='; *s++ = *c; break;
            default  : *s++ = *c;
        }
    printf("%u %u\n", part1, power());  // 514394 236358

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
