#include <stdio.h>
#include <stdlib.h>  // abs

#define WIRES 2
#define MAXSEG 500

typedef enum dir {
    HORZ, VERT
} Dir;

// len = total wire length from start to here
// hor = wire segment is horizontal: 0=no, 1=yes
typedef struct seg {
    Dir dir;
    int x, y, len;
} Seg;

typedef struct intersect {
    int dist, time;
} Intersect;

static Seg wire[WIRES][MAXSEG];
static int count[WIRES];

static Intersect intersect(int i, int j)
{
    if (i <= 0 || j <= 0 || i >= count[0] || j >= count[1] || wire[0][i].dir == wire[1][j]. dir)
        return (Intersect){-1, -1};
    const Seg *h, *v;
    if (wire[0][i].dir == HORZ) {
        h = &wire[0][i];
        v = &wire[1][j];
    } else {
        v = &wire[0][i];
        h = &wire[1][j];
    }
    if ((((h - 1)->x < v->x && v->x <= h->x) || (h->x <= v->x && v->x < (h - 1)->x)) &&
        (((v - 1)->y < h->y && h->y <= v->y) || (v->y <= h->y && h->y < (v - 1)->y)))
        return (Intersect){abs(v->x) + abs(h->y), h->len + v->len - abs(h->x - v->x) - abs(v->y - h->y)};
    return (Intersect){-1, -1};
}

int main(void)
{
    FILE *f = fopen("../aocinput/2019-03-input.txt", "r");
    if (!f) { fprintf(stderr, "File not found.\n"); return 1; }
    for (int i = 0; i < WIRES; ++i) {
        for (int j = 1, c, len = 0; j < MAXSEG; ) {
            //
        }
        nexti:;
    }
    fclose(f);

    printf("%d %d\n", count[0], count[1]);

    return 0;
}
