#include <stdio.h>
#include <stdbool.h>
#include <limits.h>  // INT_MIN
#include "startstoptimer.h"

#define STEPS 100
#define FLASH  10
#define DIM    10
static int oct[DIM + 2][DIM + 2] = {0};

static int cascade(void)
{
    // Increase all energy levels
    for (int i = 1; i <= DIM; ++i)
        for (int j = 1; j <= DIM; ++j)
            oct[i][j]++;

    // Loop while more might be about to flash
    bool flash;
    do {
        flash = false;
        for (int i = 1; i <= DIM; ++i)
            for (int j = 1; j <= DIM; ++j)
                if (oct[i][j] >= FLASH) {
                    flash = true;
                    // Increase energy levels of neighbours
                    for (int x = i - 1; x <= i + 1; ++x)
                        for (int y = j - 1; y <= j + 1; ++y)
                            oct[x][y]++;
                    // Mark as done
                    oct[i][j] = INT_MIN;
                }
    } while (flash);

    // Count flashes
    int n = 0;
    for (int i = 1; i <= DIM; ++i)
        for (int j = 1; j <= DIM; ++j)
            if (oct[i][j] < 0) {
                oct[i][j] = 0;
                ++n;
            }
    return n;
}

int main(void)
{
    starttimer();

    // Import
    FILE *f = fopen("../aocinput/2021-11-input.txt", "r");
    for (int i = 1; i <= DIM; ++i) {
        for (int j = 1; j <= DIM; ++j)
            oct[i][j] = fgetc(f) - '0';
        fgetc(f);  // discard \n
    }
    fclose(f);

    // Part 1
    int step = 0, total = 0;
    while (step < STEPS) {
        ++step;
        total += cascade();
    }
    printf("Part 1: %d\n", total);  // 1739

    // Part 2
    while (++step && cascade() < DIM * DIM);
    printf("Part 2: %d\n", step);  // 324

    printf("\nTime: %.1f µs\n", stoptimer_us());
    return 0;
}
