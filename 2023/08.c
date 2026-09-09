/**
 * Advent of Code 2023
 * Day 8: Haunted Wasteland
 * https://adventofcode.com/2023/day/8
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 08.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 08.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 2.27 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 3.72 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 6.51 µs
 */

#include <stdio.h>
#include <stdint.h>    // uint16_t, uint64_t
#include <inttypes.h>  // PRIu64
#include <stdbool.h>
#ifdef TIMER
    #include <string.h>  // memset
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2023-08-input.txt"
#define FSIZE 16384  // needed for my input: 13657
#define START 8  // nodes ending in A, needed for my input: 6
#define HSIZE (26 * 26 * 26)  // hash size
#define SSIZE ((HSIZE >> 6) + 1)  // seen size in 64-bit units
#define QSIZE (1 << 3)  // queue size, needed for my input: 1<<2 = 4
#define QMASK (QSIZE - 1)  // queue mask

typedef struct pair {
    uint16_t a, b;  // left/right, index/cost
} Pair;

static char input[FSIZE];
static Pair node[HSIZE];
static uint16_t start[START];
static uint64_t seen[SSIZE];
static Pair queue[QSIZE];
static unsigned qhead, qtail;

// Assume queue never full (for my input: maxlen=3)
static void push(const Pair x)
{
    queue[qhead++] = x;
    qhead &= QMASK;
}

static bool pop(Pair *const x)
{
    if (qhead == qtail)  // queue is never full
        return false;  // so only return false when queue is empty
    *x = queue[qtail++];
    qtail &= QMASK;
    return true;
}

// "AAA"=0, "AAB"=676, "AAZ"=16900, "ZZZ"=17575
// So "ends in A": hash < 676
//    "ends in Z": hash >= 16900
static uint16_t hash(const char *s)
{
    return *s + *(s + 1) * 26 + *(s + 2) * 26 * 26 - ('A' + 'A' * 26 + 'A' * 26 * 26);
}

// Mark index as seen, return false if already seen
static bool mark(const uint16_t x)
{
    const int i = x >> 6;
    const uint64_t bit = UINT64_C(1) << (x & ((1 << 6) - 1));
    if (seen[i] & bit)
        return false;
    seen[i] |= bit;
    return true;
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    fread(input, 1, sizeof input, f);  // read single bytes until EOF
    fclose (f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
    memset(seen, 0, sizeof seen);
#endif

    // First line length, my input: 293
    const char *c = input + 250;
    for (; *c != '\n'; c++);
    const unsigned len = c - input;

    // Build graph, save nodes ending in A
    unsigned count = 0;
    for (c += 2; *c; c += 17) {  // skip empty line, skip line
        const uint16_t h = hash(c);
        node[h] = (Pair){hash(c + 7), hash(c + 12)};
        if (h < 676)  // "xxA" < "AAB" = 676
            start[count++] = h;
    }

    unsigned part1 = len;
    uint64_t part2 = len;
    for (unsigned i = 0; i < count; ++i) {
        qhead = qtail = 0;  // reset queue
        push((Pair){start[i], 0});
        mark(start[i]);
        Pair x;
        while (pop(&x)) {
            if (x.a >= 16900) {  // "xxZ"
                if (x.a == 17575)  // "ZZZ"
                    part1 *= x.b;  // LCM of primes is product
                part2 *= x.b;
                break;
            }
            if (mark(node[x.a].a))
                push((Pair){node[x.a].a, x.b + 1});
            if (mark(node[x.a].b))
                push((Pair){node[x.a].b, x.b + 1});
        }
    }
    printf("%u %"PRIu64"\n", part1, part2);  // 19631 21003205388413

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
