/**
 * Advent of Code 2020
 * Day 3: Toboggan Trajectory
 * https://adventofcode.com/2020/day/3
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 03.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 03.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 1.03 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :    ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : 2.61 µs
 */

#include <stdio.h>
#ifdef TIMER
    #include <string.h>  // memset
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2020-03-input.txt"
#define ROW 323  // lines in input file
#define COL  31  // characters per line (excl. newline)
#define SLOPE 5  // different slopes for part 2
#define TREE '#'

typedef struct slope {
    int right, down;
} Slope;

// [1]={3,1} is slope for part 1
static const Slope slope[SLOPE] = {{1,1}, {3,1}, {5,1}, {7,1}, {1,2}};

static char map[ROW][COL + 1];  // +newline
static unsigned tree[SLOPE];

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    fread(map, sizeof map, 1, f);  // read one block of fixed size
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
    memset(tree, 0, sizeof tree);
#endif

    unsigned prod = 1;  // overflows signed int
    for (int i = 0; i < SLOPE; ++i) {
        for (int row = 0, col = 0; row < ROW; col += slope[i].right, row += slope[i].down) {
            if (col >= COL)  // more than twice as fast as mod (%)
                col -= COL;
            tree[i] += map[row][col] == TREE;
        }
        prod *= tree[i];
    }
    printf("%u %u\n", tree[1], prod);  // 247 2983070376

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
