#include <stdio.h>
#include <string.h>  // memcpy

#define W 31
#define H 323

typedef struct slope {
    int x, y;
} Slope;

static const Slope slope[] = {
    {1, 1},
    {3, 1},  // slope for part 1
    {5, 1},
    {7, 1},
    {1, 2},
};

static char area[H][W + 1];

int main(void)
{
    FILE *f = fopen("../aocinput/2020-03-input.txt", "r");
    if (!f)
        return 1;
    char line[64];
    for (int i = 0; fgets(line, sizeof line, f); ++i)
        memcpy(&area[i][0], line, W);
    fclose(f);

    unsigned mult = 1;
    for (size_t i = 0; i < sizeof slope / sizeof *slope; ++i) {
        unsigned tree = 0;
        for (int x = 0, y = 0; y < H; x = (x + slope[i].x) % W, y += slope[i].y)
            tree += area[y][x] == '#';
        mult *= tree;
        if (i == 1)
            printf("Part 1: %u\n", tree);  // 247
    }
    printf("Part 2: %u\n", mult);  // 2983070376
    return 0;
}
