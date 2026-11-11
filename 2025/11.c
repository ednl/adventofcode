/**
 * Advent of Code 2025
 * Day 11: Reactor
 * https://adventofcode.com/2025/day/11
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 11.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 11.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  7.35 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :  9.92 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 17.6  µs
 */

#include <stdio.h>
#include <string.h>    // memset
#include <stdint.h>    // uint32_t, uint64_t
#include <inttypes.h>  // PRIu64
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2025-11-input.txt"
#define FSIZE (3<<12)  // 12288, needed for my input: 9448
#define NODES   576    // max node count, needed for my input: 560+1=561
#define CHILDREN 32    // max children per node, needed for my input: 25

typedef uint64_t u64;
typedef uint32_t u32;

static char input[FSIZE];
static const char *list[NODES];  // every start of child nodes in input file
static u32 name[26 * 26 * 26];  // "aaa"=0 to "zzz"=17575
static u32 len[NODES];  // child count per node
static u32 child[NODES][CHILDREN];  // child names translated to indexes
static u32 cache[NODES];  // path counts

// "aaa"=0 to "zzz"=17575
static u32 hash(const char *s)
{
    return s[0] * 26 * 26 + s[1] * 26 + s[2] - ('a' * 26 * 26 + 'a' * 26 + 'a');
}

// Zero can be legit cache value in part 2, so don't use that
static void resetcache(void)
{
    memset(cache, -1, sizeof cache);
}

// Recursive DFS with memoization to count all paths from i to end
// i and end are indexes [0..NODES)
static u32 paths(const u32 i, const u32 end)
{
    if (i == end)
        return 1;
    if (cache[i] != UINT32_MAX)  // -1 is init value (="not cached yet")
        return cache[i];  // 0 for paths unable to reach the end
    u32 count = 0;
    for (u32 j = 0; j < len[i]; ++j)
        count += paths(child[i][j], end);
    return (cache[i] = count);
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) { fprintf(stderr, "File not found: %s\n", FNAME); return 1; }
    fread(input, 1, sizeof input, f);  // read single bytes until EOF
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    u32 n = 0;
    for (const char *s = input; *s; ++n, ++s) {  // also skip newline
        name[hash(s)] = n;               // hash table of node name to index
        list[n] = s + 5;                 // remember start of child names list
        const char *const t = (s += 4);  // jump to zeroth separator and remember
        for (; *s != '\n'; s += 4);      // skip to newline
        len[n] = (s - t) >> 2;           // child node count
    }
    name[hash("out")] = n++;             // add unlisted "out" node with zero children
    for (u32 i = 0; i < n; ++i) {        // translate all child names to indexes
        const char *s = list[i];
        for (u32 j = 0; j < len[i]; s += 4, ++j)
            child[i][j] = name[hash(s)];
    }

    // Part 1
    const u32 you = name[hash("you")];
    const u32 out = name[hash("out")];
    resetcache();  // must init at program start because "not cached" = -1
    printf("%u ", paths(you, out));  // 670

    // Part 2
    // Input is directed acyclic graph, so either fft->dac or dac->fft is
    // possible, not both. For my input, full path is svr->fft->dac->out
    // which is also the logical order from an electronics perspective.
    const u32 svr = name[hash("svr")];
    const u32 fft = name[hash("fft")];
    const u32 dac = name[hash("dac")];
    resetcache(); const u32 p1 = paths(svr, fft);  // 8049
    resetcache(); const u32 p2 = paths(fft, dac);  // 3377314
    resetcache(); const u32 p3 = paths(dac, out);  // 12215
    printf("%"PRIu64"\n", (u64)p1 * p2 * p3);   // 332052564714990

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
