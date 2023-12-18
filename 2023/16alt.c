#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define N 110
#define LIM (N + 1)  // max x/y value
#define DIM (N + 2)  // +border
#define ROW (DIM + 2)  // +2='\n\0'
#define SSIZE 80

typedef enum dir {
    NONE=0, HORZ, VERT
} Dir;

typedef struct vec {
    int x, y;
} Vec;

typedef struct beam {
    Vec pos;
    int step;  // horz: +/-1, vert: +/-(N+2)
    Dir dir;   // 1=horz, 2=vert
} Beam;

static char mirror[DIM][ROW];
static int seen[DIM][DIM];  // HORZ | VERT
static Beam stack[SSIZE];
static int slen, energized;

static bool pop(Beam* const beam)
{
    if (!slen) return false;
    *beam = stack[--slen];
    return true;
}

static bool push(const Beam beam)
{
    if (slen == SSIZE) { printf("!S"); return false; }
    stack[slen++] = beam;
    return true;
}

static void propagate(const Beam beam)
{
    const char* m = &mirror[beam.pos.y][beam.pos.x];

}

int main(void)
{
    FILE* f = fopen("../aocinput/2023-16-input.txt", "r");
    for (int i = 0; i < N; ++i)
        fgets(&mirror[i + 1][1], N + 2, f);
    fclose(f);
    memset(mirror[0], '#', N + 2);
    memset(mirror[N + 1], '#', N + 2);
    for (int i = 0; i < N + 2; ++i) {
        mirror[i][0] = '#';
        mirror[i][N + 1] = '#';
        mirror[i][N + 2] = '\n';
        mirror[i][N + 3] = '\0';
    }

    for (int i = 0; i < N + 2; ++i)
        printf("%s", mirror[i]);

    return 0;
}
