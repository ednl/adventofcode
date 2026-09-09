/**
 * Advent of Code 2023
 * Day 5: If You Give A Seed A Fertilizer
 * https://adventofcode.com/2023/day/5
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 05a.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 05a.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 2.45 µs (part 1)
 *     Mac Mini 2020 (M1 3.2 GHz)    : ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : ? µs
 *     iMac 2013 (i5 4570 3.2 GHz)   : ? µs
 */

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2023-05-input.txt"
#define FSIZE 6400
#define SEED 20
#define MAPS 7
#define CONV 48

typedef struct map {
    uint64_t dst, src, end;
} Map;
typedef struct range {
    uint64_t a, b;
} Range;

static char input[FSIZE];
static uint64_t seed[SEED];
static Map map[MAPS][CONV];
static int maplen[MAPS];
static Range range[256];

static uint64_t parseint(const char **s)
{
    uint64_t x = 0;
    while (**s & 16)
        x = x * 10 + (*(*s)++ & 15);
    (*s)++;
    return x;
}

static uint64_t applymap1(const Map *const m, const int len, const uint64_t x)
{
    for (int i = 0; i < len; ++i)
        if (x >= m[i].src && x < m[i].end)
            return x - m[i].src + m[i].dst;
    return x;
}

static Range applymap2(const Map *const m, const int len, const Range r)
{
    for (int i = 0; i < len; ++i) {
        const uint64_t x = r.a > m[i].src ? r.a : m[i].src;  // max start
        const uint64_t y = r.b < m[i].end ? r.b : m[i].end;  // min end
        if (x < y)  // overlap
            return (Range){x, y};
    }
    return r;
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    fread(input, 1, sizeof input, f);
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    const char *c = input + 7;
    for (int i = 0; i < SEED; ++i)
        seed[i] = parseint(&c);
    for (int i = 0; i < MAPS; ++i) {
        c += 18;
        while (*c != '\n')
            c++;
        c++;
        int j = 0;
        do {
            const uint64_t dst = parseint(&c);
            const uint64_t src = parseint(&c);
            const uint64_t len = parseint(&c);
            map[i][j++] = (Map){dst, src, src + len};
        } while (*c & 16);
        maplen[i] = j;
    }

    uint64_t part1 = UINT32_MAX;
    for (int i = 0; i < SEED; ++i) {
        uint64_t x = seed[i];
        for (int j = 0; j < MAPS; ++j)
            x = applymap1(&map[j][0], maplen[j], x);
        if (x < part1)
            part1 = x;
    }
    printf("%"PRIu64" ", part1);  // 836040384

    for (int i = 0; i < SEED; i += 2)
        range[i >> 1] = (Range){seed[i], seed[i] + seed[i + 1]};
    int rlen = SEED >> 1;

    uint64_t part2 = UINT32_MAX;
    /*
    for (int i = 0; i < MAPS; ++i) {
        int next = rlen;
        for (int j = 0; j < rlen; ++j) {
            const uint64_t x = range[j].a > map[i].src ? r.a : m[i].src;  // max start
            const uint64_t y = r.b < m[i].end ? r.b : m[i].end;  // min end
            if (x < y)  // overlap
                range[next++] = (Range){x, y};
        }
        rlen = next;
    }
        // if (r.a < part2)
        //     part2 = r.a;
    */
    printf("%"PRIu64"\n", part2);  // 10834440

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
