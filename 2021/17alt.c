/**
 * Advent of Code 2021
 * Day 17: Trick Shot
 * https://adventofcode.com/2021/day/17
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 17.c -lm
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 17.c -lm
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :    ? µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 33.5 µs
 *     Raspberry Pi 5 (2.4 GHz)      :    ? µs
 */

#include <stdio.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2021-17-input.txt"
#define FSIZE 64  // needed for my input: 36

static char input[FSIZE];

static int tri(const int n)
{
    return n * (n + 1) >> 1;
}

static int parseint(const char **s)
{
    int x = *(*s)++ & 15;
    while (**s >= '0')
        x = x * 10 + (*(*s)++ & 15);
    return x;
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");  // fread() requires binary mode
    if (!f) return 1;
    fread(input, 1, FSIZE, f);  // read single bytes until EOF or size=FSIZE
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    const char *c = input + 15;  // skip "target area: x="
    const int x0 = parseint(&c);
    c += 2;  // skip ".."
    const int x1 = parseint(&c);
    c += 5; // skip ", y=-"
    const int y1 = parseint(&c);  // largest absolute y-value of target area
    c += 3;  // skip "..-"
    const int y0 = parseint(&c);  // smallest absolute y-value of target area

    // Part 1
    // Probe going first up & then back down is symmetrical, so when going down
    // it passes y=0 afer which the largest possible step is to the lower bound
    // (largest absolute value) of the target area: y1. So the height reached
    // is the sum of 1 to y1 - 1.
    printf("%d\n", (y1 - 1) * y1 >> 1);  // 4753

    // Part 2: 1546
    // 0, 1, 3, 6, 10, 15, 21, 28, 36, 45, 55, 66, 78, 91, 105, 120, 136, 153, 171, 190, 210
    // target: x = 137..171
    // 1 step: 171-137+1 = 35
    // 2 steps: T69-T67=69+68=137, T86-T84=86+85=171 => 86-69+1 = 18
    // 3 steps: T47-T44=47+46+45=138, T58-T55=58+57+56=171 => 58-47+1 = 12
    // [...]
    // 11 steps: T18-T7=143, T20-T9=165 => 20-18+1=3
    // 12 steps: T17-T5=138, T19-T7=162 => 19-17+1=3
    // 13 steps: T17-T4=143, T19-T6=169 => 19-17+1=3
    // 14 steps: T17-T3=147, T18-T4=161 => 18-17+1=2
    // 15 steps: T17-T2=150, T18-T3=165 => 18-17+1=2
    // 16 steps: T17-T1=152, T18-T2=168 => 18-17+1=2
    // 17 steps: T17-T0=153, T18-T1=170 => 18-17+1=2
    // 18 steps: T17-T0+T0=153, T18-T0=171 => 1+1=2
    // 19 steps: T17-T0+T0+T0=153, T18-T0+T0=171 => 1+1=2

    // 0, 1, 3, 6, 10, 15, 21, 28, 36, 45, 55, 66, 78, 91, 105, 120, 136, 153, 171, 190, 210
    // target: y = 73..98
    // 1 step: 98-73+1 = 26, also: T(73-1)*2+1,...,T(98-1)*2+1 = 2629,...,4754
    // 2 step: T37-T35=73, T49-T47=97 => 49-37+1=13, also: T(37-2)+2,...,T(49-2)+2 = 632,...,1130
    // [...]
    // 11 steps: T12-T1=77, T13-T2=88 => 31-12+1=2, also: T(12-11)+11,T(13-11)+11 = 12,14
    // 12 steps: T12-T0=78, T13-T1=90 => 31-12+1=2, also: T(13-12)+12 = 13
    // 13 steps: T13-T0=91 => 1

    // Generate triangular numbers
    int Tr[101];
    Tr[0] = 0;
    for (int i = 1; i < 101; ++i)
        Tr[i] = Tr[i - 1] + i;
    printf("%d\n", Tr[100]);

    int tx[20] = {0, x1 - x0 + 1};
    int ty[20] = {0, y1 - y0 + 1};
    int i = 19;
    for (; Tr[i] > x1; --i);
    for (; Tr[i] >= x0; --i)
        for (int j = i; j < 20; ++j)
            tx[j]++;
    for (int t = 2; t < 20; ++t) {
        int i = t + 1;
        for (; Tr[i] - Tr[i - t] < x0; ++i);  // find first hit
        for (; Tr[i] - Tr[i - t] <= x1; ++i)
            tx[t]++;
    }
    for (int i = 0; i < 20; ++i)
        printf("%2d: %d %d %d\n", i, tx[i], ty[i], tx[i] * ty[i]);

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
