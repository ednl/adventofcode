/**
 * Advent of Code 2023
 * Day 11: Cosmic Expansion
 * https://adventofcode.com/2023/day/11
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -Ofast -march-native -Wall 11.c ../startstoptimer.c
 *    gcc   -std=gnu17 -Ofast -march-native -Wall 11.c ../startstoptimer.c
 * Get minimum runtime:
 *     m=50000;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * Minimum runtime:
 *     Apple M1 Mac Mini      : 117 ns
 *     Raspberry Pi 5 2.4 GHz : 406 ns
 */

#include <stdio.h>     // fopen, fclose, fgets, printf
#include <stdlib.h>    // malloc, free
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRId64
#include <stdbool.h>   // bool
#include "../startstoptimer.h"

#define EXAMPLE 0
#if EXAMPLE
#define NAME "../aocinput/2023-11-example.txt"
#define N 10   // rows and columns in image map
#define M 100  // empty space expansion factor
#else
#define NAME "../aocinput/2023-11-input.txt"
#define N 140      // rows and columns in image map
#define M 1000000  // empty space expansion factor
#endif

// Vector
typedef struct vec {
    int64_t x, y;
} Vec;

// Galaxy map: each galaxy has an image position and expanded position
typedef struct map {
    Vec pos, exp;
} Map;

// Space image from input.
static char image[N][N + 2];  // +2: room for '\n\0'

// Amount of expansion at every coordinate (gx,gy).
//   stretch[gx].x = how many empty columns up to & incl. column index gx
//   stretch[gy].y = how many empty rows up to & incl. row index gy
static Vec stretch[N];

// Add vectors (return new vector).
static Vec add(const Vec v, const Vec w)
{
    return (Vec){v.x + w.x, v.y + w.y};
}

// Add vectors (in-place using reference).
static void add_r(Vec* v, const Vec w)
{
    v->x += w.x;
    v->y += w.y;
}

// Multiply vector with constant (return new vector).
static Vec mul(const Vec v, const int64_t factor)
{
    return (Vec){v.x * factor, v.y * factor};
}

// Absolute value of integer constant.
static int64_t absint(const int64_t n)
{
    return n < 0 ? -n : n;
}

// Manhattan distance between vectors.
static int64_t manh(const Vec v, const Vec w)
{
    return absint(v.x - w.x) + absint(v.y - w.y);
}

// Expand one set of coordinates (factor=1: no expansion).
// Use previously computed global stretch list.
static Vec expand(const Vec pos, const int64_t factor)
{
    return add(pos, mul((Vec){stretch[pos.x].x, stretch[pos.y].y}, factor - 1));
}

// Sum of distances between every pair of galaxies in expanded space.
static int64_t dist(Map* map, const size_t len, const int64_t expansionfactor)
{
    Map* m = map;
    for (size_t i = 0; i < len; ++i, ++m)
        m->exp = expand(m->pos, expansionfactor);

    #if EXAMPLE || defined(DEBUG)
    printf("\n");
    m = map;
    for (size_t i = 0; i < len; ++i, ++m)
        printf("%zu: (%"PRId64",%"PRId64") -> (%3"PRId64",%3"PRId64")\n", i, m->pos.x, m->pos.y, m->exp.x, m->exp.y);
    printf("\n");
    #endif

    int64_t sum = 0;
    for (size_t i = 0; i < len - 1; ++i)
        for (size_t j = i + 1; j < len; ++j)  // every unique pair (i,j)=(j,i)
            sum += manh(map[i].exp, map[j].exp);
    return sum;
}

int main(void)
{
    starttimer();
    FILE* f = fopen(NAME, "r");
    if (!f)
        return 1;
    for (size_t i = 0; i < N; ++i)
        fgets(image[i], sizeof *image, f);
    fclose(f);

    #if EXAMPLE || defined(DEBUG)
    printf("Image:\n");
    for (size_t i = 0; i < N; ++i)
        printf("%s", image[i]);
    printf("\n");
    #endif

    // Which rows/cols are empty space and will be expanded?
    for (size_t i = 0; i < N; ++i) {  // rows or cols
        // Check for empty rows[i].
        bool empty = true;
        for (size_t j = 0; j < N; ++j)
            if (image[i][j] == '#') {  // all cols[j] in row[i]
                empty = false;
                break;
            }
        stretch[i].y = empty;  // 0 or 1
        // Check for empty cols[i].
        empty = true;
        for (size_t j = 0; j < N; ++j)
            if (image[j][i] == '#') {  // all rows[j] in col[i]
                empty = false;
                break;
            }
        stretch[i].x = empty;  // 0 or 1
    }

    // Cumulative sum of stretch per row,col.
    //   stretch[i].x = how many columns of empty space in image columns [0..i]
    //   stretch[i].y = how many rows of empty space in image rows [0..i]
    // So, for expansion factor 2, add 1x stretch[gx].x to galaxies with x-coordinate gx.
    for (size_t i = 1; i < N; ++i)
        add_r(&stretch[i], stretch[i - 1]);

    #if EXAMPLE || defined(DEBUG)
    printf("Stretch cols: ");
    for (size_t i = 0; i < N; ++i)
        printf(" %"PRId64, stretch[i].x);
    printf("\nStretch rows: ");
    for (size_t i = 0; i < N; ++i)
        printf(" %"PRId64, stretch[i].y);
    printf("\n");
    #endif

    // Count galaxies.
    size_t count = 0;
    for (size_t i = 0; i < N; ++i)
        for (size_t j = 0; j < N; ++j)
            count += image[i][j] == '#';

    // Save image coordinates of galaxies to list.
    Map* galaxy = malloc(count * sizeof *galaxy);
    size_t k = 0;
    for (size_t i = 0; i < N; ++i)
        for (size_t j = 0; j < N; ++j)
            if (image[i][j] == '#')
                galaxy[k++].pos = (Vec){(int64_t)j, (int64_t)i};

    printf("Part 1: %"PRId64"\n", dist(galaxy, count, 2));  // ex:  374, input:      9608724
    printf("Part 2: %"PRId64"\n", dist(galaxy, count, M));  // ex: 8410, input: 904633799472

    free(galaxy);
    printf("Time: %.0f us\n", stoptimer_us());
    return 0;
}
