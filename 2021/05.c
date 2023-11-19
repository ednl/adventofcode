#include <stdio.h>
#include <stdbool.h>

#define LINES  500
#define DIM   1000

typedef struct {
    int x0, y0, x1, y1, dx, dy;
    bool diag;
} Line;

static Line line[LINES] = {0};
static int vent[DIM][DIM] = {0};

static int multivent(bool diag)
{
    for (int i = 0; i < LINES; ++i) {
        if (diag == line[i].diag) {
            int x = line[i].x0;
            int y = line[i].y0;
            while (x != line[i].x1 || y != line[i].y1) {
                vent[x][y]++;
                x += line[i].dx;
                y += line[i].dy;
            }
            vent[x][y]++;
        }
    }

    int multiple = 0;
    for (int i = 0; i < DIM; ++i)
        for (int j = 0; j < DIM; ++j)
            multiple += vent[i][j] > 1;
    return multiple;
}

int main(void)
{
    FILE *f = fopen("../aocinput/2021-05-input.txt", "r");
    if (!f)
        return 1;

    int i = 0, x0, y0, x1, y1, dx, dy;
    while (i < LINES && fscanf(f, "%d,%d -> %d,%d ", &x0, &y0, &x1, &y1) == 4) {
        dx = x0 < x1 ? 1 : (x0 > x1 ? -1 : 0);
        dy = y0 < y1 ? 1 : (y0 > y1 ? -1 : 0);
        line[i++] = (Line){x0, y0, x1, y1, dx, dy, dx && dy};
    }
    fclose(f);
    if (i != LINES)
        return 2;

    printf("Part 1: %u\n", multivent(false));
    printf("Part 2: %u\n", multivent(true));
    return 0;
}
