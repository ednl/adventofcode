/**
 * Advent of Code 2025
 * Day 11: Reactor
 * https://adventofcode.com/2025/day/11
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    cc -std=c17 -Wall -Wextra -pedantic 11.c
 * Enable timer:
 *    cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 11.c
 * Get minimum runtime from timer output in bash:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 100 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 172 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 278 µs
 */

#include <stdio.h>
#include <stdlib.h>    // qsort, malloc, free, exit
#include <string.h>    // strcmp, memcpy, memset
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRId64
#include <assert.h>    // static_assert
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
#define NODES (LINES + 1)  // unique node count
#define NAMESIZE 4         // 3 chars + '\0'
static_assert(sizeof(int) == NAMESIZE, "int is not 32-bit");  // 4=4

typedef struct node {
    int name;    // 4 bytes = string of len 3 + nul terminator
    int len;     // how many child nodes
    int *child;  // child nodes as index of node array
} Node;

static char input[FSIZE];
static Node node[NODES];
static int64_t cache[NODES];

// Zero can be legit cache value in part 2, so don't use that
static void resetcache(void)
{
    memset(cache, -1, sizeof cache);
}

// Find index of node name
// Node::name must be first member, node must exist
static int nodeindex(const void *name)
{
    return (Node *)bsearch(name, node, NODES, sizeof *node, (int(*)(const void *, const void *))strcmp) - node;
}

// Recursive DFS for all paths from start to goal
static int64_t paths(const int start, const int goal, const int avoid)
{
    if (start == goal)
        return 1;
    if (start == avoid)
        return 0;
    if (cache[start] >= 0)  // not cached = -1
        return cache[start];
    int64_t count = 0;
    for (int j = 0; j < node[start].len; ++j)
        count += paths(node[start].child[j], goal, avoid);
    return (cache[start] = count);
}

int main(void)
{
    // Read input file from disk
    FILE *f = fopen(FNAME, "rb");  // fread requires binary mode
    if (!f) { fprintf(stderr, "File not found: %s\n", FNAME); return 1; }
    fread(input, sizeof input, 1, f);  // read whole file at once
    fclose(f);

#if DEBUG
    fwrite(input, sizeof input, 1, stdout);
    printf("\n");
#endif

#ifdef TIMER
    starttimer();
#endif

    // Parse nodes and children
    char *c = input;
    for (int i = 0; i < LINES; ++i) {
        *(c + NAMESIZE - 1) = '\0';      // terminate node name
        memcpy(&node[i].name, c, NAMESIZE);
        c += NAMESIZE + 1;               // go to first child (+1 for ':')
        const char *const src = c;       // remember as src
        int children = 1;                // always at least one child for listed nodes
        for (; *(c + NAMESIZE - 1) != '\n'; c += NAMESIZE, ++children)
            *(c + NAMESIZE - 1) = '\0';  // terminate child name
        *(c + NAMESIZE - 1) = '\0';      // terminate last child name
        c += NAMESIZE;                   // c now points to start of next line
        const int bytes = children * NAMESIZE;
        void *dst = malloc(bytes);
#if DEBUG
        if (!dst) { fprintf(stderr, "Error: out of memory while parsing line %d\n", i + 1); exit(EXIT_FAILURE); }
#endif
        memcpy(dst, src, bytes);         // copy strings of len 3+'\0'=4 to int array, reinterpret later
        node[i].child = dst;
        node[i].len = children;
    }
    // Add last node, unlisted because it has no children
    node[LINES] = (Node){*(int *)"out", 0, NULL};

    // Sort by node name and replace child node names with index of node array
    qsort(node, NODES, sizeof *node, (int(*)(const void *, const void *))strcmp);
    for (int i = 0; i < NODES; ++i)
        for (int j = 0; j < node[i].len; ++j)
            node[i].child[j] = nodeindex(&node[i].child[j]);

#if EXAMPLE || DEBUG
    for (int i = 0; i < NODES; ++i) {
        printf("%s:", (char *)&node[i].name);
        for (int j = 0; j < node[i].len; ++j)
            printf(" %s", (char *)&node[node[i].child[j]].name);  // child is now index of node array
        printf("\n");
    }
    printf("\n");
#endif

    const int avoidnone = -1;
    const int out = nodeindex("out");
#if EXAMPLE != 2
    const int you = nodeindex("you");
    resetcache();  // must reset at start because "not cached" = -1
    printf("Part 1: %"PRId64"\n", paths(you, out, avoidnone));  // example: 5, input: 670
#endif

#if EXAMPLE != 1
    // Input is directed acyclic graph, so either fft->dac=0 or dac->fft=0
    // For my input, must pass fft first, so full path is svr->fft->dac->out
    // which means: avoid dac when searching for svr->fft,
    // otherwise way too many paths svr->dac that go nowhere
    const int svr = nodeindex("svr");
    const int fft = nodeindex("fft");
    const int dac = nodeindex("dac");
    resetcache();
    const int64_t p1 = paths(svr, fft, dac);        // input: 8049
    resetcache();
    const int64_t p2 = paths(fft, dac, avoidnone);  // input: 3377314
    resetcache();
    const int64_t p3 = paths(dac, out, avoidnone);  // input: 12215
    printf("Part 2: %"PRId64"\n", p1 * p2 * p3);    // example: 2, input: 332052564714990
#endif

#if DEBUG || !defined(TIMER)  // otherwise leave cleanup to OS
    for (int i = 0; i < NODES; ++i) {
        free(node[i].child);  // node["out"].child is already null but that's fine
        node[i].child = NULL;
    }
#endif

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
}
