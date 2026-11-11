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
 *     Macbook Pro 2024 (M4 4.4 GHz) :  46.5 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :  79.6 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 138.1 µs
 */

#include <stdio.h>
#include <stdlib.h>    // qsort
#include <string.h>    // memcpy, memset
#include <stdint.h>    // uint32_t, uint64_t
#include <inttypes.h>  // PRIu64
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2025-11-input.txt"
#define FSIZE (3<<12)  // =12288, needed for my input: 9448
#define NODES   576    // lines in input file +"out", needed for my input: 560+1=561
#define CHILDREN 32    // max children per node, needed for my input: 25
#define NAMELEN   4    // node name length: 3 chars +'\0'
#define NAMEMASK ((1U<<24) - 1)  // first 3 bytes of name (assumes little-endian)

typedef uint64_t u64;
typedef uint32_t u32;

// Node::name must be first member, for qsort() and bsearch()
typedef struct node {
    u32 name;    // must be 32-bit = 4 bytes = string of len 3 +'\0'
    u32 len;     // child node count
    u32 *child;  // child nodes as indexes of node array, ptr to child[] row
} Node;

static char input[FSIZE];
static Node node[NODES];
static u32 child[NODES][CHILDREN];
static u32 cache[NODES];

// Zero can be legit cache value in part 2, so don't use that
static void resetcache(void)
{
    memset(cache, -1, sizeof cache);
}

static int cmp(const void *p, const void *q)
{
    const u32 a = *(const u32 *)p;
    const u32 b = *(const u32 *)q;
    if (a < b) return -1;
    if (a > b) return  1;
    return 0;
}

// Find index of node name (as char* or int* with same length data)
// Node::name must be first member, and name must exist in node array
static u32 nodeindex(const void *name, const u32 count)
{
    return (Node *)bsearch(name, node, count, sizeof *node, cmp) - node;
}

// Recursive DFS with memoization to count all paths from u to end
static u32 paths(const u32 u, const u32 end)
{
    if (u == end)
        return 1;
    if (cache[u] != UINT32_MAX)  // -1 is init value (="not cached yet")
        return cache[u];  // 0 for paths unable to reach the end
    u32 count = 0;
    for (u32 j = 0; j < node[u].len; ++j)
        count += paths(node[u].child[j], end);
    return (cache[u] = count);
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

    // Parse nodes with children
    u32 n = 0;                                    // node count
    for (const char *s = input; *s; ++n, ++s) {   // skip newline
        memcpy(&node[n].name, s, NAMELEN);        // avoid unaligned u32 cast
        node[n].name &= NAMEMASK;                 // null-terminate node name
        node[n].child = &child[n][0];
        const char *const ch0 = (s += NAMELEN);   // start of children at zeroth separator
        for (; *s != '\n'; s += NAMELEN);         // to newline
        memcpy(node[n].child, ch0 + 1, s - ch0);  // skip zeroth separator
        node[n].len = (s - ch0) / NAMELEN;        // child count
        for (u32 i = 0; i < node[n].len; ++i)
            node[n].child[i] &= NAMEMASK;         // null-terminate child names
    }
    // Add last node, unlisted because it has no children
    node[n++] = (Node){*(int32_t *)"out", 0, NULL};

    // Sort by node name and replace child node names with index of node array
    qsort(node, n, sizeof *node, cmp);
    for (u32 i = 0; i < n; ++i)
        for (u32 j = 0; j < node[i].len; ++j)
            node[i].child[j] = nodeindex(&node[i].child[j], n);

    // Part 1
    const u32 out = nodeindex("out", n);
    const u32 you = nodeindex("you", n);
    resetcache();  // must init at program start because "not cached" = -1
    printf("%u ", paths(you, out));  // 670

    // Part 2
    // Input is directed acyclic graph, so either fft->dac or dac->fft is
    // possible, not both. For my input, full path is svr->fft->dac->out
    // which is also the logical order from an electronics perspective.
    const u32 svr = nodeindex("svr", n);
    const u32 fft = nodeindex("fft", n);
    const u32 dac = nodeindex("dac", n);
    resetcache(); const u32 p1 = paths(svr, fft);  // 8049
    resetcache(); const u32 p2 = paths(fft, dac);  // 3377314
    resetcache(); const u32 p3 = paths(dac, out);  // 12215
    printf("%"PRIu64"\n", (u64)p1 * p2 * p3);   // 332052564714990

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
