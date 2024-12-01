/**
 * Advent of Code 2024
 * Day 2: ?
 * https://adventofcode.com/2024/day/2
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -O3 -march=native -Wall -Wextra 02.c
 *    gcc   -std=gnu17 -O3 -march=native -Wall -Wextra 02.c
 * Get minimum runtime:
 *     m=999999;for((i=0;i<5000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime:
 *     Mac Mini 2020 (M1 3.2 GHz)                       :  ? µs
 *     iMac 2013 (Core i5 Haswell 4570 3.2 GHz)         :  ? µs
 *     Raspberry Pi 5 (2.4 GHz)                         :  ? µs
 *     Macbook Air 2013 (Core i5 Haswell 4250U 1.3 GHz) :  ? µs
 *     Raspberry Pi 4 (1.8 GHz)                         :  ? µs
 */

#include <stdio.h>

#define FNAME "../aocinput/2024-02-input.txt"
#define N 100

int main(void)
{
    FILE *f = fopen(FNAME, "r");
    if (!f) { fputs("File not found.\n", stderr); return 1; }

    char buf[64];
    for (int i = 0; i < N && fgets(buf, sizeof buf, f); ++i) {
        //
    }
    fclose(f);

    int n = 0;
    printf("Part 1: %d\n", n);
    return 0;
}
