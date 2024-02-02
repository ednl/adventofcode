#include <stdio.h>

#define NAME "../aocinput/2023-14-input.txt"
#define N 100

typedef struct cache {
    __uint128_t bprev, bnext;
} Cache;

static __uint128_t rock_x[N], rock_y[N];  // '#' square rock barriers
static __uint128_t roll_x[N], roll_y[N];  // 'O' round rock boulders

static Cache cache[N][4][N];
static int clen[N];

int main(void)
{
    FILE* f = fopen(NAME, "r");
    if (!f) { fputs("File not found.\n", stderr); return 1; }

    // Read rock/roll rows.
    char buf[128];
    for (int i = 0; i < N && fgets(buf, sizeof buf, f); ++i) {
        __uint128_t s = 0, r = 0;  // square, round
        const char* end = buf + N;
        for (const char* c = buf; c != end; ++c) {
            s = s << 1 | (*c == '#');
            r = r << 1 | (*c == 'O');
        }
        rock_x[i] = s;  // bit pattern of square rocks per row (x)
        roll_x[i] = r;  // bit pattern of round  rocks per row (x)
    }
    fclose(f);

    // Construct rock/roll cols.
    for (int i = 0; i < N; ++i) {
        __uint128_t s = 0, r = 0;  // square, round
        for (int j = 0; j < N; ++j) {
            s = s << 1 | (rock_x[j] >> (N - i - 1) & 1);
            r = r << 1 | (roll_x[j] >> (N - i - 1) & 1);
        }
        rock_y[i] = s;  // bit pattern of square rocks per col (y)
        roll_y[i] = r;  // bit pattern of round  rocks per col (y)
    }

    puts("Identical rows:");
    for (int i = 0; i < N - 1; ++i)
        for (int j = i + 1; j < N; ++j)
            if (rock_x[i] == rock_x[j])
                printf("x: %d,%d\n", i, j);

    puts("Identical cols:");
    for (int i = 0; i < N - 1; ++i)
        for (int j = i + 1; j < N; ++j)
            if (rock_y[i] == rock_y[j])
                printf("y: %d,%d\n", i, j);

    puts("Identical row,col:");
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            if (rock_x[i] == rock_y[j])
                printf("xy: %d,%d\n", i, j);

    return 0;
}
