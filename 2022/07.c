/**
 * Advent of Code 2022
 * Day 7: No Space Left On Device
 * https://adventofcode.com/2022/day/7
 * By: E. Dronkert https://github.com/ednl
 */

#include <stdio.h>
#include <stdlib.h>  // strtoul

#define DIRS      (256ul)  // maximum number of subdirs (190 for my input)
#define MAX1   (100000ul)  // part 1: max dir size to include in sum
#define DISK (70000000ul)  // total disk size
#define NEED (30000000ul)  // part 2: free space needed

static FILE *f;
static size_t dircount, dirsize[DIRS];

static size_t subdir(void)
{
    size_t sum = 0;
    char line[32];
    while (fgets(line, sizeof line, f))
        if (line[2] == 'c') {    // change directory
            if (line[5] == '.')  // cd ..
                return dirsize[dircount++] = sum;
            else                // cd <subdir>
                sum += subdir();
        } else                  // file entry (or else strtoul()=0)
            sum += strtoul(line, NULL, 10);
    return dirsize[dircount++] = sum;
}

int main(void)
{
    if (!(f = fopen("../aocinput/2022-07-input.txt", "r")))
        return 1;
    size_t minsize = subdir() + NEED - DISK;
    fclose(f);

    size_t part1 = 0, part2 = DISK;
    for (size_t i = 0; i < dircount; ++i) {
        if (dirsize[i] <= MAX1)
            part1 += dirsize[i];
        if (dirsize[i] >= minsize && dirsize[i] < part2)
            part2 = dirsize[i];
    }
    printf("Part 1: %zu\n", part1);  // 1845346
    printf("Part 2: %zu\n", part2);  // 3636703
    return 0;
}
