/**
 * Advent of Code 2020
 * Day 5: Binary Boarding
 * https://adventofcode.com/2020/day/5
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 05alt.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 05alt.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 0.77 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :    ? µs
 *     Raspberry Pi 5 (2.4 GHz)      :    ? µs
 */

#include <stdio.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2020-05-input.txt"
#define PASS 761  // boarding passes (lines in input file)
#define LEN  10   // line length = 10x F/B/L/R (excl. newline)
#define FSIZE (PASS * (LEN + 1))  // +newline

static char input[FSIZE];

// xorsum(1,n) = 1 ^ 2 ^ ... ^ (n-1) ^ n
static unsigned xorsum1(const unsigned x)
{
    switch (x & 3) {
        case 0: return x;
        case 1: return 1;
        case 2: return x + 1;
        case 3: return 0;
    }
    return 0;  // keep compiler happy
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    fread(input, FSIZE, 1, f);  // read one block of fixed size
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    // Parse input file, track max, sum by xor
    const char *c = input;
    unsigned max = 0, sum = 0;
    for (int i = 0; i < PASS; ++i) {
        unsigned id = 0;
        for (int j = 0; j < LEN; ++j)
            id = id << 1 | !(*c++ & 4);  // F/L = 0, B/R = 1
        if (id > max)
            max = id;
        sum ^= id;
        c++;  // skip newline
    }

    // Missing value = sum ^ xorsum(min,max)
    // where: xorsum(min,max) = xorsum(1,max) ^ xorsum(1,min-1)
    // and: range = PASS+1 (because +1 gap) = max-min+1
    //   => min = max-PASS
    //   => min-1 = max-PASS-1 = max-(PASS+1)
    printf("%u %u\n", max, sum ^ xorsum1(max) ^ xorsum1(max - (PASS + 1)));  // 861 633

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
