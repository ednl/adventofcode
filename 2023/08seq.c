/**
 * Advent of Code 2023
 * Day 8: Haunted Wasteland
 * https://adventofcode.com/2023/day/8
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=c17 -Wall -Wextra -pedantic 08seq.c
 *    gcc   -std=c17 -Wall -Wextra -pedantic 08seq.c
 * Enable timer:
 *    clang -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 08seq.c
 *    gcc   -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 08seq.c
 * Get minimum runtime from timer output:
 *     n=10000;m=999999;for((i=0;i<n;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i/$n)";done
 * Minimum runtime:
 *     Macbook Pro 2024 (M4 4.4 GHz)       : 212 µs
 *     Mac Mini 2020 (M1 3.2 GHz)          : 276 µs
 *     iMac 2013 (i5 Haswell 4570 3.2 GHz) :   ? µs
 *     Raspberry Pi 5 (2.4 GHz)            : 409 µs
 */

#include <stdio.h>     // getline (should probably check for availability)
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRId64
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define EXAMPLE 0  // 0=full input, also available: 1,2,3 = example 1,2,3
#if EXAMPLE
    #define STR_HELPER(x) #x
    #define STR(x) STR_HELPER(x)
    #define NAME "../aocinput/2023-08-example"STR(EXAMPLE)".txt"
#else
    #define NAME ("../aocinput/2023-08-input.txt")
#endif
#define BITS  (5)  // range 1..26 fits in 5 bits
#define BTWO  (BITS << 1)  // twice the bits
#define WORD  (1 << BITS)  // 32
#define MASK  (WORD - 1)   // 31
#define NODES (WORD * WORD * WORD)  // 32768
#define START (('A' & MASK) << BTWO | ('A' & MASK) << BITS | ('A' & MASK))
#define MAXSTARTERS 8  // must be bigger than number of xxA nodes in input (=6)

static char *lr;  // L=left,R=right instructions from first line of input
static int node[NODES][2];  // simple indexing = fast array (but VERY sparse)
static int startnodes;  // how many xxA nodes in input
static int xxA[MAXSTARTERS];  // index values of nodes with names ending in A

// Greatest Common Divisor
// https://en.wikipedia.org/wiki/Euclidean_algorithm
static int64_t gcd(int64_t a, int64_t b)
{
    while (b) {
        const int64_t tmp = b;
        b = a % b;
        a = tmp;
    }
    return a;
}

// Least Common Multiple
// https://en.wikipedia.org/wiki/Least_common_multiple#Using_the_greatest_common_divisor
static int64_t lcm(const int64_t a, const int64_t b)
{
    return a / gcd(a, b) * b;
}

// Hash 3 uppercase letters to 15-bit value
// 5-bit mask: 'A' = 1, 'Z' = 26
// "AAA" = (( 1 * 32) +  1) * 32 +  1 =  1024 +  32 +  1 =  1057
// "ZZZ" = ((26 * 32) + 26) * 32 + 26 = 26624 + 832 + 26 = 27482
static int hash(const char *s)
{
    return (s[0] & MASK) << BTWO | (s[1] & MASK) << BITS | (s[2] & MASK);
}

// Check if index i is to a node whose name does NOT end in 'Z'
static bool end_not_z(const int i)
{
    return (i & MASK) != ('Z' & MASK);
}

// Navigate the map from node at index to first node that ends in Z
// For part 1, this works because the first xxZ node from AAA is ZZZ
// For part 2, this is good enough because the first xxZ node from xxA
// always loops back to the starting point; otherwise loop detection
// would be needed.
// Return: number of steps
static int navigate(int index)
{
    int steps = 0;  // could be int16 but compiler complains about cast
    for (const char *dir = lr; end_not_z(index); ++steps) {  // start search from start of LR instructions
        index = node[index][(int)*dir];  // *dir is 0 (L) or 1 (R), cast to suppress "char index" warning
        if (*++dir == '\n')  // fetch next LR instruction, check for end
            dir = lr;        // loop around
    }
    return steps;
}

int main(void)
{
#ifdef TIMER
    starttimer();
#endif

    // Read and convert one line of 'L' and 'R' instructions to index 0 or 1
    FILE *f = fopen(NAME, "r");
    if (!f)
        return 1;  // file not found
    size_t lr_size;
    int LR = getline(&lr, &lr_size, f) - 1;  // subtract 1 for '\n'
    if (LR < 1)
        return 2;  // line 1 empty, or read error
    for (int i = 0; i < LR; ++i)  // leave '\n' intact
        lr[i] = lr[i] >> 1 & 1;   // 'L'=0b1001100 => 0, 'R'=0b1010010 => 1

    // Skip empty line
    fgetc(f);

    // Read and convert named nodes to "hashed" index values
    // Save nodes ending in A
    char *buf = NULL;
    size_t bufsize;
    while (getline(&buf, &bufsize, f) > 1) {  // e.g.: "ABC = (DEF, GHI)\n"
        const int i = hash(buf);
        node[i][0] = hash(buf + 7);
        node[i][1] = hash(buf + 12);
        if (buf[2] == 'A')  // save nodes ending in 'A'
            xxA[startnodes++] = i;  // for my input, startnodes goes up to 6
    }
    // fclose(f);  // file was opened read-only, safe to leave clean-up to OS
    // free(buf);  // leave clean-up to OS

    int64_t part2 = 1;  // neutral start because LCM(x,1) = x
    for (int i = 0; i < startnodes; ++i) {
        const int steps = navigate(xxA[i]);
        part2 = lcm(part2, steps);
        if (xxA[i] == START)  // AAA node could be anywhere in input
            printf("Part 1: %d\n", steps);  // ex1: 2, ex2: 6, ex3: 2, input: 19631
    }
    printf("Part 2: %"PRId64"\n", part2);  // ex1: 2, ex2: 6, ex3: 6, input: 21003205388413
    // free(lr);  // leave clean-up to OS

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
}
