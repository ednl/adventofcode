/**
 * Advent of Code 2023
 * Day 8: Haunted Wasteland
 * https://adventofcode.com/2023/day/8
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -O3 -march=native -Wall -Wextra 08.c ../startstoptimer.c
 *    gcc   -std=gnu17 -O3 -march=native -Wall -Wextra 08.c ../startstoptimer.c
 * Get minimum runtime:
 *     m=999999;for((i=0;i<5000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * Minimum runtime:
 *     Mac Mini 2020 (M1 3.2 GHz)          : 432 µs
 *     iMac 2013 (i5 Haswell 4570 3.2 GHz) : 488 µs
 *     Raspberry Pi 5 (2.4 GHz)            : 556 µs
 *     Raspberry Pi 4 (1.8 GHz)            : 993 µs
 */

#include <stdio.h>     // fopen, fclose, fgets, fgetc, printf, fputc, puts
#include <stdlib.h>    // qsort
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRId64
#include "../startstoptimer.h"

#define EXAMPLE 0
#if EXAMPLE
    #define DEBUG
    #define STR_HELPER(x) #x
    #define STR(x) STR_HELPER(x)
    #define NAME "../aocinput/2023-08-example"STR(EXAMPLE)".txt"
    #if EXAMPLE == 1
        #define LR    2
        #define NODES 7
    #elif EXAMPLE == 2
        #define LR    3
        #define NODES 3
    #elif EXAMPLE == 3
        #define LR    2
        #define NODES 8
    #endif
#else
    // #define DEBUG  // uncomment or compile with -DDEBUG
    #define NAME "../aocinput/2023-08-input.txt"
    #define LR    293  // number of LR instructions
    #define NODES 786  // number of nodes, each with two neighbours
#endif

static char lr[LR + 1];  // +1 for '\0'
static int node[NODES][3];  // each node consists of 3 IDs (hashes of node names)

// Greatest Common Divisor
static int64_t gcd(int64_t a, int64_t b)
{
	int64_t t;
	while (b) {
		t = b;
		b = a % b;
		a = t;
	}
	return a;
}

// Least Common Multiple
static int64_t lcm(int64_t a, int64_t b)
{
	return a / gcd(a, b) * b;
}

// Qsort helper: sort nodes by node number ascending
// p,q actually point to int[3] but we need p[0],q[0] anyway, so this works
static int node_asc(const void *p, const void *q)
{
    const int a = *(const int*)p;
    const int b = *(const int*)q;
    if (a < b) return -1;
    if (a > b) return  1;
    return 0;
}

// Convert 3 chars to int ID ("hash value")
static int str2int(const char *s)
{
    int n = 0;
    for (int i = 0; i < 3; ++i)
        n = (n << 8) + *s++;  // store chars in bytes, first char ends up in MSByte
    return n;
}

#ifdef DEBUG
// Output int as 3 chars to stdout
static void printnode(int n)
{
    for (int shift = 24; shift; )
        fputc(n >> (shift -= 8) & 0xff, stdout);
}
#endif

// Binsearch in sorted node array for name n ("hash table")
// All data is consistent, so a node will always be found
// Return: node index i where node[i][0] == n
static int nodeindex(const int n)
{
    int l = 0, r = NODES - 1;
    if (n == node[l][0]) return l;
    if (n == node[r][0]) return r;
    // Now always true: node[l][0] < n < node[r][0]
    while (r - l != 1) {
        int m = ((l + 1) >> 1) + (r >> 1);  // avoid index overflow
        if      (n > node[m][0]) l = m;
        else if (n < node[m][0]) r = m;
        else return m;
    }
    return -1;  // should not happen for this data
}

// Walk from node at index i to first node that ends in Z
// For part 1, this works because the first xxZ node is ZZZ
// For part 2, this is good enough because the first xxZ node always loops
// back to the starting point; otherwise loop detection would be needed
// Return: number of steps
static int walk(int i)
{
    const char *go = lr;  // start all searches from beginning of LR instructions
    int steps = 0;
    while ((node[i][0] & 0xff) != 'Z') {
        i = node[i][(int)*go];  // *go is 1 for L, 2 for R
        if (!*++go)             // fetch next LR instruction, check for end
            go = lr;            // loop around
        ++steps;
    }
    return steps;
}

int main(void)
{
    starttimer();
    FILE *f = fopen(NAME, "r");
    if (!f) { fputs("File not found.\n", stderr); return 1; }

    fgets(lr, sizeof lr, f);  // one long line of LR instructions, excludes '\n'
    fgetc(f); fgetc(f);  // newline, empty line
    char buf[32];
    for (int i = 0; i < NODES; ++i) {
        fgets(buf, sizeof buf, f);       // read whole line
        node[i][0] = str2int(buf);       // node name to number
        node[i][1] = str2int(buf + 7);   // next L node name to number
        node[i][2] = str2int(buf + 12);  // next R node name to number
    }
    fclose(f);

    #ifdef DEBUG
    printf("%s\n\n", lr);
    #endif

    // Transform LR instructions from ['L','R'] to [1,2] (=index into node entry)
    for (int i = 0; i < LR; ++i)
        lr[i] = (char)(lr[i] >> 3 & 3);  // 'L'=0b1001100 => 1, 'R'=0b1010010 => 2

    #ifdef DEBUG
    for (int i = 0; i < NODES; ++i) {
        printf("%3d: ", i);
        printnode(node[i][0]);
        printf(" = (");
        printnode(node[i][1]);
        fputc(',', stdout);
        printnode(node[i][2]);
        puts(")");
    }
    printf("\n");
    #endif

    // Sort nodes by node number (=node[][0])
    qsort(node, NODES, sizeof *node, node_asc);  // *node is an int[3] so size is 12

    // Transform next node numbers to index of node array
    for (int i = 0; i < NODES; ++i) {
        node[i][1] = nodeindex(node[i][1]);
        node[i][2] = nodeindex(node[i][2]);
    }

    #ifdef DEBUG
    printf("\n");
    for (int i = 0; i < NODES; ++i)
        printf("%3d: %7d = (%3d,%3d)\n", i, node[i][0], node[i][1], node[i][2]);
    printf("\n");
    #endif

    const int part1 = walk(0);  // node AAA is at index 0
    printf("Part 1: %d\n", part1);  // ex1: 2, ex2: 6, ex3: 2, input: 19631

    // Part 2
    // For every xxA node, determine no. of steps to first xxZ node, then take LCM of those
    int64_t part2 = part1;  // node AAA from part 1
    for (int i = 1; i < NODES; ++i)  // skip node AAA at index 0
        if ((node[i][0] & 0xff) == 'A') {
            part2 = lcm(part2, walk(i));
            #ifdef DEBUG
            printf("%d: len=%d lcm=%"PRId64"\n", i, walk(i), part2);
            #endif
        }
    printf("Part 2: %"PRId64"\n", part2);  // ex1: 2, ex2: 6, ex3: 6, input: 21003205388413
    printf("Time: %.0f us\n", stoptimer_us());
    return 0;
}
