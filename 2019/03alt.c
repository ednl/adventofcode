#include <stdio.h>
#include <stdlib.h>

#define N 160

typedef struct seg {
    int x0, x1, y0, y1, d0, d1;
} Seg;

static Seg wire[2][N];

static int asc_x(const void *p, const void *q)
{
    const int a = ((const Seg *)p)->x0;
    const int b = ((const Seg *)q)->x0;
    if (a < b) return -1;
    if (a > b) return  1;
    return 0;
}

static int asc_y(const void *p, const void *q)
{
    const int a = ((const Seg *)p)->y0;
    const int b = ((const Seg *)q)->y0;
    if (a < b) return -1;
    if (a > b) return  1;
    return 0;
}

int main(void)
{
    FILE *f = fopen("../aocinput/2019-03-input.txt", "r");
    if (!f) return 1;
    int i = 0, j = 0, x0 = 0, y0 = 0, d0 = 0, x1, y1, d1, c;
    int xmin = 0, xmax = 0, ymin = 0, ymax = 0;
    while ((c = fgetc(f)) != '\n') {
        int dir = c;
        int num = 0;
        while ((c = fgetc(f)) >= '0' && c <= '9')
            num = num * 10 + (c & 15);
        switch (dir) {
            case 'U': x1 = x0; y1 = y0 - num; if (y1 < ymin) ymin = y1; break;
            case 'D': x1 = x0; y1 = y0 + num; if (y1 > ymax) ymax = y1; break;
            case 'L': x1 = x0 - num; y1 = y0; if (x1 < xmin) xmin = x1; break;
            case 'R': x1 = x0 + num; y1 = y0; if (x1 > xmax) xmax = x1; break;
        }
        d1 = d0 + num;
        wire[i][j] = (Seg){x0, x1, y0, y1, d0, d1};
        if (c == '\n')
            break;
        x0 = x1;
        y0 = y1;
        d0 = d1;
        i++;
        if (i == 2) {
            i = 0;
            j++;
        }
    }
    printf("%d %d %d %d\n", xmin, xmax, ymin, ymax);
    printf("%d %d\n", i, j);
    if (wire[0][0].x0 == wire[0][0].x1) {
        qsort(wire[0], j + 1, sizeof (Seg), asc_x);
        qsort(wire[1], j + i, sizeof (Seg), asc_y);
    } else {
        qsort(wire[0], j + 1, sizeof (Seg), asc_y);
        qsort(wire[1], j + i, sizeof (Seg), asc_x);
    }
    for (i = 0; i <= j; ++i)
        printf("%3d: %5d %5d %5d %5d %6d %6d | %5d %5d %5d %5d %6d %6d\n", i,
            wire[0][i].x0, wire[0][i].x1, wire[0][i].y0, wire[0][i].y1, wire[0][i].d0, wire[0][i].d1,
            wire[1][i].x0, wire[1][i].x1, wire[1][i].y0, wire[1][i].y1, wire[1][i].d0, wire[0][i].d1
        );
    fclose(f);
}
