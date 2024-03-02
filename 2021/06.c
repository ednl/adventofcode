#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include "../startstoptimer.h"

#define CYCLE   7  // spawn every 7 days
#define DELAY   2  // add 2 days to the first spawn cycle
#define LIFE (CYCLE + DELAY)  // the whole circle of life
#define DAYS1  80  // part 1
#define DAYS2 256  // part 2

// Histogram of fish population count per age mod 9
static uint64_t age[LIFE] = {0};

// Live for days, return population
static uint64_t live(const int days)
{
    static int day = 0, i = 0, j = CYCLE;
    for (; day < days; ++day, ++i == LIFE && (i = 0), ++j == LIFE && (j = 0))
        age[j] += age[i];  // start new spawn cycle
        // Leaving current bin age[i] untouched = each fishie produces one kiddo

    uint64_t pop = 0;
    for (int k = 0; k < LIFE; ++k)
        pop += age[k];
    return pop;
}

int main(void)
{
    starttimer();
    // Build population histogram by age bin
    FILE *f = fopen("../aocinput/2021-06-input.txt", "r");
    int c = ',';
    while (c == ',') {
        age[fgetc(f) - '0']++;
        c = fgetc(f);
    }
    fclose(f);

    printf("Part 1: %"PRIu64"\n", live(DAYS1));  // 374927
    printf("Part 2: %"PRIu64"\n", live(DAYS2));  // 1687617803407
    printf("Time: %.0f us\n", stoptimer_us());
    return 0;
}
