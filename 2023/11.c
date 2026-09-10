/**
 * Advent of Code 2023
 * Day 11: Cosmic Expansion
 * https://adventofcode.com/2023/day/11
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 11.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 11.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  9.5 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :  ?   µs
 *     Raspberry Pi 5 (2.4 GHz)      :  ?   µs
 */

#include <stdio.h>
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRId64
#ifdef TIMER
    #include <string.h>  // memset
    #include "../startstoptimer.h"  // timer
#endif

#define FNAME "../aocinput/2023-11-input.txt"
#define N 140      // rows and columns in image map
#define M 1000000  // empty space expansion factor part 2
#define G 512      // galaxies, needed for my input: 430

static char image[N][N + 1];
static int xcount[N], ycount[N], xshift[N], yshift[N];
static int xpos[G], ypos[G];

// Nifty algorithm by /u/NikitaSkybytskyi
// https://www.reddit.com/r/adventofcode/comments/18fqxuq/an_on_algorithm_for_day_11/kcvwlev/
static int64_t dist(const int *const restrict pos, const int *const restrict shift, const int n, const int64_t f)
{
    int64_t sum = 0, partial = 0;
    for (int i = 0; i < n; ++i) {
        const int64_t exp = (f - 1) * shift[pos[i]] + pos[i];
        sum += i * exp - partial;
        partial += exp;
    }
    return sum;
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) { fputs("File not found: "FNAME, stderr); return 1; }
    fread(image, sizeof image, 1, f);  // read as one block
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
    memset(xcount, 0, sizeof xcount);
    memset(ycount, 0, sizeof ycount);
#endif

    int galaxies = 0;
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            if (image[i][j] == '#') {
                xcount[j]++;
                ycount[i]++;
                galaxies++;
            }

    // x-pos sorted by x, y-pos sorted by y to avoid negative distance
    for (int i = 0, k = 0; i < N; ++i)
        for (int j = 0; j < xcount[i]; ++j)
            xpos[k++] = i;
    for (int i = 0, k = 0; i < N; ++i)
        for (int j = 0; j < ycount[i]; ++j)
            ypos[k++] = i;

    xshift[0] = !xcount[0];
    yshift[0] = !ycount[0];
    for (int i = 1; i < N; ++i) {
        xshift[i] = xshift[i - 1] + !xcount[i];
        yshift[i] = yshift[i - 1] + !ycount[i];
    }

    printf("%"PRId64" %"PRId64"\n",  // 9608724 904633799472
        dist(xpos, xshift, galaxies, 2) + dist(ypos, yshift, galaxies, 2),
        dist(xpos, xshift, galaxies, M) + dist(ypos, yshift, galaxies, M));

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
