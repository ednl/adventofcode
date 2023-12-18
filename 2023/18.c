#include <stdio.h>
#include <stdint.h>

#define EXAMPLE 1
#if EXAMPLE
#define NAME "../aocinput/2023-18-example.txt"
#define DIG 14  // number of trench digging instructions in example
#else
#define NAME "../aocinput/2023-18-input.txt"
#define DIG 766  // number of trench digging instructions in input
#endif

typedef enum dir {
    RIGHT, DOWN, LEFT, UP
} Dir;

typedef struct vec {
    int x, y;
} Vec;

typedef struct dig {
    Dir dir;
    int64_t len;
} Dig;

static const char dir2char[] = "RDLU";
static Dig trench[2][DIG];  // two different trenches for parts 1 & 2

static Dir char2dir(const char c)
{
    switch (c) {
        case 'R': return RIGHT;
        case 'D': return DOWN;
        case 'L': return LEFT;
        case 'U': return UP;
    }
    return 0;
}

int main(void)
{
    FILE* f = fopen(NAME, "r");
    char c;
    unsigned len, rgb;
    for (int i = 0; i < DIG && fscanf(f, " %c %2u (#%6x)", &c, &len, &rgb) == 3; ++i) {
        trench[0][i] = (Dig){char2dir(c), (int64_t)len};
        trench[1][i] = (Dig){rgb & 3, (int64_t)(rgb >> 4)};
    }
    fclose(f);

    #if EXAMPLE || defined(DEBUG)
    for (int i = 0; i < DIG; ++i) {
        printf("%c %lld | %c %6lld\n", dir2char[trench[0][i].dir], trench[0][i].len, dir2char[trench[1][i].dir], trench[1][i].len);
    }
    printf("\n");
    #endif

    for (int i = 0; i < 2; ++i) {  // parts 1 & 2
        int64_t x = 0, y = 0, a0 = 0, a1 = 0;  // pos, area
        for (int j = 0; j < DIG; ++j) {
            const Dig* t = &trench[i][j];
            switch (t->dir) {
                case RIGHT: x += t->len; a0 += t->len; break;
                case DOWN : y += t->len; a0 += t->len; a1 += t->len * x; break;
                case LEFT : x -= t->len; a0 += t->len; break;
                case UP   : y -= t->len; a0 += t->len; a1 -= t->len * (x + 1); break;
            }
        }
        printf("Part %d: %lld + %lld = %lld\n", i + 1, a0, a1, a0 + a1);  // example: (62,952408144115), input: (46334,?)
    }
    return 0;
}
