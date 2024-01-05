#include <stdio.h>

#define NAME "../aocinput/2023-14-input.txt"
#define N 100

typedef struct cache {
    __uint128_t bprev, bnext;
} Cache;

static __uint128_t rock_x[N], rock_y[N];  // '#' barriers
static __uint128_t ball_x[N], ball_y[N];  // 'O' rolling around

static Cache cache[N][4][N];
static int clen[N];

int main(void)
{
    FILE* f = fopen(NAME, "r");
    if (!f) { fputs("File not found.\n", stderr); return 1; }

    char buf[128];
    for (int i = 0; i < N && fgets(buf, sizeof buf, f); ++i) {
        __uint128_t r = 0, b = 0;
        const char* end = buf + N;
        for (const char* s = buf; s != end; ++s) {
            r = r << 1 | (*s == '#');
            b = b << 1 | (*s == 'O');
        }
        rock_x[i] = r;
        ball_x[i] = b;
    }
    fclose(f);

    for (int i = 0; i < N; ++i) {
        __uint128_t r = 0, b = 0;
        for (int j = 0; j < N; ++j) {
            r = r << 1 | (rock_x[j] >> (N - i - 1) & 1);
            b = b << 1 | (ball_x[j] >> (N - i - 1) & 1);
        }
        rock_y[i] = r;
        ball_y[i] = b;
    }

    for (int i = 0; i < N - 1; ++i)
        for (int j = i + 1; j < N; ++j)
            if (rock_x[i] == rock_x[j])
                printf("x: %d,%d\n", i, j);

    for (int i = 0; i < N - 1; ++i)
        for (int j = i + 1; j < N; ++j)
            if (rock_y[i] == rock_y[j])
                printf("y: %d,%d\n", i, j);

    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            if (rock_x[i] == rock_y[j])
                printf("xy: %d,%d\n", i, j);

    return 0;
}
