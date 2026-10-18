/**
 * Advent of Code 2024
 * Day 18: RAM Run
 * https://adventofcode.com/2024/day/18
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 18.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 18.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 20.2 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : ? µs
 */

#include <stdio.h>
#include <string.h>  // memset
#include <stdint.h>  // uint8_t
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

// Input file
#define FNAME "../aocinput/2024-18-input.txt"
#define FSIZE (5<<12)    // =20480, needed for my input: 19727 +1
#define BYTES 3450       // lines in input file
#define QBITS 8          // needed for my input: 7 ((1<<7) > 94)

// Puzzle parameters
#define T 1024           // time (bytes dropped) for part 1
#define N 71             // RAM grid width and height

// Derived values
#define M   (N + 2)      // map width/height with border
#define BEG (M + 1)      // top left of memory grid
#define END (N * M + N)  // bottom right of memory grid
#define QSIZE (1u << QBITS)
#define QMASK (QSIZE - 1)

// Solution parameters
#define CORRUPT 1        // byte was dropped here
#define TOPRGHT 2        // connected to top-right half
#define BOTLEFT 4        // connected to bottom left half
#define BLOCKED 7        // blocking the way from start to end
#define SEEN    8        // processed by part 1 BFS

static const int nb4[] = {1, M, -1, -M};
static const int nb8[] = {-(M + 1), -M, -(M - 1), -1, 1, M - 1, M, M + 1};

static char input[FSIZE];
static unsigned byte[BYTES];
static uint8_t map[M * M];
static unsigned qdata[QSIZE];
static unsigned qhead, qtail;

// Assume queue is never full (needed for my input: 94)
static void enq(const unsigned val)
{
    qdata[qhead++] = val;
    qhead &= QMASK;
}

// Assume queue is never full (or else head==tail)
static bool deq(unsigned *const val)
{
    if (qhead == qtail)
        return false;
    *val = qdata[qtail++];
    qtail &= QMASK;
    return true;
}

static unsigned readnum(const char **s)
{
    unsigned x = 0;
    while (**s & 16)  // until comma or newline
        x = x * 10 + (*(*s)++ & 15);
    return x;
}

static unsigned bfs(void)
{
    unsigned ixlen = BEG << 16, ix, len;
    map[BEG] |= SEEN;
    do {
        ix = ixlen >> 16;
        len = ixlen & ((1u<<16) - 1);
        if (ix == END)
            return len;
        for (unsigned i = 0; i < 4; ++i) {
            const unsigned k = ix + nb4[i];
            if (!(map[k] & (SEEN | CORRUPT))) {
                map[k] |= SEEN;
                enq(k << 16 | (len + 1));
            }
        }
    } while (deq(&ixlen));
    return 0;
}

static void flood(const unsigned ix, const unsigned side)
{
    for (unsigned i = 0; i < 8; ++i) {
        const unsigned k = ix + nb8[i];
        if ((map[k] & 7) == CORRUPT) {  // unconnected byte
            map[k] |= side;
            flood(k, side);
        }
    }
}

static bool drop(const unsigned ix)
{
    map[ix] |= CORRUPT;
    for (unsigned i = 0; i < 8; ++i)
        map[ix] |= map[ix + nb8[i]];
    switch (map[ix] & 7) {
        case CORRUPT: return false;
        case BLOCKED: return true;
    }
    flood(ix, map[ix] & (TOPRGHT | BOTLEFT));
    return false;
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) { fputs("File not found: "FNAME, stderr); return 1; }
    fread(input, 1, FSIZE, f);  // read single bytes until EOF
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
    memset(map, 0, sizeof map);
    qhead = qtail = 0;
#endif

    memset(map, TOPRGHT | CORRUPT, M);
    memset(&map[(M - 1) * M], BOTLEFT | CORRUPT, M);
    for (int i = M - 1; i < (M - 1) * M; i += M) {
        map[i] = TOPRGHT | CORRUPT;
        map[i + 1] = BOTLEFT | CORRUPT;
    }
    const char *s = input;
    for (unsigned i = 0; *s; ++i) {
        const unsigned x = readnum(&s); s++;  // skip comma
        const unsigned y = readnum(&s); s++;  // skip newline
        byte[i] = ((y + 1) * M) + x + 1;  // +borders
    }

    // Part 1
    unsigned t = 0;
    for (; t < T; ++t)
        drop(byte[t]);
    printf("%u ", bfs());  // 286

    // Part 2
    for (; t < BYTES; ++t)
        if (drop(byte[t]))
            break;
    printf("%u,%u\n", byte[t] % M - 1, byte[t] / M - 1);  // 20,64

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
