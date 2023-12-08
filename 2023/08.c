/**
 * Advent of Code 2023
 * Day 8: Haunted Wasteland
 * https://adventofcode.com/2023/day/8
 * By: E. Dronkert https://github.com/ednl
 */

#include <stdio.h>     // fopen, fclose, fgets, fscanf, printf
#include <stdlib.h>    // qsort
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRId64

#define EXAMPLE 0
#if EXAMPLE
    #define DEBUG 1
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
    #define DEBUG 0
    #define NAME "../aocinput/2023-08-input.txt"
    #define LR    293
    #define NODES 786
#endif

static char lr[LR + 1];  // +'\0'
static int node[NODES][3];

// Greatest Common Divisor
static int64_t gcd(int64_t a, int64_t b)
{
	int64_t t;
	while (b)
	{
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

static int str2int(const char* s)
{
    int n = 0;
    for (int i = 0; i < 3; ++i)
        n = (n << 8) + *s++;  // store chars in bytes, first char in MSByte
    return n;
}

#if DEBUG
static char* int2str(int n)
{
    static char s[4] = {0};
    for (int i = 3; i > 0; n >>= 8)
        s[--i] = (char)(n & 0xff);
    return s;
}
#endif

// Binsearch in sorted node array for index i where node[i][0] == n
static int index(const int n)
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

// Walk from node at index n to first node that ends in Z
//   return: number of steps
static int walk(int n)
{
    const char* go = lr;  // start all searches from beginning of LR instructions
    int steps = 0;
    while ((node[n][0] & 0xff) != 'Z') {
        n = node[n][(int)*go];  // *go is 1 for L, 2 for R
        if (!*++go)             // next LR instruction, check for end
            go = lr;            // loop around
        ++steps;
    }
    return steps;
}

int main(void)
{
    FILE* f = fopen(NAME, "r");
    if (!f)
        return 1;
    fgets(lr, sizeof lr, f);  // one long line of LR instructions
    fgetc(f); fgetc(f);  // empty line
    char buf[32];
    for (int i = 0; i < NODES; ++i) {
        fgets(buf, sizeof buf, f);  // next node line
        node[i][0] = str2int(buf);
        node[i][1] = str2int(buf + 7);
        node[i][2] = str2int(buf + 12);
    }
    fclose(f);

    #if DEBUG
    printf("%s\n\n", lr);
    #endif

    // Transform LR instructions from ['L','R'] to [1,2] (=index into node entry)
    for (int i = 0; i < LR; ++i)
        lr[i] = (char)(1 + (lr[i] >> 1 & 1));  // 'L'=0b1001100 => 1, 'R'=0b1010010 => 2

    #if DEBUG
    for (int i = 0; i < NODES; ++i) {
        printf("%3d: %s = (", i, int2str(node[i][0]));
        printf("%s", int2str(node[i][1]));
        printf(",%s) -> %7d = (%7d,%7d)\n" , int2str(node[i][2]), node[i][0], node[i][1], node[i][2]);
    }
    printf("\n");
    #endif

    // Sort nodes by node number (=node[][0])
    qsort(node, NODES, sizeof *node, node_asc);  // *node is an int[3] so size is 12

    // Transform next node names to index of node array
    for (int j = 1; j < 3; ++j)
        for (int i = 0; i < NODES; ++i)
            node[i][j] = index(node[i][j]);

    #if DEBUG
    printf("\n");
    for (int i = 0; i < NODES; ++i)
        printf("%3d: %7d = (%3d,%3d)\n", i, node[i][0], node[i][1], node[i][2]);
    printf("\n");
    #endif

    const int part1 = walk(0);  // node AAA is at index 0
    printf("Part 1: %d\n", part1);  // ex1: 2, ex2: 6, ex3: 2, input: 19631

    int64_t part2 = part1;  // node AAA already determined in part 1
    for (int i = 1; i < NODES; ++i)  // skip index 0
        if ((node[i][0] & 0xff) == 'A') {
            const int len = walk(i);
            part2 = lcm(part2, len);
            #if DEBUG
            printf("%d: len=%d lcm=%"PRId64"d\n", i, len, part2);
            #endif
        }

    printf("Part 2: %"PRId64"\n", part2);  // ex1: 2, ex2: 6, ex3: 6, input: 21003205388413
    return 0;
}
