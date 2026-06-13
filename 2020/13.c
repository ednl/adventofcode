/**
 * Advent of Code 2020
 * Day 13: Shuttle Search
 * https://adventofcode.com/2020/day/13
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 13.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 13.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 184 ns
 *     Mac Mini 2020 (M1 3.2 GHz)    : ? ns
 *     Raspberry Pi 5 (2.4 GHz)      : ? ns
 */

#include <stdio.h>
#include <stdint.h>  // uint64_t
#include <inttypes.h>  // PRId64
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2020-13-input.txt"
#define FSIZE 200  // needed for my input: 167

static char input[FSIZE];

static int parseint(const char **s)
{
    int x = *(*s)++ & 15;
    while (**s >= '0' && **s <= '9')
        x = x * 10 + (*(*s)++ & 15);
    (*s)++;  // skip delim or newline
    return x;
}

static int depart(const int time, const int bus)
{
    const int x = (bus - time) % bus;
    return x < 0 ? x + bus : x;
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    const char *const end = input + fread(input, 1, FSIZE, f);
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    const char *c = input;
    const int mytime = parseint(&c);  // "earliest timestamp you could depart on a bus"
    int mybus = 0, minwait = 1000;
    int64_t t = 0, dt = 1;
    for (int time = 0; c != end; ++time)
        if (*c != 'x') {
            const int bus = parseint(&c);
            // Part 1
            const int wait1 = depart(mytime, bus);
            if (wait1 < minwait) {
                minwait = wait1;
                mybus = bus;
            }
            // Part 2: https://en.wikipedia.org/wiki/Chinese_remainder_theorem#Search_by_sieving
            const int wait2 = depart(time, bus);
            while (t % bus != wait2)
                t += dt;
            dt *= bus;
        } else
            c += 2;
    printf("%d %"PRId64"\n", mybus * minwait, t);  // 3385 600689120448303

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
