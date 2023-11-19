#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// static const char *input = "../aocinput/2021-20-example.txt";  // DIM 5
static const char *input = "../aocinput/2021-20-input.txt";  // DIM 100
#define DIM  100
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
static bool *img[2] = {0};  // current and next image
static int cur = 0, dim = DIM;

static void read(void)
{
    img[0] = malloc((size_t)(DIM * DIM) * sizeof(bool));
    FILE *f = fopen(input, "r");
    for (int i = 0; i < BITS; ++i)
        filter[i] = fgetc(f) == '#';
    fgetc(f); fgetc(f);
    for (int i = 0; i < DIM; ++i) {
        int ii = i * DIM;
        for (int j = 0; j < DIM; ++j)
            img[0][ii + j] = fgetc(f) == '#';
        fgetc(f);
    }
    fclose(f);
}

static void evolve(int nextgen)
{
    static int gen = 0;
    while (gen < nextgen) {
        const int next = 1 - cur;
        const int dim2 = dim + 2;
        // Infinite border always starts at 0 then depends on 2 bits filter[0],filter[511]
        // 0X: forever 0
        // 11: 0 once, then 1
        // 10: flips 0-1-0-1-etc.
        const bool inf = filter[0] ? (filter[511] ? gen : cur) : 0;
        img[next] = realloc(img[next], (size_t)(dim2 * dim2) * sizeof(bool));
        for (int i = 0; i < dim2; ++i) {  // rows in next image
            int ii = i * dim2;
            for (int j = 0; j < dim2; ++j) {  // cols in next image
                int index = 0;
                for (int k = 0; k < 9; ++k) {
                    index <<= 1;
                    int r = i - 1 + kernel[k].dr;  // row in cur image
                    int c = j - 1 + kernel[k].dc;  // col in cur image
                    index |= r >= 0 && r < dim && c >= 0 && c < dim ? img[cur][r * dim + c] : inf;
                }
                img[next][ii + j] = filter[index];
            }
        }
        cur = next;
        dim = dim2;
        ++gen;
    }
}

static int lit(void)
{
    const int count = dim * dim;
    int sum = 0;
    for (int i = 0; i < count; ++i)
        sum += img[cur][i];
    return sum;
}

int main(void)
{
    read();

    evolve(2);
    printf("Part 1: %d\n", lit());

    evolve(50);
    printf("Part 2: %d\n", lit());

    free(img[0]);
    free(img[1]);
    return 0;
}
