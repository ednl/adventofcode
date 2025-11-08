/**
 * Advent of Code 2024
 * Day 23: LAN Party
 * https://adventofcode.com/2024/day/23
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -Wall -Wextra 23.c
 *    gcc   -std=gnu17 -Wall -Wextra 23.c
 * Enable timer:
 *    clang -DTIMER -O3 -march=native 23.c ../startstoptimer.c
 *    gcc   -DTIMER -O3 -march=native 23.c ../startstoptimer.c
 * Get minimum runtime from timer output:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz)                    :  266 µs
 *     Mac Mini 2020 (M1 3.2 GHz)                       :  366 µs
 *     Raspberry Pi 5 (2.4 GHz)                         :  585 µs
 *     Macbook Air 2013 (Core i5 Haswell 4250U 1.3 GHz) :  818 µs
 *     Raspberry Pi 4 (1.8 GHz)                         : 2657 µs
 */

#include <stdio.h>
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define EXAMPLE 0
#if EXAMPLE == 1
    #define FNAME "../aocinput/2024-23-example.txt"
    #define N 32
#else
    #define FNAME "../aocinput/2024-23-input.txt"
    #define N 3380
#endif
#define FSIZE (N * 6)  // 5 characters per line +newline
#define RANGE 826  // min ID: makeid("aa")=0, max ID: makeid("zz")=825
#define MAXNB  13  // max (and also min) edge count per node in my input
#define FIRST (((1 << 5) - 1) << 5)  // mask for first letter of node name (=0b1111100000=992)
#define T_    (('t' - 'a') << 5)     // first letter of node name is 't'

static char input[FSIZE];
static bool node[RANGE];        // which node IDs are present in my input
static bool adj[RANGE][RANGE];  // adjacency matrix
static int id[RANGE];           // condensed list of nodes present in my input (range used: 0..519)
static int ids;                 // how many different nodes present in my input (my input: 520)
static int nb[RANGE][MAXNB];    // adjacency list: adj[x][y]=1 => nb[z][0..12]=y where z=0..519 and id[z]=x

// Node name to ID
static int makeid(const char *const s)
{
    return (*s - 'a') << 5 | (*(s + 1) - 'a');
}

// Node ID to name
static void printid(const int id)
{
    printf("%c%c", 'a' + (id >> 5), 'a' + (id & 31));
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");  // binary mode needed for fread
    if (!f) { fprintf(stderr, "File not found: %s\n", FNAME); return 1; }
    fread(input, sizeof input, 1, f);  // read whole file in one go
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    // Parse input, identify nodes and edges
    const char *c = input;
    for (int n = 0; n < N; ++n) {
        const int a = makeid(c);
        const int b = makeid(c + 3);
        node[a] = node[b] = true;
        adj[a][b] = adj[b][a] = true;
        c += 6;
    }

    // Condense all nodes and edges into node list and adjacency list
    for (int i = 0; i < RANGE; ++i)    // for every possible node ID
        if (node[i]) {                 // if node present in input
            for (int j = 0, k = 0; j < RANGE && k < MAXNB; ++j)  // check every possible edge for this node
                if (adj[i][j])         // if edge present in input
                    nb[ids][k++] = j;  // save to adjacency list (automatically sorted)
            id[ids++] = i;             // save to node list (automatically sorted)
        }

#if EXAMPLE
    for (int i = 0; i < ids; ++i) {
        printid(id[i]);
        putchar(':');
        for (int j= 0; j < MAXNB && (!j || nb[i][j]); ++j) {
            putchar(' ');
            printid(nb[i][j]);
        }
        putchar('\n');
    }
    putchar('\n');
#endif

    // Part 1: count distinct totally connected node triplets where at least one starts with 't'
    int t = 0, count = 0;
    for (; (id[t] & FIRST) != T_; ++t);   // skip all nodes with names that don't start with 't'
    for (; (id[t] & FIRST) == T_; ++t) {  // check all nodes with names that start with 't'
        const int *const a = nb[t];       // convenience pointer to all neighbours of current 't_' node
        for (int i = 1; i < MAXNB && a[i]; ++i)  // check every pair of neighbour nodes for edge
            for (int j = 0; j < i; ++j)   // but avoid double counting combinations with previous 't_' nodes
                if (adj[a[i]][a[j]] && (a[i] > id[t] || (a[i] & FIRST) != T_) && (a[j] > id[t] || (a[j] & FIRST) != T_))
                    ++count;
    }
    printf("Part 1: %d\n", count);  // example: 7, input: 1599

    // Part 2: find largest clique, print as sorted CSV.
    // Overview but unused: https://en.wikipedia.org/wiki/Clique_problem
    // Instead, count number of connected pairs in neighbours of each node
    // and collect nodes with max pair count.
    int password[MAXNB] = {0};
    int maxpairs = 0, pwdlen = 0;
    for (int i = 0; i < ids; ++i) {
        const int *const a = nb[i];  // convenience pointer to array of neighbour nodes
        int pairs = 0;
        for (int j = 1; j < MAXNB && a[j]; ++j)
            for (int k = 0; k < j; ++k)
                if (adj[a[j]][a[k]])
                    ++pairs;
        if (pairs > maxpairs) {
            maxpairs = pairs;
            password[0] = id[i];
            pwdlen = 1;
        } else if (pairs == maxpairs && adj[id[i]][password[0]])  // extra check for example
            password[pwdlen++] = id[i];
    }
    printf("Part 2: ");
    printid(password[0]);
    for (int i = 1; i < pwdlen; ++i) {
        putchar(',');
        printid(password[i]);
    }
    putchar('\n');  // example: co,de,ka,ta; input: av,ax,dg,di,dw,fa,ge,kh,ki,ot,qw,vz,yw

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
    return 0;
}
