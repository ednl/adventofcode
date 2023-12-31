/**
 * Advent of Code 2023
 * Day 15: Lens Library
 * https://adventofcode.com/2023/day/15
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -Ofast -march=native -Wall 15alt.c ../startstoptimer.c
 *    gcc   -std=gnu17 -Ofast -march=native -Wall 15alt.c ../startstoptimer.c
 * Get minimum runtime:
 *     m=999999;for((i=0;i<5000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * Minimum runtime:
 *     Mac Mini 2020 (M1 3.2 GHz)          :  346 µs
 *     iMac 2013 (i5 Haswell 4570 3.2 GHz) :  523 µs
 *     Raspberry Pi 5 (2.4 GHz)            :  556 µs
 *     Raspberry Pi 4 (1.8 GHz)            : 1174 µs
 */

#include <stdio.h>    // fopen, fclose, getline, printf
#include <stdlib.h>   // free
#include <stdint.h>   // int64_t
#include <stdbool.h>  // bool
#include "../startstoptimer.h"

#define EXAMPLE 0
#if EXAMPLE
#define NAME "../aocinput/2023-15-example.txt"
#define N 11  // number of steps
#else
#define NAME "../aocinput/2023-15-input.txt"
#define N 4000  // number of steps
#endif

// n=[0..N), box=[0..255], focal=0=remove, focal=[1..9]=add/replace
typedef struct step {
    int32_t id;  // numerical ID from text label=[a-z]{1,6}
    int16_t ord;
    uint8_t box, focal;
} Step;

static Step step[N];

#if EXAMPLE || defined(DEBUG)
static int makelabel(int32_t id, char* label)
{
    char buf[8];
    int len = 0;
    while (id) {
        buf[len++] = 'a' + id % 26;
        id /= 26;
    }
    for (int i = 0; i < len; ++i)
        label[i] = buf[len - i - 1];
    label[len] = '\0';
    return len;
}

static void show(int n)
{
    char buf[8];
    for (int i = 0; i < n; ++i)
        if (step[i].box < 4) {
            makelabel(step[i].id, buf);
            printf("%2d: ord=%2d box=%d [%s %d]\n", i, step[i].ord, step[i].box, buf, step[i].focal);
        }
}
#endif

static int cmp_id_ord(const void* p, const void* q)
{
    const Step* const a = p;
    const Step* const b = q;
    if (a->id  < b->id ) return -1;
    if (a->id  > b->id ) return  1;
    if (a->ord < b->ord) return -1;
    if (a->ord > b->ord) return  1;
    return 0;
}

static int cmp_box_ord(const void* p, const void* q)
{
    const Step* const a = p;
    const Step* const b = q;
    if (a->box < b->box) return -1;
    if (a->box > b->box) return  1;
    if (a->ord < b->ord) return -1;
    if (a->ord > b->ord) return  1;
    return 0;
}

int main(void)
{
    starttimer();
    FILE* f = fopen(NAME, "r");
    if (!f) { fputs("File not found.\n", stderr); return 1; }

    char* line = NULL;
    size_t bufsz;
    ssize_t len;
    if ((len = getline(&line, &bufsz, f)) < 1)  // read whole file (one line) at once
        return 2;
    fclose(f);
    line[--len] = '\0';  // remove newline, now 2x '\0' in a row

    int part1 = 0;
    int16_t count = 0;
    const char* s = line;
    while (count < N) {
        int32_t id = 0;
        uint8_t hash = 0;
        while (*s >= 'a') {
            id = id * 26 + (*s - 'a');
            hash += *s++;
            hash *= 17;
        }
        // hash &= 0xff;  // box number for part 2
        if (*s == '-') {
            step[count] = (Step){id, count, hash, 0};
            hash = hash * 17 + 253;  // add '-' to hash for part 1
        } else if (*s == '=') {
            step[count] = (Step){id, count, hash, *(++s) & 15};
            hash = hash * 33 + (uint8_t)*s * 17 + 221;  // add '=' and focal length to hash for part 1
        }
        part1 += (hash & 0xff);
        ++count;
        s += 2;
    }
    printf("Part 1: %d\n", part1);  // example: 1320, input: 514394

    qsort(step, (size_t)count, sizeof *step, cmp_id_ord);
#if EXAMPLE || defined(DEBUG)
    show(count);
#endif

    int reduced = 0;
    for (int i = 0, j, end; i < count; i = end) {
        for (end = i + 1; end < count && step[end].id == step[i].id; ++end);
        const int last = end - 1;
        if (!step[last].focal) continue;
        for (j = last; i < j && step[j - 1].focal; --j);
        step[j].focal = step[last].focal;
        step[reduced++] = step[j];
    }

    qsort(step, (size_t)reduced, sizeof *step, cmp_box_ord);
#if EXAMPLE || defined(DEBUG)
    show(reduced);
    printf("%d %d\n", count, reduced);  // 4000 246
#endif

    int part2 = 0;
    for (int i = 0, j = 1; i < reduced; ++j) {
        const int box = step[i].box;
        for (; j < reduced && step[j].box == box; ++j);
        const int slots = j - i;
        for (int slot = 0; slot < slots; ++slot, ++i)
            part2 += (box + 1) * (slot + 1) * step[i].focal;
    }
    printf("Part 2: %d\n", part2);  // example: 145, input: 236358

    free(line);
    printf("Time: %.0f us\n", stoptimer_us());
    return 0;
}
