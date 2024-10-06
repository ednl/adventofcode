#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define N 110
#define LIM (N + 1)  // max x/y value (min=1)
#define DIM (N + 2)  // +border
#define ROW (DIM + 2)  // +2 for '\n\0'
#define SSIZE 80

typedef enum ori {
    NONE=0, VERT, HORZ
} Ori;

typedef enum dir {
    NODIR=0, UP, DOWN, LEFT, RIGHT
} Dir;

typedef struct vec {
    int x, y;
} Vec;

typedef struct ray {
    Vec pos, dir;
} Ray;

static const char *dir2char = "_UDLR";
static const Vec delta[5] = {{0,0},{0,-1},{0,1},{-1,0},{1,0}};  // _,U,D,L,R

static char grid[DIM][ROW];
static Ori seen[DIM][DIM];
static Ray stack[SSIZE];
static int slen, energized;

static Vec turnleft(const Vec dir)
{
    return (Vec){-dir.y, dir.x};
}

static Vec turnright(const Vec dir)
{
    return (Vec){dir.y, -dir.x};
}

static bool pop(Ray *const ray)
{
    if (!slen) return false;
    *ray = stack[--slen];
    return true;
}

static bool push(const Ray ray)
{
    if (slen == SSIZE) { printf("!S"); return false; }
    stack[slen++] = ray;
    return true;
}

static void trace(const Ray ray)
{
}

int main(void)
{
    FILE *f = fopen("../aocinput/2023-16-input.txt", "r");
    if (!f) { fputs("File not found.\n", stderr); return 1; }

    for (int i = 0; i < N; ++i)
        fgets(&grid[i + 1][1], N + 2, f);
    fclose(f);
    memset(grid[0], '#', N + 2);
    memset(grid[N + 1], '#', N + 2);
    for (int i = 0; i < N + 2; ++i) {
        grid[i][0] = '#';
        grid[i][N + 1] = '#';
        grid[i][N + 2] = '\n';
        grid[i][N + 3] = '\0';
    }

    for (int i = 0; i < N + 2; ++i)
        printf("%s", grid[i]);

    return 0;
}
