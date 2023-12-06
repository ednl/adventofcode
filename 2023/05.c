/**
 * Advent of Code 2023
 * Day 5: If You Give A Seed A Fertilizer
 * https://adventofcode.com/2023/day/5
 * By: E. Dronkert https://github.com/ednl
 */

#include <stdio.h>     // fopen, fclose, fscanf, printf
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRId64

// Input parameters
#define EXAMPLE 0
#if EXAMPLE == 1
    #define NAME "../aocinput/2023-05-example.txt"
    #define SEEDS  4
    #define CONVS  5  // max 4 conversion ranges per cat in example
#else
    #define NAME "../aocinput/2023-05-input.txt"
    #define SEEDS 20
    #define CONVS 50  // max 46 conversion ranges per cat in my input
#endif
#define CATS 8  // categories: seed, soil, fertilizer, water, light, temperature, humidity, location
#define MAPS (CATS - 1)  // translation from cat(n) to cat(n+1)

// Data structures
typedef int64_t i64;
typedef struct Conv {
    i64 src, end, ofs, len;
} Conv;
typedef struct Map {
    Conv conv[CONVS];
    int len;  // actual number of conversion ranges used
} Map;

// Global data
static i64 seed[SEEDS];
static Map map[MAPS];

static i64 min(const i64 a, const i64 b)
{
    return a < b ? a : b;
}

static i64 convert1(i64 src)
{
    for (int i = 0; i < MAPS; ++i)
        for (int j = 0; j < map[i].len; ++j)
            if (src >= map[i].conv[j].src && src <= map[i].conv[j].end) {
                src += map[i].conv[j].ofs;
                break;
            }
    return src;
}

int main(void)
{
    FILE* f = fopen(NAME, "r");
    if (!f)
        return 1;
    while (fgetc(f) != ':');  // skip to first seed number
    for (int i = 0; i < SEEDS; ++i)  // read seed numbers
        fscanf(f, "%"PRId64, &seed[i]);
    while (fgetc(f) != ':');  // skip to first conversion of first map
    for (int i = 0; i < MAPS; ++i) {
        i64 dst, src, len;
        while (map[i].len < CONVS && fscanf(f, "%"PRId64" %"PRId64" %"PRId64, &dst, &src, &len) == 3)
            map[i].conv[map[i].len++] = (Conv){src, src + len - 1, dst - src, len};
        while (!feof(f) && fgetc(f) != ':');  // skip to first conversion of next map
    }
    fclose(f);

    i64 part1 = INT64_MAX;
    for (int i = 0; i < SEEDS; ++i)
        part1 = min(part1, convert1(seed[i]));
    printf("%"PRId64"\n", part1);  // example: 35, input: 836040384

    return 0;
}
