///////////////////////////////////////////////////////////////////////////////
////
////  Advent of Code 2019
////  Day 8: Space Image Format, part two
////
////  E. Dronkert
////  https://github.com/ednl/aoc2019
////
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>

#define W 25
#define H 6
#define P (W * H)

int main(void)
{
    FILE *fp;
    int i, c, k, bit[P];

    for (i = 0; i < P; ++i)
        bit[i] = 2;

    if ((fp = fopen("../aocinput/2019-08-input.txt", "r")) != NULL)
    {
        i = 0;
        while ((c = fgetc(fp)) != EOF)
            if ((k = c - '0') >= 0 && k < 3)
            {
                if (k != 2 && bit[i] == 2)
                    bit[i] = k;
                if (++i == P)
                    i = 0;
            }
        i = 0;
        while (i < P)
        {
            printf("%c", bit[i++] == 1 ? '*' : ' ');
            if (!(i % W))
                printf("\n");
        }
    }
    return 0;
}
