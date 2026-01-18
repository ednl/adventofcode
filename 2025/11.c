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
 * Get minimum runtime from timer output in bash:
 *     m=9999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 100 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 172 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 278 µs
 */

#include <stdio.h>
#include <stdlib.h>    // qsort, malloc
#include <string.h>    // strcmp, memcpy, memset
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRId64
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define EXAMPLE 0
#if EXAMPLE == 1
    #define FNAME "../aocinput/2025-11-example.txt"
    #define FSIZE 118
    #define LINES 10   // lines in example 1 file
#elif EXAMPLE == 2
    #define FNAME "../aocinput/2025-11-example2.txt"
    #define FSIZE 129
    #define LINES 13   // lines in example 2 file
#else
    #define FNAME "../aocinput/2025-11-input.txt"
    #define FSIZE 9448
    #define LINES 560  // lines in input file
#endif
#define NODES (LINES + 1)  // unique node count (+"out")
#define STRLEN 3           // node name are 3 chars
#define NAME (STRLEN + 1)  // +'\0'

// Node::name must be first member, for qsort() and bsearch()
typedef struct node {
    int32_t name;    // must be 32-bit = 4 bytes = string of len 3 +'\0'
    int32_t len;     // child node count
    int32_t *child;  // child nodes as indexes of node array
} Node;

static char input[FSIZE];
static Node node[NODES];
static int64_t cache[NODES];

// Zero can be legit cache value in part 2, so don't use that
static void resetcache(void)
{
    memset(cache, -1, sizeof cache);
}

// Find index of node name (as char* or int* with same length data)
// Node::name must be first member, and name must exist in node array
static int nodeindex(const void *name)
{
    // Use standard strcmp as comparison function, but needs to have void* params
    return (Node *)bsearch(name, node, NODES, sizeof *node, (int(*)(const void *, const void *))strcmp) - node;
}

// Recursive DFS with memoization to count all paths from u to end
static int64_t paths(const int u, const int end)
{
    if (u == end)
        return 1;
    if (cache[u] != -1)  // -1 is init value (="not cached yet")
        return cache[u];  // 0 for paths unable to reach end
    int64_t count = 0;
    for (int j = 0; j < node[u].len; ++j)
        count += paths(node[u].child[j], end);
    return (cache[u] = count);
}

int main(void)
{
    // Read input file from disk
    FILE *f = fopen(FNAME, "rb");  // fread requires binary mode
    if (!f) { fprintf(stderr, "File not found: %s\n", FNAME); return 1; }
    fread(input, sizeof input, 1, f);  // read whole file at once
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    // Parse nodes and children
    char *c = input;
    for (int i = 0; i < LINES; ++i) {
        *(c + STRLEN) = '\0';               // terminate node name by replacing ':'
        memcpy(&node[i].name, c, NAME);     // avoid awkward casts by using memcpy
        c += NAME + 1;                      // go to first child (+1 to skip space)
        const char *const src = c;          // remember as src
        int children = 1;                   // always at least one child for listed nodes
        for (; *(c + STRLEN) != '\n'; c += NAME, ++children)
            *(c + STRLEN) = '\0';           // terminate child name
        *(c + STRLEN) = '\0';               // terminate last child name
        c += NAME;                          // c now points to start of next line
        const int bytes = children * NAME;  // NAME must be sizeof(int32_t)
        void *dst = malloc(bytes);          // if this fails, you have bigger problems
        memcpy(dst, src, bytes);            // copy strings of len 3+'\0'=4 to int32 array
        node[i].child = dst;
        node[i].len = children;
    }
    // Add last node, unlisted because it has no children
    node[LINES] = (Node){*(int32_t *)"out", 0, NULL};

    // Sort by node name and replace child node names with index of node array.
    // Use standard strcmp as comparison function, but needs to have void* params
    qsort(node, NODES, sizeof *node, (int(*)(const void *, const void *))strcmp);
    for (int i = 0; i < NODES; ++i)
        for (int j = 0; j < node[i].len; ++j)
            node[i].child[j] = nodeindex(&node[i].child[j]);

    // Part 1
    const int out = nodeindex("out");
#if EXAMPLE != 2  // example 2 does not have "you" node
    const int you = nodeindex("you");
    resetcache();  // must init at program start because "not cached" = -1
    printf("Part 1: %"PRId64"\n", paths(you, out));  // example: 5, input: 670
#endif

    // Part 2
#if EXAMPLE != 1  // example 1 does not have svr, fft or dac nodes
    // Input is directed acyclic graph, so either fft->dac or dac->fft is
    // possible, not both. For my input, full path is svr->fft->dac->out.
    const int svr = nodeindex("svr");
    const int fft = nodeindex("fft");
    const int dac = nodeindex("dac");
    resetcache();
    const int64_t p1 = paths(svr, fft);  // input: 8049
    resetcache();
    const int64_t p2 = paths(fft, dac);  // input: 3377314
    resetcache();
    const int64_t p3 = paths(dac, out);  // input: 12215
    printf("Part 2: %"PRId64"\n", p1 * p2 * p3);  // example: 2, input: 332052564714990
#endif
    // leave cleanup of malloc'ed memory to OS

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
}
