#include <stdio.h>   // fopen, fclose, fscanf, printf
#include <stdint.h>  // uint32_t
#include <stdlib.h>  // qsort

// Number of lines in input file
#define N 945

typedef struct {
    uint32_t a, b;
} range_t;

static range_t block[N];

// Sort ascending
static int asc(const void *p, const void *q)
{
    const range_t *r1 = p;
    const range_t *r2 = q;
    if (r1->a < r2->a) return -1;
    if (r1->a > r2->a) return  1;
    if (r1->b < r2->b) return -1;
    if (r1->b > r2->b) return  1;
    return 0;
}

int main(void)
{
    FILE *f = fopen("../aocinput/2016-20-input.txt", "r");
    if (!f)
        return 1;
    size_t n = 0;
    uint32_t a, b;
    while (n < N && fscanf(f, "%u-%u", &a, &b) == 2)
        block[n++] = (range_t){a, b};
    fclose(f);

    // Combine segments
    qsort(block, n, sizeof *block, asc);
    size_t i = 0, j = 1;
    while (j < n) {
        while (j < n && block[i].b >= block[j].a - 1) {
            if (block[j].b > block[i].b)
                block[i].b = block[j].b;
            ++j;
        }
        if (j < n)
            block[++i] = block[j++];
    }
    n = i + 1;
    printf("Part 1: %u\n", block[0].a ? 0 : block[0].b + 1);  // 14975795

    // printf("  0: %10u %10u %u\n", block[0].a, block[0].b, block[0].a - block[n - 1].b - 1);
    // for (i = 1; i < n; ++i)
    //     printf("%3zu: %10u %10u %u\n", i, block[i].a, block[i].b, block[i].a - block[i - 1].b - 1);

    uint32_t allow = block[0].a - block[n - 1].b - 1;  // loop around, also works with UINT32_MAX
    for (i = 1; i < n; ++i)
        allow += block[i].a - block[i - 1].b - 1;  // gap to previous
    printf("Part 2: %u\n", allow);  // 101

    return 0;
}
