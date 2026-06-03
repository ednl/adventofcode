/**
 * Advent of Code 2020
 * Day 2: Password Philosophy
 * https://adventofcode.com/2020/day/2
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 02.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 02.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  4.31 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :     ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : 13.1  µs
 */

#include <stdio.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2020-02-input.txt"
#define L 30    // max line length 3+3+3+20+1
#define N 1000  // lines in input file
#define FSIZE (L * N)  // needed for my input: 20507

static char input[FSIZE];

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    fread(input, 1, FSIZE, f);  // read char-by-char to max size
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    int valid1 = 0, valid2 = 0;
    for (const char *c = input; *c; ++c) {
        int min, max;
        char letter;
        switch (*(c + 6)) {
        case ' ':  // range is 2x 1-digit
            min = *c & 15;
            max = *(c + 2) & 15;
            letter = *(c + 4);
            c += 7;
            break;
        case ':':  // range is 1-digit + 2-digit
            min = *c & 15;
            max = (*(c + 2) & 15) * 10 + (*(c + 3) & 15);
            letter = *(c + 5);
            c += 8;
            break;
        default:  // range is 2x 2-digit
            min = (*c & 15) * 10 + (*(c + 1) & 15);
            max = (*(c + 3) & 15) * 10 + (*(c + 4) & 15);
            letter = *(c + 6);
            c += 9;
            break;
        }
        const char *const pwd = c - 1;  // start one back to use 1-based min/max
        int count = 0;
        while (*c != '\n')
            count += letter == *c++;
        valid1 += min <= count && count <= max;
        valid2 += (pwd[min] == letter) ^ (pwd[max] == letter);
    }
    printf("%d %d\n", valid1, valid2);  // 500 313

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
