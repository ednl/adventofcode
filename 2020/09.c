/**
 * Advent of Code 2020
 * Day 8: Handheld Halting
 * https://adventofcode.com/2020/day/8
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic -Wno-char-subscripts 08.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 08.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  9.77 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 13.76 µs
 *     Raspberry Pi 5 (2.4 GHz)      :  ? µs
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2020-09-input.txt"
#define FSIZE 8824
#define LEN 1000
#define PRE 25

static char input[FSIZE];
static int data[LEN];
// static int pair[PRE][PRE];

static int parseint(const char **s)
{
    int x = *(*s)++ & 15;
    while (**s != '\n')
        x = x * 10 + (*(*s)++ & 15);
    (*s)++;  // skip newline
    return x;
}

// Brute force
static bool valid(const int k)
{
    for (int i = k - PRE; i < k - 1; ++i) {
        const int res = data[k] - data[i];
        for (int j = i + 1; j < k; ++j)
            if (data[j] == res)
                return true;
    }
    return false;
}

// Pre-calculated pairwise sums instead of doing the same sums over & over
// static bool valid(const int k)
// {
//     for (int i = 0; i < PRE - 1; ++i)
//         for (int j = i + 1; j < PRE; ++j)
//             if (pair[i][j] == data[k])
//                 return true;
//     return false;
// }

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    fread(input, FSIZE, 1, f);
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    // Parse i32
    // uint64_t *const m = (uint64_t *)input;
    // for (int i = 0; i < (FSIZE >> 3); ++i)
    //     m[i] &= UINT64_C(0x0f0f0f0f0f0f0f0f);  // pre-mask all bytes, newline unaffected
    const char *c = input;
    for (int i = 0; (data[i] = parseint(&c)) > 0; ++i);

    // Part 1
    // Fill triangle circular buffer for first run
    // for (int i = 0; i < PRE - 1; ++i)
    //     for (int j = i + 1; j < PRE; ++j)
    //         pair[i][j] = data[i] + data[j];
    int k = PRE;
    // int upd = 0;
    while (valid(k)) {
        // Update triangle circular buffer
        // const int64_t diff = data[k] - data[k - PRE];
        // for (int i = 0; i < upd; ++i)
        //     pair[i][upd] += diff;  // update col
        // for (int j = upd + 1; j < PRE; ++j)
        //     pair[upd][j] += diff;  // update row
        // if (++upd == PRE)
        //     upd = 0;
        // Next k
        k++;
    }
    printf("%d", data[k]);  // part 1: 15353384 (k=502)

    // Part 2
    int i = k - 1, j = k - 1;
    int sum = data[i];
    while (sum != data[k]) {
        while (sum < data[k])
            sum += data[--i];
        while (sum > data[k])
            sum -= data[j--];
    }
    // my input: i=384, j=400
    int min = data[i], max = data[i];
    for (++i; i <= j; ++i) {
        if (data[i] < min) min = data[i];
        if (data[i] > max) max = data[i];
    }
    printf(" %d\n", min + max);  // part 2: 2466556

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
