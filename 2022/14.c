/**
 * Advent of Code 2022
 * Day 14: Regolith Reservoir
 * https://adventofcode.com/2022/day/14
 * By: E. Dronkert https://github.com/ednl
 *
 * Benchmark with the internal timer on a Mac Mini M1 using this Bash oneliner:
 *   m=50000;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * gives a shortest runtime for my input file (not the example) of 5.08 ms.
 * On a Raspberry Pi 4 with the CPU in performance mode: 26.7 ms.
 *   echo performance | sudo tee /sys/devices/system/cpu/cpufreq/policy0/scaling_governor
 *   /boot/config.txt: arm_boost=1, no overclock
 */

#include <stdio.h>
#include <stdlib.h>   // calloc, free
#include <stdbool.h>
#include <stdint.h>   // uint8_t
#include "../startstoptimer.h"

#define EXAMPLE 0
#if EXAMPLE
#define NAME "../aocinput/2022-14-example.txt"
#define GROW (8)  // example: min.x=494->489, max.x=503->511, so step=8 means 1x grow left + 1x grow right
#else
#define NAME "../aocinput/2022-14-input.txt"
#define GROW (162)  // input: min.x=492->330, max.x=602->670, so step=162 means 1x grow left + 1x grow right
#endif

#define INLETX (500)
#define INLETY (0)
#define SPACE UINT8_C(0)
#define ROCK  UINT8_C(1)
#define SAND  UINT8_C(2)

typedef struct {
    int x, y;
} Vec;

typedef struct {
    uint8_t *grid;
    Vec min, max, dim;
    int size;
} Cave;

static const Vec inlet = {INLETX,INLETY};
static const Vec down = {0,1}, left = {-1,0}, right = {2,0}, back = {-1,-1};
static Cave cave;
static Vec startpos;

// Sign of x: x > 0 => +1, x == 0 => 0, x < 0 => -1
static int sign(int x)
{
    return (x > 0) - (x < 0);
}

static Vec signdot(const Vec p)
{
    return (Vec){sign(p.x), sign(p.y)};
}

static bool equal(const Vec a, const Vec b)
{
    return a.x == b.x && a.y == b.y;
}

static void addto(Vec *const a, const Vec b)
{
    a->x += b.x;
    a->y += b.y;
}

static Vec add(const Vec a, const Vec b)
{
    return (Vec){a.x + b.x, a.y + b.y};
}

static Vec sub(const Vec a, const Vec b)
{
    return (Vec){a.x - b.x, a.y - b.y};
}

static int gridindex(const Vec p)
{
    const Vec q = sub(p, cave.min);
    return q.y * cave.dim.x + q.x;
}

static bool isfree(const Vec p)
{
    return cave.grid[gridindex(p)] == SPACE;
}

static void place(const Vec p, const uint8_t material)
{
    cave.grid[gridindex(p)] = material;
}

static void makegrid(Cave *const c)
{
    c->dim = sub(c->max, c->min);
    addto(&c->dim, (Vec){1,1});  // dim = max - min + 1
    c->size = c->dim.x * c->dim.y;
    c->grid = calloc((size_t)c->size, sizeof *c->grid);
}

#if EXAMPLE
static void show(void)
{
    printf("\n");
    int i = 0, j = index(inlet);
    for (int y = 0; y < cave.dim.y; ++y) {
        for (int x = 0; x < cave.dim.x; ++x, ++i)
            printf("%c", cave.grid[i] == SAND ? 'o' : (cave.grid[i] == ROCK ? '#' : (i == j ? '+' : '.')));
        printf("\n");
    }
}
#endif

