/**
 * Advent of Code 2023
 * Day 15: Lens Library
 * https://adventofcode.com/2023/day/15
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 15.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 15.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 11.9 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 21.3 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 81.4 µs
 */

#include <stdio.h>
#include <string.h>  // memmove, memset
#include <stdint.h>  // uint32_t, uint8_t
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2023-15-input.txt"
#define FSIZE ((1<<14)|(1<<13))  // needed for my input: 22846
#define N 256  // number of boxes
#define M 8    // max number of lenses per box, needed for my input: 6

typedef uint32_t u32;
typedef uint8_t u8;
typedef struct lens {
    u32 label;
    u8 focal;
} Lens;

static char input[FSIZE];
static Lens lens[N][M];
static u8 count[N];

static inline u32 next(const u32 prev, const u8 byte)
{
    return (prev + byte) * 17 & 0xff;
}

// Remove lens from box
static void rem(const u32 box, const u32 label)
{
    for (u8 i = 0; i != count[box]; ++i)
        if (lens[box][i].label == label) {
            memmove(&lens[box][i], &lens[box][i + 1], (--count[box] - i) * sizeof (Lens));
            return;  // thanks /u/terje_wiig_mathisen for spotting that I forgot this
        }
}

// Insert lens into box
static void ins(const u32 box, const u32 label, const u8 focal)
{
    for (u8 i = 0; i != count[box]; ++i)
        if (lens[box][i].label == label) {
            lens[box][i].focal = focal;  // replace
            return;
        }
    lens[box][count[box]++] = (Lens){label, focal};  // append
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
    memset(count, 0, sizeof count);
#endif

    u32 part1 = 0;
    for (const char *c = input; *c; c += 2) {
        u32 hash = 0, label = 0;
        for (; *c >= 'a'; c++) {
            hash = next(hash, *c);
            label = label << 5 | (*c & 31);  // max 6 chars
        }
        if (*c == '-') {
            rem(hash, label);
            hash = next(hash, '-');
        } else {  // '='
            ins(hash, label, *++c & 15);
            hash = next(next(hash, '='), *c);
        }
        part1 += hash;
    }
    u32 part2 = 0;
    for (u32 i = 0; i != N; ++i)
        for (u8 j = 0; j != count[i]; ++j)
            part2 += (i + 1) * (j + 1) * lens[i][j].focal;
    printf("%u %u\n", part1, part2);  // 514394 236358

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
