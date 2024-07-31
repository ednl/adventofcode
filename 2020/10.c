/**
 * Advent of Code 2020
 * Day 10: Adapter Array
 * https://adventofcode.com/2020/day/10
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -Ofast -march=native -Wall -Wextra 10.c ../startstoptimer.c
 *    gcc   -std=gnu17 -Ofast -march=native -Wall -Wextra 10.c ../startstoptimer.c
 * Get minimum runtime:
 *     m=999999;for((i=0;i<5000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * Minimum runtime:
 *     Mac Mini 2020 (M1 3.2 GHz)          : 15 µs
 *     iMac 2013 (i5 Haswell 4570 3.2 GHz) :  ? µs
 *     Air 2013 (i5 Haswell 4250U 1.3 GHz) :  ? µs
 *     ThinkPad Ubuntu (i5 8250U 1.8 GHz)  :  ? µs
 *     Raspberry Pi 5 (2.4 GHz)            :  ? µs
 *     Raspberry Pi 4 (1.8 GHz)            :  ? µs
 */

#include <stdio.h>     // fopen, fclose, fgets, printf
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRId64
#include <stdbool.h>
#include "../startstoptimer.h"

#define FNAME "../aocinput/2020-10-input.txt"
#define JOLTRANGE 256  // max value in input file: 256-1-3=252
#define MINGAP 1
#define MAXGAP 3

static bool adapter[JOLTRANGE];
static int64_t arrangements[JOLTRANGE];

int main(void)
{
    starttimer();

    FILE *f = fopen(FNAME, "r");
    if (!f)
        return 1;
    int maxjolts = 0;
    char buf[8];
    while (fgets(buf, sizeof buf, f)) {
        int jolts = 0;
        char *s = buf;
        while (*s != '\n')
            jolts = jolts * 10 + (*s++ & 15);  // slightly faster than fscanf
        adapter[jolts] = true;
        if (jolts > maxjolts)
            maxjolts = jolts;
    }
    fclose(f);
    adapter[0] = true;  // charging outlet at 0 jolts
    adapter[maxjolts += MAXGAP] = true;  // device being charged at the other end

    // Part 1
    int gapcount1 = 0, gapcount3 = 0;
    for (int i = 0; i < maxjolts;) {
        int j = i + 1;
        while (!adapter[j])
            ++j;
        switch (j - i) {
            case MINGAP: ++gapcount1; break;
            case MAXGAP: ++gapcount3; break;
        }
        i = j;
    }
    printf("%d\n", gapcount1 * gapcount3);  // 2574

    // Part 2
    arrangements[0] = 1;  // charging outlet is fixed
    for (int j = 1; j < MAXGAP; ++j)
        for (int i = 0; i < j; ++i)
            arrangements[j] += arrangements[i] * adapter[i];  // only add if adapter[i] exists (=true)
    for (int j = MAXGAP; j <= maxjolts; ++j)
        for (int i = j - MAXGAP; i < j; ++i)
            arrangements[j] += arrangements[i] * adapter[i];
    printf("%"PRId64"\n", arrangements[maxjolts]);  // 2644613988352

    printf("Time: %.0f µs\n", stoptimer_us());
    return 0;
}
