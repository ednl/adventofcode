/**
 * Advent of Code 2022
 * Day 12: Hill Climbing Algorithm
 * https://adventofcode.com/2022/day/12
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 12.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 12.c
 * Test output with timer enabled:
 *     ./a.out | tail -n7
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  8.5 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 12.2 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 41.9 µs
 */

#include <stdio.h>
#include <string.h>  // memset
#include <stdint.h>  // int16_t
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

// Custom input parameters
#define FNAME "../aocinput/2022-12-input.txt"
#define R  41       // rows in input
#define C 159       // cols in input (without newlines)

// Derived values
#define PR (R + 2)  // padded rows
#define PC (C + 1)  // padded cols
#define END ((PR - 1) * PC - 1)  // index in 'alt' of last newline in input

// Assumptions about input!
#define S ((PR >> 1) * PC)  // index in 'alt' of letter S (start of middle line)
#define QSIZE 64    // must be power of 2 (needed for my input: 42)

typedef struct queue {
    int head, tail;
    int data[QSIZE];
} Queue;

static const int step[] = {-1, PC, -PC, 1};
static char alt[PR * PC];
static uint16_t dist[PR * PC];  // shorter data type for faster reset
static Queue queue;

// Assume queue is never full
static void enq(const int val)
{
    queue.data[queue.head++] = val;
    queue.head &= (QSIZE - 1);
}

// Assume queue is never full (or else head==tail)
static bool deq(int *const val)
{
    if (queue.head != queue.tail) {
        *val = queue.data[queue.tail++];
        queue.tail &= (QSIZE - 1);
        return true;
    }
    return false;
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    fread(&alt[PC], R * PC, 1, f);  // read input as 1 chunk partway into alt
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
    // Reset BFS data at start of timing loop
    queue.head = queue.tail = 0;
    memset(dist, 0, sizeof dist);
#endif

    // Add border
    memset(alt, '#', PC);
    memset(&alt[END], '#', PC + 1);
    for (int i = 2 * PC - 1; i < END; i += PC)
        alt[i] = '#';

    // Find 'E'
    int cur = S + PC - 2;  // last index in the same row as S
    for (; alt[cur] != 'E'; cur--);
#ifdef TIMER
    const int posE = cur;  // save position to reset later
#endif

    alt[S] = 'a';
    alt[cur] = 'z';
    dist[cur] = 1;  // unseen: dist=0, so start at 1
    uint16_t firsta = 0;
    do {
        const char nextalt = alt[cur] - 1;
        const uint16_t nextdist = dist[cur] + 1;
        for (int i = 0; i < 4; ++i) {
            const int next = cur + step[i];
            if (alt[next] >= nextalt && !dist[next]) {  // descent<=1 (level or climb is fine) and not seen?
                dist[next] = nextdist;
                if (alt[next] == 'a' && !firsta)  // first 'a'?
                    firsta = nextdist;  // part 2
                if (next == S)  // goal? (= index in 'alt' of letter S)
                    goto done;  // part 1
                enq(next);
            }
        }
    } while (deq(&cur));
done:
    // unseen: dist=0, so all distances are 1 too high
    printf("%d %d\n", dist[S] - 1, firsta - 1);  // 504 500

#ifdef TIMER
    // Reset input file for next timing loop
    // no need to reset 'S' because index is fixed
    alt[posE] = 'E';
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
