#include <stdio.h>

#define FNAME "../aocinput/2024-06-input.txt"
#define N       130
#define COLS    (N + 1)  // +newline
#define FSIZE   (N * COLS)
#define END     ((N + 1) * COLS)
#define MAPSIZE ((N + 2) * COLS)

typedef enum dir { U, R, D, L } Dir;

static char map[MAPSIZE];
static int jump[MAPSIZE][4];

static Dir turn(const Dir dir)
{
    return (dir + 1) & 3;
}

static int start(void)
{
    for (int i = COLS; i < END; ++i)
        if (map[i] == '^')
            return i;
    return 0;
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    fread(&map[COLS], FSIZE, 1, f);
    fclose(f);
    printf("%d\n", start());
}
