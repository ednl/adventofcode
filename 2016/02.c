#include <stdio.h>

static const int pad1[3][3] = {{1,4,7},{2,5,8},{3,6,9}};
static const int pad2[5][5] = {{0,0,5,0,0},{0,2,6,10,0},{1,3,7,11,13},{0,4,8,12,0},{0,0,9,0,0}};

int main(void)
{
    FILE *f = fopen("../aocinput/2016-02-input.txt", "r");
    if (!f)
        return 1;
    int ch, b1 = 0, b2 = 0, x1 = 1, y1 = 1, x2 = 0, y2 = 2;
    while ((ch = fgetc(f)) > 0) {
        switch (ch) {
            case 'U':
                if (y1 > 0)
                    --y1;
                if (y2 > 0 && pad2[x2][y2 - 1])
                    --y2;
                break;
            case 'D':
                if (y1 < 2)
                    ++y1;
                if (y2 < 4 && pad2[x2][y2 + 1])
                    ++y2;
                break;
            case 'L':
                if (x1 > 0)
                    --x1;
                if (x2 > 0 && pad2[x2 - 1][y2])
                    --x2;
                break;
            case 'R':
                if (x1 < 2)
                    ++x1;
                if (x2 < 4 && pad2[x2 + 1][y2])
                    ++x2;
                break;
            case 10:
                b1 = b1 * 10 + pad1[x1][y1];
                b2 = b2 * 16 + pad2[x2][y2];
                break;
        }
    }
    fclose(f);
    printf("Part 1: %d\n", b1);
    printf("Part 2: %X\n", b2);
    return 0;
}
