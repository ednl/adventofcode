/**
 * Advent of Code 2021
 * Day 9: Smoke Basin
 * https://adventofcode.com/2021/day/9
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic ../topn.c 09.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c ../topn.c 09.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 20.0 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 33.2 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 97.0 µs
 */

#include <stdio.h>
#include <string.h>   // memcpy, memset
#include "../topn.h"  // don't sort whole array, only top 3
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2021-09-input.txt"
#define N 100  // grid dimensions
#define M 256  // max number of basins

static char input[N][N + 1];       // input grid NxN + newlines
static char height[N + 2][N + 1];  // border but reuse newlines as no-go
static int basin[M];               // size of each basin found

// Floodfill the basin, keep track of minimum, return basin size
static int fillbasin(const int x, const int y, int *map)
{
    *map |= 1 << (height[x][y] & 15);  // elevation map
    height[x][y] = '9';  // mark as done
    int area = 1;
    if ((height[x][y + 1] & 15) < 9) area += fillbasin(x, y + 1, map);
    if ((height[x + 1][y] & 15) < 9) area += fillbasin(x + 1, y, map);
    if ((height[x][y - 1] & 15) < 9) area += fillbasin(x, y - 1, map);
    if ((height[x - 1][y] & 15) < 9) area += fillbasin(x - 1, y, map);
    return area;
}

// Sort int array in descending order
static int descending(const void *p, const void *q)
{
    const int a = *(const int *)p;
    const int b = *(const int *)q;
    return (a < b) - (b < a);
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    fread(input, sizeof input, 1, f);
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    // None shall pass
    memset(&height[0][0], '9', N + 2);  // first row + first col of 2nd row
    memcpy(&height[1][1], input, sizeof input);  // input data
    memset(&height[N + 1][0], '9', N + 1);  // last row

    int risk = 0, count = 0;
    for (int i = 1; i <= N; ++i)
        for (int j = 1; j <= N; ++j)
            if (height[i][j] < '9') {
                int map = 0;
                basin[count++] = fillbasin(i, j, &map);  // part 2
                risk += 1 + __builtin_ctz(map);  // part 1
            }
    topn(basin, 3, count, sizeof *basin, descending);  // part 2
    printf("%d %d\n", risk, basin[0] * basin[1] * basin[2]);  // 506 931200

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
