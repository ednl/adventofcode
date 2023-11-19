#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// static const char *input = "../aocinput/2021-20-example.txt";  // DIM 5
static const char *input = "../aocinput/2021-20-input.txt";  // DIM 100
#define DIM  100
#define SIZE 256
#define BITS 512

typedef struct {
    int dr, dc;
} Pos;
static const Pos kernel[9] = {
    {-1,-1},{-1,0},{-1,1},
    { 0,-1},{ 0,0},{ 0,1},
    { 1,-1},{ 1,0},{ 1,1},
};

static bool filter[BITS] = {0};
static bool img[2][SIZE][SIZE] = {0};  // current and next image
static int cur = 0, zero = (SIZE - DIM) >> 1, lim = (SIZE + DIM) >> 1;

static void read(void)
{
    FILE *f = fopen(input, "r");
    for (int i = 0; i < BITS; ++i)
        filter[i] = fgetc(f) == '#';
    fgetc(f); fgetc(f);
    for (int i = zero; i < lim; ++i) {
        for (int j = zero; j < lim; ++j)
            img[0][i][j] = fgetc(f) == '#';
        fgetc(f);  // newline
    }
    fclose(f);
}

static void evolve(int nextgen)
{
    static int gen = 0;
    while (gen < nextgen) {
        ++gen;
        const int next = gen & 1;
        const int zero2 = zero - 1;
        const int lim2 = lim + 1;
        const bool inf = cur;
        for (int i = zero2; i < lim2; ++i) {  // rows in next image
            for (int j = zero2; j < lim2; ++j) {  // cols in next image
                int index = 0;
                for (int k = 0; k < 9; ++k) {
                    index <<= 1;
                    int r = i + kernel[k].dr;  // row in cur image
                    int c = j + kernel[k].dc;  // col in cur image
                    index |= r >= zero && r < lim && c >= zero && c < lim ? img[cur][r][c] : inf;
                }
                img[next][i][j] = filter[index];
            }
        }
        cur = next;
        zero = zero2;
        lim = lim2;
    }
}

static int lit(void)
{
    int sum = 0;
    for (int i = zero; i < lim; ++i)
        for (int j = zero; j < lim; ++j)
            sum += img[cur][i][j];
    return sum;
}

int main(void)
{
    read();
    evolve(2);
    printf("Part 1: %d\n", lit());  // 5229
    evolve(50);
    printf("Part 2: %d\n", lit());  // 17009
    return 0;
}
