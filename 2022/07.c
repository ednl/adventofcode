/**
 * Advent of Code 2022
 * Day 7: No Space Left On Device
 * https://adventofcode.com/2022/day/7
 * By: E. Dronkert https://github.com/ednl
 */

#include <stdio.h>
#include <stdlib.h>  // atoi

#define EXAMPLE 0
#if EXAMPLE
    #define INP "../aocinput/2022-07-example.txt"
#else
    #define INP "../aocinput/2022-07-input.txt"
#endif

#define DIRS      256  // maximum number of subdirs (190 for my input)
#define MAX1   100000  // part 1: max dir size to include in sum
#define DISK 70000000  // total disk size
#define NEED 30000000  // part 2: free space needed

static FILE *f;
static int dircount, dirsize[DIRS];

static int subdir(void)
{
    int sum = 0;
    char line[32];
    while (fgets(line, sizeof line, f))
        if (line[2] == 'c') {    // change directory
            if (line[5] == '.')  // "cd .." = up
                return dirsize[dircount++] = sum;
            else                 // "cd <subdir>" = down
                sum += subdir();
        } else                   // file entry (or else atoi=0)
            sum += atoi(line);
    return dirsize[dircount++] = sum;
}

int main(void)
{
    if (!(f = fopen(INP, "r")))
        return 1;
    int minsize = subdir() + NEED - DISK;
    fclose(f);

#if EXAMPLE
    puts("All dir sizes:");
    for (int i = 0; i < dircount; ++i)
        printf("%d %d\n", i, dirsize[i]);
    printf("\n");
#endif

    int part1 = 0, part2 = DISK;
    for (int i = 0; i < dircount; ++i) {
        if (dirsize[i] <= MAX1)
            part1 += dirsize[i];
        if (dirsize[i] >= minsize && dirsize[i] < part2)
            part2 = dirsize[i];
    }
    printf("Part 1: %d\n", part1);  // example:    95437, input: 1845346
    printf("Part 2: %d\n", part2);  // example: 24933642, input: 3636703
    return 0;
}
