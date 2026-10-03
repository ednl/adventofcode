/**
 * Advent of Code 2024
 * Day 4: Ceres Search
 * https://adventofcode.com/2024/day/4
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 04.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 04.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 18.0 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : ? µs
 */

#include <stdio.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2024-04-input.txt"
#define N 140                 // input grid size
#define L 4                   // "XMAS" length

// Derived values
#define ROWS (N + L * 2)      // 4 border rows top+bottom
#define COLS (N + 1)          // +newline
#define BEG (L * COLS)        // first index of input: row 4, col 0
#define END ((L + N) * COLS)  // last+1 index of input
#define F (COLS - 1)          // index offset direction NE / SW = "Fwd slash"
#define V (COLS)              // index offset direction N  / S  = "Vertical"
#define B (COLS + 1)          // index offset direction NW / SE = "Backslash"
#define X (('M' + 'S') * 2)   // XMAS corners for part 2

// 1-D grid
static char g[ROWS * COLS];

int main(void)
{
    FILE *f = fopen(FNAME, "rb");  // fread() requires binary mode
    if (!f) { fputs("File not found: "FNAME, stderr); return 1; }
    fread(&g[BEG], N * (N + 1), 1, f);  // read whole file at once
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    int sum = 0;
    for (int i = BEG; i < END; ++i)
        if (g[i] == 'X') {
            sum += g[i+1] == 'M' && g[i+  2] == 'A' && g[i+  3] == 'S';
            sum += g[i-1] == 'M' && g[i-  2] == 'A' && g[i-  3] == 'S';
            sum += g[i+F] == 'M' && g[i+F*2] == 'A' && g[i+F*3] == 'S';
            sum += g[i-F] == 'M' && g[i-F*2] == 'A' && g[i-F*3] == 'S';
            sum += g[i+V] == 'M' && g[i+V*2] == 'A' && g[i+V*3] == 'S';
            sum += g[i-V] == 'M' && g[i-V*2] == 'A' && g[i-V*3] == 'S';
            sum += g[i+B] == 'M' && g[i+B*2] == 'A' && g[i+B*3] == 'S';
            sum += g[i-B] == 'M' && g[i-B*2] == 'A' && g[i-B*3] == 'S';
        }
    printf("%u ", sum);  // 2414

    sum = 0;
    for (int i = BEG + B; i < END - B; ++i)
        if (g[i] == 'A')
            sum += g[i-B] + g[i-F] + g[i+F] + g[i+B] == X
                && g[i-B] != g[i+B];
    printf("%u\n", sum);  // 1871

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
