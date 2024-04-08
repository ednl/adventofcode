#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define S 500   // spring at x=S,y=0
#define N 2048  // max. lines in input file (my input: 1832)

typedef struct line {
    int x1, x2, y1, y2;
} Line;

// Clay barrier on horz or vert line x1..x2,y1..y2 where either x1==x2 or y1==y2
static Line clay[N];

int main(void)
{
    FILE *f = fopen("../aocinput/2018-17-input.txt", "r");
    if (!f) { fputs("File not found.\n", stderr); return 1; }

    int n = 0;
    char k1, k2;
    int v1, v2, v3;
    while (n < N && fscanf(f, "%c=%d, %c=%d..%d ", &k1, &v1, &k2, &v2, &v3) == 5)
        clay[n++] = k1 == 'x' ? (Line){v1, v1, v2, v3} : (Line){v2, v3, v1, v1};
    fclose(f);

    int xmin = S, xmax = S;
    int ymin = N, ymax = 0;
    for (int i = 0; i < n; ++i) {
        if (clay[i].x1 < xmin) xmin = clay[i].x1;
        if (clay[i].x2 > xmax) xmax = clay[i].x2;
        if (clay[i].y1 < ymin) ymin = clay[i].y1;
        if (clay[i].y2 > ymax) ymax = clay[i].y2;
    }
    --xmin;  // empty column left
    ++xmax;  // empty column right
    --ymin;  // room for spring
    for (int i = 0; i < n; ++i) {
        clay[i].x1 -= xmin;
        clay[i].x2 -= xmin;
        clay[i].y1 -= ymin;
        clay[i].y2 -= ymin;
    }

    int width = xmax - xmin + 3;  // +\n\0
    int height = ymax - ymin + 1;
    int gridsize = width * height;  // my input: 520335 = 509 kB
    // printf("x: %d..%d = %d\ny: %d..%d = %d\nsize = %d\n", xmin, xmax, width, ymin, ymax, height, gridsize);

    char *grid = malloc((size_t)gridsize);
    if (!grid) { fputs("Out of memory.\n", stderr); return 2; }
    memset(grid, ' ', gridsize);
    for (int i = width - 2; i < gridsize; i += width) {
        grid[i] = '\n';
        grid[i + 1] = '\0';
    }

    for (int i = 0; i < n; ++i) {
        const int step = clay[i].x1 == clay[i].x2 ? width : 1;
        const char *const end = grid + clay[i].y2 * width + clay[i].x2 + step;
        for (char *c = grid + clay[i].y1 * width + clay[i].x1; c != end; c += step)
            *c = '#';
    }

    int spring = S - xmin;
    grid[spring] = '+';

    int x = spring, y = 0;
    while (y < height - 1 && grid[(y + 1) * width + x] == ' ')
        grid[++y * width + x] = '|';
    if (y < height - 1) {
        int x1 = x;
        while (x1 > 0 && grid[(y + 1) * width + x1] != ' ' && grid[y * width + x1 - 1] == ' ')
            grid[y * width + --x1] = '|';
        int x2 = x;
        while (x2 < width - 1 && grid[(y + 1) * width + x2] != ' ' && grid[y * width + x2 + 1] == ' ')
            grid[y * width + ++x2] = '|';
    }

    f = fopen("17.txt", "w");
    if (!f) { fputs("Could not write to file.\n", stderr); free(grid); return 3; }
    for (int i = 0; i < gridsize; i += width)
        fputs(&grid[i], f);
    fclose(f);

    free(grid);
    return 0;
}
