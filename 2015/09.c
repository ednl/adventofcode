/**
 * Advent of Code 2015
 * Day 9: All in a Single Night
 * https://adventofcode.com/2015/day/9
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -O3 -march=native -Wall -Wextra 09.c ../permutations.c
 *    gcc   -std=gnu17 -O3 -march=native -Wall -Wextra 09.c ../permutations.c
 * Get minimum runtime:
 *     m=999999;for((i=0;i<2000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * Minimum runtime:
 *     Mac Mini 2020 (M1 3.2 GHz)          :   ? µs
 *     iMac 2013 (i5 Haswell 4570 3.2 GHz) : 336 µs
 *     Raspberry Pi 5 (2.4 GHz)            :   ? µs
 *     Raspberry Pi 4 (1.8 GHz)            :   ? µs
 */

#include <stdio.h>
#include <stdlib.h>
#include "../permutations.h"
#include "../startstoptimer.h"

#define N 8
static int dist[N][N];

int main(void)
{
    starttimer();
    FILE *f = fopen("../aocinput/2015-09-input.txt", "r");
    if (!f)
        return 1;
    for (int i = 0; i < N - 1; ++i)
        for (int j = i + 1; j < N; ++j) {
            char line[64], *c;
            if ((c = fgets(line, sizeof line, f))) {
                while (*c != '=')
                    ++c;
                dist[i][j] = dist[j][i] = atoi(c + 2);
            }
        }
    fclose(f);

    int *p, min = 1000, max = 0;
    while ((p = permutations(N))) {
        int sum = 0;
        for (int i = 0; i < N - 1; ++i)
            sum += dist[p[i]][p[i + 1]];
        min = sum < min ? sum : min;
        max = sum > max ? sum : max;
    }
    printf("%d %d\n", min, max);  // 251 898
    printf("Time: %.0f us\n", stoptimer_us());
    return 0;
}
