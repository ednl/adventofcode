/**
 * Advent of Code 2024
 * Day 9: Disk Fragmenter
 * https://adventofcode.com/2024/day/9
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -Wall -Wextra 09.c
 *    gcc   -std=gnu17 -Wall -Wextra 09.c
 * Enable timer:
 *    clang -DTIMER -O3 -march=native 09.c ../startstoptimer.c
 *    gcc   -DTIMER -O3 -march=native 09.c ../startstoptimer.c
 * Get minimum runtime from timer output:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Mac Mini 2020 (M1 3.2 GHz)                       :   ? µs
 *     Raspberry Pi 5 (2.4 GHz)                         :   ? µs
 *     Macbook Air 2013 (Core i5 Haswell 4250U 1.3 GHz) :   ? µs
 *     Raspberry Pi 4 (1.8 GHz)                         :   ? µs
 */

#include <stdio.h>
#include <stdint.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define EXAMPLE 1
#if EXAMPLE
    #define N 20
    static char map[N] = "2333133121414131402";
#else
    #define FNAME "../aocinput/2024-09-input.txt"
    #define N 20000
    static char map[N];
#endif
#define M (N >> 1)

static const uint32_t tri[] = {0,0,1,3,6,10,15,21,28,36};
static uint32_t file[M], free[M];

int main(void)
{
#if !EXAMPLE
    FILE *f = fopen(FNAME, "rb");
    if (!f) { fprintf(stderr, "File not found: %s\n", FNAME); return 1; }
    fread(map, sizeof map, 1, f);
    fclose(f);
#endif

#ifdef TIMER
    starttimer();
#endif

    for (int i = 0, j = 1, k = 0; i < N; i += 2, j += 2, ++k) {
        file[k] = map[i] &= 15;
        free[k] = map[j] &= 15;
    }

    size_t chksum = 0, pos = 0, head = 0, tail = M - 1, space, blocks;
    while (head < tail) {
        // (pos + pos+1 + ... + pos+n-1)*ID = (n*pos + 0+1+..+n-1)*ID
        chksum += head * (file[head] * pos + tri[file[head]]);  // ID = head, n = file[head], triangular_number(n-1)
        pos += file[head];  // move to next free space
        space = free[head];  // length of free space
        blocks = file[tail];  // blocks to move
        while (space >= blocks) {
            space -= blocks;
            chksum += tail * (file[tail] * pos + tri[file[tail]]);  // ID = head, n = file[head], triangular_number(n-1)
            pos += file[tail];
            blocks = file[--tail];
        }
        //TODO
        pos += free[head];
        ++head;
    }

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
    return 0;
}
