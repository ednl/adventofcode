#include <stdio.h>
#include "../startstoptimer.h"

#define SERIAL  2866  // my puzzle input
#define GRIDSIZE 300

typedef struct power {
    int totalpower, row, col, squaresize;
} Power;

static int grid[GRIDSIZE][GRIDSIZE], rowsum[GRIDSIZE][GRIDSIZE], sqrsum[GRIDSIZE][GRIDSIZE];

// 1 <= squaresize <= GRIDSIZE
static Power largesttotalpower(int squaresize)
{
    Power ltp = {0};
    for (int j = 0; j < GRIDSIZE - squaresize + 1; ++j) {
        sqrsum[0][j] = rowsum[0][j];
        for (int i = 1; i < squaresize; ++i)
            sqrsum[0][j] += rowsum[i][j];
        if (sqrsum[0][j] > ltp.totalpower)
            ltp = (Power){sqrsum[0][j], 0, j, squaresize};
        for (int i = 1; i < GRIDSIZE - squaresize + 1; ++i) {
            sqrsum[i][j] = sqrsum[i - 1][j] - rowsum[i - 1][j] + rowsum[i + squaresize - 1][j];
            if (sqrsum[i][j] > ltp.totalpower)
                ltp = (Power){sqrsum[i][j], i, j, squaresize};
        }
    }
    return ltp;
}

static int power(int row, int col)
{
    return ((col + 10) * row + SERIAL) * (col + 10) / 100 % 10 - 5;
}

int main(void)
{
    starttimer();
    for (int i = 0; i < GRIDSIZE; ++i)
        for (int j = 0; j < GRIDSIZE; ++j)
            grid[i][j] = power(i + 1, j + 1);

    int squaresize = 3;
    for (int i = 0; i < GRIDSIZE; ++i) {
        rowsum[i][0] = grid[i][0];
        for (int j = 1; j < squaresize; ++j)
            rowsum[i][0] += grid[i][j];
        for (int j = 1; j < GRIDSIZE - squaresize + 1; ++j)
            rowsum[i][j] = rowsum[i][j - 1] - grid[i][j - 1] + grid[i][j + squaresize - 1];
    }
    Power max = largesttotalpower(squaresize);
    printf("Part 1: %d,%d\n", max.col + 1, max.row + 1);  // 20,50

    int declinestreak = 0;
    while (declinestreak < 2 && ++squaresize <= GRIDSIZE) {
        for (int i = 0; i < GRIDSIZE; ++i)
            for (int j = 0; j < GRIDSIZE - squaresize + 1; ++j)
                rowsum[i][j] += grid[i][j + squaresize - 1];
        Power cur = largesttotalpower(squaresize);
        if (cur.totalpower > max.totalpower) {
            max = cur;
            declinestreak = 0;
        } else
            declinestreak++;
    }
    printf("Part 2: %d,%d,%d\n", max.col + 1, max.row + 1, max.squaresize);  // 238,278,9
    printf("Time: %.0f us\n", stoptimer_us());
    return 0;
}
