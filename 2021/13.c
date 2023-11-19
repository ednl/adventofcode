#include <stdio.h>
#include <stdlib.h>
#include "startstoptimer.h"

#define ND 866
#define NF  12

static int dot[ND][2] = {0};
static int fold[NF][2] = {0};

static void readinput(void)
{
    FILE *f = fopen("../aocinput/2021-13-input.txt", "r");
    for (int i = 0; i < ND && fscanf(f, "%d,%d ", &dot[i][0], &dot[i][1]) == 2; ++i);
    char c;
    for (int i = 0; i < NF && fscanf(f, "fold along %c=%d ", &c, &fold[i][1]) == 2; ++i)
        fold[i][0] = c - 'x';  // "x" => axis 0, "y" => axis 1
    fclose(f);
}

// Sort by y,x
static int yx_asc(const void *a, const void *b)
{
    const int *p = a;
    const int *q = b;
    if (p[1] < q[1]) return -1;
    if (p[1] > q[1]) return  1;
    if (p[0] < q[0]) return -1;
    if (p[0] > q[0]) return  1;
    return 0;
}

static void dofold(int i)
{
    const int axis = fold[i][0], val = fold[i][1];
    int dist;
    for (int j = 0; j < ND; ++j)
        if ((dist = dot[j][axis] - val) > 0)
            dot[j][axis] -= dist * 2;
}

static int visible(void)
{
    qsort(dot, ND, sizeof(int[2]), yx_asc);
    int i = 0, n = 1;
    while (++i < ND)
         n += dot[i][0] != dot[i - 1][0] || dot[i][1] != dot[i - 1][1];
    return n;
}

static void show(void)
{
    qsort(dot, ND, sizeof(int[2]), yx_asc);
    int i = 0, x = 0, y = dot[0][1];
    printf("\n");
    while (i < ND) {
        for (; x < dot[i][0]; ++x)
            printf(" ");
        while (i < ND && x == dot[i][0] && y == dot[i][1])
            ++i;
        printf("#");
        ++x;
        if (i < ND && y != dot[i][1]) {
            printf("\n");
            x = 0;
            y = dot[i][1];
        }
    }
    printf("\n\n");
}

int main(void)
{
    starttimer();
    readinput();
    dofold(0);
    printf("Part 1: %d\n", visible());  // 708
    printf("Part 2:\n");                // EBLUBRFH
    for (int i = 1; i < NF; ++i)
        dofold(i);
    show();
    printf("%.1f µs\n", stoptimer_us());
    return 0;
}
