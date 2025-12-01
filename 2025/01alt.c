// Translated from Python version by /u/SupportPowerful6174
// https://www.reddit.com/r/adventofcode/comments/1pbmajc/2025_day_01_part_2_python_efficient_algorithm_on/

#include <stdio.h>
#include "../startstoptimer.h"

#define FNAME "../aocinput/2025-01-input.txt"
#define FSIZE 19650

static char input[FSIZE];
static const char *const end = input + FSIZE;

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) { fprintf(stderr, "File not found: %s\n", FNAME); return 1; }
    fread(input, sizeof input, 1, f);
    fclose(f);
    starttimer();
    int dial = 50, zero1 = 0, zero2 = 0;
    for (const char *c = input; c != end; ++c) {
        const int prev = dial;
        const int dir = (*c++ & 3) - 1;  // 'L'=-1, 'R'=1
        int turn = *c++ & 15;
        while (*c != '\n')
            turn = turn * 10 + (*c++ & 15);
        zero2 += turn / 100;
        dial += turn % 100 * dir;
        if (dial < 0) {
            dial += 100;
            zero2 += dial != prev && prev != 0;
        } else if (dial >= 100) {
            dial -= 100;
            zero2 += dial != prev && prev != 0;
        } else if (dial == 0) {
            zero2 += prev != 0;
        }
        zero1 += !dial;
    }
    printf("%d %d\n", zero1, zero2);  // 1180 6892
    printf("Time: %.0f us\n", stoptimer_us());
}
