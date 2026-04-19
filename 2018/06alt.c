#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define FNAME "../aocinput/2018-06-input.txt"
#define N 50

// typedef struct vec2 {
//     int x, y;
// } Vec2;

// static Vec2 coor[N];
static int x[N];
static int y[N];

static int intasc(const void *p, const void *q)
{
    const int a = *(const int *)p;
    const int b = *(const int *)q;
    if (a < b) return -1;
    if (a > b) return  1;
    return 0;
}

int main(void)
{
    FILE *f = fopen(FNAME, "r");
    if (!f) return 1;
    for (int i = 0; i < N && fscanf(f, "%d, %d ", &x[i], &y[i]) == 2; ++i);
    fclose(f);

    qsort(x, N, sizeof *x, intasc);
    qsort(y, N, sizeof *y, intasc);
    int dx = 0, dy = 0;
    for (int i = 0; i < N; ++i) {
        dx += x[i];
        dy += y[i];
        printf("%2d: %3d %3d\n", i, x[i], y[i]);
    }
    printf("%d %d\n", dx, dy);
}
