#include <stdio.h>

#define SERIAL  2866
#define GRIDSIZE 300

typedef struct {
    int totalpower, row, col, squaresize;
} power_t;

static int grid[GRIDSIZE][GRIDSIZE];

// One-based row/col indices [1..GRIDSIZE]
static int power(int row, int col)
{
    return ((col + 10) * row + SERIAL) * (col + 10) / 100 % 10 - 5;
}

int main(void)
{
    // Init
    for (int i = 0; i < GRIDSIZE; ++i)
        for (int j = 0; j < GRIDSIZE; ++j)
            grid[i][j] = power(i + 1, j + 1);

    power_t max = {0};
    int squaresize = 3, declinestreak = 0;
    while (declinestreak < 2 && squaresize <= GRIDSIZE) {
        power_t cur = {0};
        for (int i = 0; i < GRIDSIZE - squaresize + 1; ++i)
            for (int j = 0; j < GRIDSIZE - squaresize + 1; ++j) {
                int totalpower = 0;
                for (int y = 0; y < squaresize; ++y)
                    for (int x = 0; x < squaresize; ++x)
                        totalpower += grid[i + y][j + x];
                if (totalpower > cur.totalpower)
                    cur = (power_t){totalpower, i, j, squaresize};
            }
        if (cur.totalpower > max.totalpower) {
            max = cur;
            declinestreak = 0;
            if (squaresize == 3)
                printf("Part 1: %d,%d\n", max.col + 1, max.row + 1);
        } else
            declinestreak++;
        ++squaresize;
    }
    printf("Part 2: %d,%d,%d\n", max.col + 1, max.row + 1, max.squaresize);
    return 0;
}
