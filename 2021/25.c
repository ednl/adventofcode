#include <stdio.h>
#include <stdbool.h>

static const char *fname = "../aocinput/2021-25-input.txt";
#define W 139
#define H 137
static char a[2][H][W] = {0};

static bool evolve(void)
{
    char c;
    bool moved = false;
    // East
    for (int i = 0; i < H; ++i)
        for (int j = 0; j < W; )
            if ((c = a[0][i][j]) == '>') {
                int k = j + 1;
                if (k == W)
                    k = 0;
                if (a[0][i][k] == '.') {
                    a[1][i][j] = '.';
                    a[1][i][k] = c;
                    j += 2;
                    moved = true;
                } else
                    a[1][i][j++] = c;
            } else
                a[1][i][j++] = c;
    // South
    for (int j = 0; j < W; ++j)
        for (int i = 0; i < H; )
            if ((c = a[1][i][j]) == 'v') {
                int k = i + 1;
                if (k == H)
                    k = 0;
                if (a[1][k][j] == '.') {
                    a[0][i][j] = '.';
                    a[0][k][j] = c;
                    i += 2;
                    moved = true;
                } else
                    a[0][i++][j] = c;
            } else
                a[0][i++][j] = c;
    return moved;
}

int main(void)
{
    FILE *f = fopen(fname, "r");
    for (int i = 0; i < H; ++i) {
        for (int j = 0; j < W; ++j)
            a[0][i][j] = (char)fgetc(f);
        fgetc(f);
    }
    fclose(f);

    int step = 1;  // strange halting condition in the puzzle
    while (evolve())
        ++step;
    printf("Part 1: %d\n", step);
    return 0;
}
