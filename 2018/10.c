#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

// Number of points in my input file
#define N 324U

typedef struct {
    int x, y;
} Vec;

// Old and new positions, constant velocity
static Vec pos0[N] = {0}, pos1[N] = {0}, vel[N] = {0};

// Read input file
static bool read(void)
{
    FILE *f = fopen("../aocinput/2018-10-input.txt", "r");
    if (!f) {
        return false;
    }

    size_t n = 0;
    Vec p, v;
    while (n < N && fscanf(f, "position=< %d, %d> velocity=< %d, %d> ", &p.x, &p.y, &v.x, &v.y) == 4) {
        pos0[n] = p;
        vel[n] = v;
        n++;
    }
    fclose(f);
    return n == N;
}

// Sort first by y then by x coordinate
static int cmp_pos(const void *const a, const void *const b)
{
    const Vec *const p = a;
    const Vec *const q = b;
    if (p->y < q->y) return -1;
    if (p->y > q->y) return  1;
    if (p->x < q->x) return -1;
    if (p->x > q->x) return  1;
    return 0;
}

int main(void)
{
    // Read input file into globals pos0[] and vel[]
    if (!read()) {
        return 1;
    }

    // Intersection: x0 + vx0.t = x1 + vx1.t
    //   => t = (x0 - x1) / (vx1 - vx0)
    // will be close approx of final t
    size_t i = 1;
    while (pos0[i].x == pos0[0].x || vel[i].x == vel[0].x) {
        i++;  // skip parallel lines or (unlikely) already overlapping points
    }
    int t = (pos0[0].x - pos0[i].x) / (vel[i].x - vel[0].x);

    // Go to initial position in one big step
    // and determine spread of the points
    Vec pmin = {INT_MAX, INT_MAX};
    Vec pmax = {INT_MIN, INT_MIN};
    for (i = 0; i < N; ++i) {
        pos0[i].x += vel[i].x * t;
        pos0[i].y += vel[i].y * t;
        if (pos0[i].x < pmin.x) pmin.x = pos0[i].x;
        if (pos0[i].x > pmax.x) pmax.x = pos0[i].x;
        if (pos0[i].y < pmin.y) pmin.y = pos0[i].y;
        if (pos0[i].y > pmax.y) pmax.y = pos0[i].y;
    }
    Vec dim0, dim1 = (Vec){pmax.x - pmin.x, pmax.y - pmin.y};

    // First try going forward (single time steps)
    // p keeps track of the best coordinates, q is the next set
    Vec *p = pos0, *q = pos1, *temp;
    bool better;
    do {
        dim0 = dim1;
        pmin = (Vec){INT_MAX, INT_MAX};
        pmax = (Vec){INT_MIN, INT_MIN};
        for (i = 0; i < N; ++i) {
            q[i].x = p[i].x + vel[i].x;
            q[i].y = p[i].y + vel[i].y;
            if (q[i].x < pmin.x) pmin.x = q[i].x;
            if (q[i].x > pmax.x) pmax.x = q[i].x;
            if (q[i].y < pmin.y) pmin.y = q[i].y;
            if (q[i].y > pmax.y) pmax.y = q[i].y;
        }
        dim1 = (Vec){pmax.x - pmin.x, pmax.y - pmin.y};
        if ((better = dim1.x <= dim0.x && dim1.y <= dim0.y)) {
            t++;
            temp = p;
            p = q;
            q = temp;
        }
    } while (better);

    // Then try going backward (single time steps)
    // while remembering the previous minimal coordinates
    Vec pmin0;
    do {
        dim0 = dim1;
        pmin0 = pmin;
        pmin = (Vec){INT_MAX, INT_MAX};
        pmax = (Vec){INT_MIN, INT_MIN};
        for (i = 0; i < N; ++i) {
            q[i].x = p[i].x - vel[i].x;
            q[i].y = p[i].y - vel[i].y;
            if (q[i].x < pmin.x) pmin.x = q[i].x;
            if (q[i].x > pmax.x) pmax.x = q[i].x;
            if (q[i].y < pmin.y) pmin.y = q[i].y;
            if (q[i].y > pmax.y) pmax.y = q[i].y;
        }
        dim1 = (Vec){pmax.x - pmin.x, pmax.y - pmin.y};
        if ((better = dim1.x <= dim0.x && dim1.y <= dim0.y)) {
            t--;
            temp = p;
            p = q;
            q = temp;
        }
    } while (better);

    // Rebase to zero using the saved minimal coordinates
    // and sort by y,x coordinates
    for (i = 0; i < N; ++i) {
        p[i].x -= pmin0.x;
        p[i].y -= pmin0.y;
    }
    qsort(p, N, sizeof(Vec), cmp_pos);

    printf("Part 1:\n\n");
    i = 0;  // point index
    for (int y = 0; y <= dim0.y; ++y) {
        for (int x = 0; x <= dim0.x; ++x) {
            if (i < N && x == p[i].x && y == p[i].y) {
                printf("#");
                do {  // there can be multiple overlapping points
                    ++i;
                } while (i < N && x == p[i].x && y == p[i].y);
            } else {
                printf(" ");
            }
        }
        printf("\n");
    }
    printf("\nPart 2: %u\n", t);

    return 0;
}