// 0=left, 1=right, 2=down
static void grow(const int direction)
{
    int l = 0, r = 0, d = 0;
    switch (direction) {
        case 0: l = GROW; break;
        case 1: r = GROW; break;
        case 2: d = 2; break;
    }

    Cave big;
    big.min = sub(cave.min, (Vec){l,0});
    big.max = add(cave.max, (Vec){r,d});
    makegrid(&big);
    int i = 0, j = l;
    for (int y = 0; y < cave.dim.y; ++y) {
        for (int x = 0; x < cave.dim.x; ++x, ++i, ++j)
            big.grid[j] = cave.grid[i];
        j += l + r;
    }
    free(cave.grid);
    cave = big;

    // Add rock from i to j (exclusive)
    i = cave.size - cave.dim.x;  // bottom left corner
    j = cave.size;               // bottom right corner
    switch (direction) {
        case 0: j = i + l; break;
        case 1: i = cave.size - r; break;
    }
    for (int k = i; k < j; ++k)
        cave.grid[k] = ROCK;
}

static bool offgrid(const Vec p)
{
    return p.x < cave.min.x || p.x > cave.max.x || p.y > cave.max.y;
}

// Drop grain of sand from inlet until no more room or into the abyss
// Place sand at resting place & return true
// Return false if initial position occupied or into the abyss
static bool drop(const int part)
{
    if (!isfree(inlet))
        return false;

    Vec p = startpos;
    while (!offgrid(p) && isfree(p))
fallfurther:
        addto(&p, down);
    if (part == 1 && offgrid(p))
        return false;

    // Down is occupied, try left
    addto(&p, left);
    if (offgrid(p)) {  // off the left edge
        if (part == 1)
            return false;  // into the abyss
        grow(0);  // part 2: make extra room on the left
    }
    if (isfree(p))
        goto fallfurther;

    // Left is occupied, try right
    addto(&p, right);
    if (offgrid(p)) {  // off the right edge
        if (part == 1)
            return false;  // into the abyss
        grow(1);  // extra room on the right
    }
    if (isfree(p))
        goto fallfurther;

    // Down/left/right all occupied, so back up to last known free position
    addto(&p, back);
    place(p, SAND);
    return true;  // stopped = there was still room for this grain of sand
}

static int pour(const int part)
{
    int grains = 0;
    while (drop(part))
        ++grains;
#if EXAMPLE
    show();
#endif
    return grains;
}

static void removesand(void)
{
    int i = 0;
    for (int y = 0; y < cave.dim.y; ++y)
        for (int x = 0; x < cave.dim.x; ++x, ++i)
            if (cave.grid[i] == SAND)
                cave.grid[i] = SPACE;
}

static void read(void)
{
    FILE *f = fopen(NAME, "r");
    if (!f)
        exit(1);

    // Get dimensions
    cave.min = inlet;
    cave.max = inlet;
    int x, y;
    while (!feof(f) && fscanf(f, "%d,%d", &x, &y) == 2) {
        if (x < cave.min.x) cave.min.x = x;
        if (x > cave.max.x) cave.max.x = x;
        if (y < cave.min.y) cave.min.y = y;
        if (y > cave.max.y) cave.max.y = y;
        if (fgetc(f) == ' ')             // space or newline
            for (int i = 0; i < 3; ++i)  // consume "-> "
                fgetc(f);
    }
    makegrid(&cave);

    // Place rock
    rewind(f);
    Vec p0, p1, diff, step;
    while (!feof(f) && fscanf(f, "%d,%d", &p0.x, &p0.y) == 2)
        while (fgetc(f) == ' ') {        // space or newline
            for (int i = 0; i < 3; ++i)  // consume "-> "
                fgetc(f);
            place(p0, ROCK);             // start of rock line
            if (fscanf(f, "%d,%d", &p1.x, &p1.y) == 2) {
                diff = sub(p1, p0);
                step = signdot(diff);
                while (!equal(p0, p1)) {
                    addto(&p0, step);
                    place(p0, ROCK);
                }
            }
        }
    fclose(f);
#if EXAMPLE
    show();
#endif
}

int main(void)
{
    starttimer();
    read();
    startpos = inlet;
    printf("Part 1: %d\n", pour(1));  // example=24, input=1330
    removesand();                     // reset cave
    grow(2);                          // add rock row at bottom
    startpos = inlet;
    printf("Part 2: %d\n", pour(2));  // example=93, input=26139
    free(cave.grid);
    printf("Time: %.0f us\n", stoptimer_us());
    return 0;
}
