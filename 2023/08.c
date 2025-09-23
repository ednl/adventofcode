/**
 * Advent of Code 2023
 * Day 8: Haunted Wasteland
 * https://adventofcode.com/2023/day/8
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=c17 -Wall -Wextra -pedantic 08alt.c
 *    gcc   -std=c17 -Wall -Wextra -pedantic 08alt.c
 * Enable timer:
 *    clang -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 08alt.c
 *    gcc   -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 08alt.c
 * Get minimum runtime from timer output:
 *     n=10000;m=999999;for((i=0;i<n;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i/$n)";done
 * Minimum runtime:
 *     Macbook Pro 2024 (M4 4.4 GHz)       : 121 µs
 *     Mac Mini 2020 (M1 3.2 GHz)          : 171 µs
 *     iMac 2013 (i5 Haswell 4570 3.2 GHz) :   ? µs
 *     Raspberry Pi 5 (2.4 GHz)            : 301 µs
 */

#include <stdio.h>     // getline (should perhaps check for availability)
#include <stdlib.h>    // free
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRId64
#include <stdbool.h>
#include <pthread.h>   // pthread_create, pthread_join
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
#define NODES (WORD * WORD * WORD)  // 32768 = simple thus fast array (but very sparse)
#define START (('A' & MASK) << BTWO | ('A' & MASK) << BITS | ('A' & MASK))
#define MAXTHREADS 16  // must be bigger than number of xxA nodes in input

static char *lr;  // L=left,R=right instructions from first line of input
static int node[NODES][2];
static int threadcount;  // number of xxA nodes in input
static int64_t xxA[MAXTHREADS];  // node index values with names ending in A
static pthread_t tid[MAXTHREADS];  // thread IDs

// Greatest Common Divisor
// https://en.wikipedia.org/wiki/Euclidean_algorithm
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

static inline bool end_not_z(const int64_t i)
{
    return (i & MASK) != ('Z' & MASK);
}

// Navigate the map from node at index i to first node that ends in Z
// For part 1, this works because the first xxZ node from AAA is ZZZ
// For part 2, this is good enough because the first xxZ node from xxA
// always loops back to the starting point; otherwise loop detection
// would be needed.
// Return: number of steps (as a void* because this the thread function)
static void *navigate(void *arg)
{
    int64_t i = (int64_t)arg, steps = 0;  // both could be int but compiler complains about cast
    for (const char *dir = lr; end_not_z(i); ++steps) {  // start all searches from beginning of LR instructions
        i = node[i][(int)*dir];  // *dir is 0 for L, 1 for R, cast to suppress warning
        if (*++dir == '\n')      // fetch next LR instruction, check for end
            dir = lr;            // loop around
    }
    return (void *)steps;  // return as thread compatible result
}

int main(void)
{
#ifdef TIMER
    starttimer();
#endif

    // Read and convert one line of 'L' and 'R' instructions to index 0 or 1
    FILE *f = fopen(NAME, "r");
    if (!f)
        return 1;
    size_t lr_size;
    int LR = getline(&lr, &lr_size, f) - 1;  // subtract 1 for '\n'
    if (LR < 1)
        return 2;
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
            xxA[threadcount++] = i;
    }
    fclose(f);
    free(buf);

    // Launch thread for each map navigation from node xxA to xxZ
    for (int i = 0; i < threadcount; ++i)
        pthread_create(&tid[i], NULL, navigate, (void *)xxA[i]);

    // Wait for threads to finish, take LCM of results
    int64_t part2 = 1;  // LCM(x,1) = x
    void *steps;  // thread return value
    for (int i = 0; i < threadcount; ++i) {
        pthread_join(tid[i], &steps);
        part2 = lcm(part2, (int64_t)steps);
        if (xxA[i] == START)  // AAA node may be anywhere
            printf("Part 1: %"PRId64"\n", (int64_t)steps);  // ex1: 2, ex2: 6, ex3: 2, input: 19631
    }
    printf("Part 2: %"PRId64"\n", part2);  // ex1: 2, ex2: 6, ex3: 6, input: 21003205388413
    free(lr);

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
}
