/**
 * Advent of Code 2019
 * Day 4: Secure Container
 * https://adventofcode.com/2019/day/4
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 04.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 04.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  4.87 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 11.4  µs
 *     Raspberry Pi 5 (2.4 GHz)      : 14.0  µs
 */

#include <stdio.h>
#include <stdint.h>    // uint64_t, uint8_t
#include <inttypes.h>  // PRIu64
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2019-04-input.txt"
#define FSIZE 16  // needed for my input: 14
#define LEN 6  // digitcount of limits in input (max. 8)

// Password numbers
typedef union pwd {
    uint64_t num;
    struct { uint8_t arr[8]; };  // access single bytes in ::num
} Pwd;

static char input[FSIZE];

// Parse number into 1 digit per byte, advance char pointer, skip one more
static uint64_t parseint(const char **str)
{
    uint64_t x = 0;
    while (**str >= '0')
        x = x << 8 | (*(*str)++ & 15);
    (*str)++;  // skip dash/newline
    return x;
}

// Next valid password
static void incr(Pwd *p)
{
    p->arr[0]++;  // increase unit
    for (int i = 0; i < LEN && p->arr[i] == 10; ) {  // digit overflow
        p->arr[i++] = 0;  // current digit: zero
        p->arr[i]++;      // next digit: increase
    }
    // Digits must be non-decreasing from most to least significant
    for (int i = LEN - 1; i > 0; --i)
        if (p->arr[i - 1] < p->arr[i])
            p->arr[i - 1] = p->arr[i];
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    fread(input, 1, FSIZE, f);
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    const char *c = input;
    const Pwd start = { parseint(&c) };
    const Pwd stop  = { parseint(&c) };

    int part1 = 0, part2 = 0;
    for (Pwd pwd = start; pwd.num <= stop.num; incr(&pwd)) {
        bool any = false, sep = false;  // any pair, separate pair
        for (int i = 1; i < LEN && !sep; ++i)
            if (pwd.arr[i] == pwd.arr[i - 1]) {
                any = true;
                sep = (i == 1 || pwd.arr[i] != pwd.arr[i - 2]) && (i == LEN - 1 || pwd.arr[i] != pwd.arr[i + 1]);
            }
        part1 += any;
        part2 += sep;
    }
    printf("%d %d\n", part1, part2);  // 1640 1126

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());
#endif
}
