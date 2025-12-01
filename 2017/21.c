// (0)
// (1,2,4,8)
// (3,5,10,12)
// (6,9)
// (7,11,13)
// (15)

// (0)
// (1,4,64,256)
// (2,8,32,128)
// (3,6,9,36,72,192,288,384)
// (5,65,260,320)
// (10,34,136,160)
// (12,33,66,96,129,132,258,264)
// (16)
// (17,20,80,272)
// (18,24,48,144)
// (40,130)
// (68,257)
// (511)

// 000 = 000 = 000 = 000 = 000 = 000 = 000 = 000
// 000   000   000   000   000   000   000   000
// 000   000   000   000   000   000   000   000


// 012 = 258 = 876 = 630 = 210 = 036 = 678 = 852
// 345   147   543   741   543   147   345   741
// 678   036   210   852   876   258   012   630


// (3)x1 => (2)x4 => (2)x9 =>
// (3)x9 => (2)x36 =>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// static int rule2[1 << 4];
// static int rule3[1 << 9];
static bool m2[2][2];
static bool m3[3][3];

static void print(const int size, const void *org)
{
    const bool (*src)[size][size] = org;
    printf("[");
    for (int i = 0; i < size; ++i) {
        if (i > 0)
            printf(" ");
        printf("[");
        for (int j = 0; j < size; ++j) {
            printf("%d", (*src)[i][j]);
            if (j < size - 1)
                printf(" ");
        }
        printf("]");
        if (i < size - 1)
            printf("\n");
    }
    printf("]\n");

}

// 0123 => 0213
// 012345678 => 036147258
static void transpose(const int size, void *org)
{
    const bool (*src)[size][size] = org;
    const int bytes = sizeof (bool) * size * size;
    bool (*tmp)[size][size] = malloc(bytes);
    for (int i = 0; i < size - 1; ++i)
        for (int j = i + 1; j < size; ++j) {
            (*tmp)[j][i] = (*src)[i][j];
            (*tmp)[i][j] = (*src)[j][i];
        }
    memcpy(org, tmp, bytes);
    free(tmp);
}

int main(void)
{
    // FILE *f = fopen("../aocopen/2017-21-input.txt", "r");
    // if (!f) return 1;
    // char buf[BUFSIZ], *c;
    // for (int i = 0; i < 6 && (c = fgets(buf, sizeof buf, f)); ++i) {
    //     // ##/## => ###/##./.#.
    //     // .../.../... => .#../..##/...#/....

    //     int x = 0;
    //     for (int j = 0; j < 2; ++j)
    //         for (int k = 0; k < 2; ++k)
    //             x = x << 1 | (*c == '#');
    // }
    // fclose(f);

    print(2, m2);
    transpose(2, m2);
    print(2, m2);
    printf("\n");
    print(3, m3);
    transpose(3, m3);
    print(3, m3);
}
