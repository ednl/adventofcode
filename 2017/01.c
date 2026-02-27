/**
 * Advent of Code 2017
 * Day 01: Inverse Captcha
 * https://adventofcode.com/2017/day/01
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 01.c
 * Enable timer:
 *     cc -std=gnu17 -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 01.c
 * Get minimum runtime from timer output:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 1.37 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :    ? µs
 *     Raspberry Pi 5 (2.4 GHz)      :    ? µs
 */

#include <stdio.h>
#include <unistd.h>  // isatty, fileno
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2017-01-input.txt"
#define MAXLEN 4096  // needed for my input: 2183

static char input[MAXLEN];

int main(int argc, char *argv[])
{
    if (argc > 1) {
        int n = 0;
        for (; n < MAXLEN && argv[1][n] >= '0' && argv[1][n] <= '9'; ++n)
            input[n] = argv[1][n];
        if (!n) { fprintf(stderr, "Invalid input: \"%s\"\n", argv[1]); return 1; }
    } else if (!isatty(fileno(stdin))) {
        fread(input, sizeof input, 1, stdin);
    } else {
        FILE *f = fopen(FNAME, "r");
        if (!f) { fprintf(stderr, "File not found: "FNAME"\n"); return 1; }
        fread(input, sizeof input, 1, f);
        fclose(f);
    }

#ifdef TIMER
    starttimer();
#endif

    // Determine length of input, calculate digit value
    int n = 0;
    for (; input[n] >= '0' && input[n] <= '9'; ++n)
        input[n] -= '0';
    input[n] = input[0];  // to compare last with first digit

    // Part 1: consecutive identical digits
    int captcha = 0;
    for (int i = 0; i < n; ++i)
        if (input[i] == input[i + 1])
            captcha += input[i];
    printf("%d\n", captcha);  // 1390

    // Part 2: halfway around identical digits
    captcha = 0;
    const int half = n >> 1;
    for (int i = 0; i < half; ++i)
        if (input[i] == input[i + half])
            captcha += input[i] << 1;
    printf("%d\n", captcha);  // 1232

#ifdef TIMER
    printf("Time: %.0f ns\n", stoptimer_ns());
#endif
}
