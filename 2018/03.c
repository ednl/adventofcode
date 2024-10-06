/**
 * Advent of Code 2018
 * Day 3: No Matter How You Slice It
 * https://adventofcode.com/2018/day/3
 * By: E. Dronkert https://github.com/ednl
 *
 * Benchmark on a Mac Mini M1, compiler Apple clang 14.0.3 with -O3 -march=native:
 *
 *     [TBD]
 *
 * Benchmark on an iMac (Late 2013, 3.2 GHz quad-core Core i5 "Haswell"),
 * compiler Apple clang 12.0.0 with -O3 -march=native:
 *
 *     $ hyperfine -N -w 200 -r 1000 ./a.out
 *     Benchmark 1: ./a.out
 *       Time (mean ± σ):       4.1 ms ±   0.5 ms    [User: 2.1 ms, System: 1.0 ms]
 *       Range (min … max):     3.4 ms …   6.2 ms    1000 runs
 *
 * Benchmark on a Raspberry Pi 4, compiler Debian gcc 10.2.1-6 with -O3 -march=native
 * and the CPU in performance mode:
 *
 *     echo performance | sudo tee /sys/devices/system/cpu/cpufreq/policy0/scaling_governor
 *     (to reset, replace performance with ondemand)
 *     /boot/config.txt: arm_boost=1, no overclock
 *
 *     $ hyperfine -N -w 100 -r 500 ./a.out
 *     Benchmark 1: ./a.out
 *       Time (mean ± σ):      11.3 ms ±   0.1 ms    [User: 5.8 ms, System: 5.2 ms]
 *       Range (min … max):    11.1 ms …  12.3 ms    500 runs
*/
#include <stdio.h>   // fopen, fclose, fscanf, printf
#include <stdint.h>  // uint16_t, uint32_t
#include <stdbool.h>

#define N 1293  // number of claims (lines) in input file
#define M 1000  // fabric dimension MxM

typedef struct {
    uint16_t x, y;
} Pos;

typedef struct {
    uint16_t id;
    Pos a, b;  // a = top left corner, b = 1 outside bottom right corner
} Claim;

static Claim claim[N];
static uint16_t fabric[M][M];

static size_t parse(void)
{
    FILE *f = fopen("../aocinput/2018-03-input.txt", "r");
    if (!f)
        return 0;

    size_t n = 0;
    uint16_t id, x, y, w, h;
    while (n < N && fscanf(f, "#%hu @ %hu,%hu: %hux%hu ", &id, &x, &y, &w, &h) == 5)
        claim[n++] = (Claim){id, {x, y}, {x + w, y + h}};
    fclose(f);
    return n;
}

int main(void)
{
    // Input
    size_t n = parse();

    // Lay claim
    const Claim *c = claim;
    for (size_t i = 0; i < n; ++i, ++c)
        for (size_t x = c->a.x; x < c->b.x; ++x)
            for (size_t y = c->a.y; y < c->b.y; ++y)
                fabric[x][y]++;
    // Count all overlap
    uint32_t overlap = 0;
    for (size_t x = 0; x < M; ++x)
        for (size_t y = 0; y < M; ++y)
            if (fabric[x][y] > 1)
                overlap++;
    printf("Part 1: %u\n", overlap);  // 110827

    // Find claim without overlap
    c = claim;
    for (size_t i = 0; i < n; ++i, ++c) {
        bool intact = true;  // extra variable to break twice
        for (size_t x = c->a.x; intact && x < c->b.x; ++x)
            for (size_t y = c->a.y; y < c->b.y && (intact = fabric[x][y] == 1); ++y)
                ;
        if (intact) {
            printf("Part 2: %u\n", c->id);  // 116
            return 0;
        }
    }
    return 1;
}
