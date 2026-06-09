/**
 * Advent of Code 2020
 * Day 9: Encoding Error
 * https://adventofcode.com/2020/day/9
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic -Wno-char-subscripts 09.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 09.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  9.76 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 13.8  µs
 *     Raspberry Pi 5 (2.4 GHz)      : 33.1  µs
 */

#include <stdio.h>
#include <stdint.h>  // uint64_t, UINT64_C
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2020-09-input.txt"
#define FSIZE 8824  // input file size in bytes
#define LEN   1000  // lines in input file
#define PRE   25    // window size part 1

static char input[FSIZE];
static int data[LEN];

// Parse masked digits of unsigned int, skip newline
static unsigned parseint(const char **s)
{
    unsigned x = *(*s)++;  // already masked
    while (**s != '\n')
        x = x * 10 + *(*s)++;  // signed int overflow segfaults on RPi
    (*s)++;  // skip newline
    return x;
}

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

    // Parse numbers until i32 overflow
    uint64_t *const m = (uint64_t *)input;
    for (int i = 0; i < (FSIZE >> 3); ++i)  // FSIZE must be divisible by 8
        m[i] &= UINT64_C(0x0f0f0f0f0f0f0f0f);  // pre-mask all bytes, newline unaffected
    const char *c = input;
    for (int i = 0; (data[i] = parseint(&c)) > 0; ++i);

    // Part 1
    int k = PRE;
    while (valid(k))
        k++;
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
