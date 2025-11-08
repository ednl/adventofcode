#include <stdio.h>
#include <stdlib.h>

static int dist(int q, int r)
{
    return (abs(q) + abs(q + r) + abs(r)) / 2;
}

int main(void)
{
    // Axial coordinates (q,r)
    // ref.: https://www.redblobgames.com/grids/hexagons/
    int q = 0, r = 0, d = 0, dmax = 0;
    FILE *f = fopen("../aocinput/2017-11-input.txt", "r");
    if (f != NULL) {
        int c, p = 0;
        while ((c = fgetc(f)) != EOF) {
            switch (c)
            {
                case 'n': --r; break;
                case 's': ++r; break;
                case 'w': --q; if (p == 'n') ++r; break;
                case 'e': ++q; if (p == 's') --r; break;
            }
            p = c;
            d = dist(q, r);
            if (d > dmax) {
                dmax = d;
            }
        }
        fclose(f);
    }
    printf("Part 1: %d\n", d);
    printf("Part 2: %d\n", dmax);
    return 0;
}
