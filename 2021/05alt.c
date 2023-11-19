// Idea from /u/crowbarous at https://www.reddit.com/r/adventofcode/comments/r9824c/2021_day_5_solutions/hnav2mt/
// Added part as argument
#include <stdio.h>
static char vent[1024][1024] = {0};
int main(int argc, char *argv[])
{
    int part = argc > 1 && argv && argv[1] ? *argv[1] - '0' : 0, multi = 0;
    FILE *f = fopen("../aocinput/2021-05-input.txt", "r");
    for (int x, y, x1, y1; fscanf(f, "%d,%d -> %d,%d ", &x, &y, &x1, &y1) == 4; )
    {
        int dx = (x < x1) - (x1 < x), dy = (y < y1) - (y1 < y);
        if (part == 2 || !dx || !dy)
            for (x1 += dx, y1 += dy; x != x1 || y != y1; x += dx, y += dy)
                multi += ++vent[x][y] == 2;
    }
    fclose(f);
    printf("Part %d: %d\n", part == 2 ? 2 : 1, multi);
    return 0;
}
