#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define EXAMPLE 1
#if EXAMPLE
#define NAME "../aocinput/2023-22-example.txt"
#define N 7
#else
#define NAME "../aocinput/2023-22-input.txt"
#define N 1250
#endif

static uint16_t brick[N][6];

static int z_asc(const void* p, const void* q)
{
    const uint16_t* a = (const uint16_t*)p;
    const uint16_t* b = (const uint16_t*)q;
    if (a[2] < b[2]) return -1;
    if (a[2] > b[2]) return  1;
    if (a[5] < b[5]) return -1;
    if (a[5] > b[5]) return  1;
    return 0;
}

int main(void)
{
    FILE* f = fopen(NAME, "r");
    if (!f)
        return 1;
    char buf[32];
    for (int i = 0; i < N && fgets(buf, sizeof buf, f); ++i) {
        char* s = buf;
        for (int j = 0; j < 6; ++j, ++s) {
            uint16_t x = 0;
            while (*s >= '0' && *s <= '9')
                x = x * 10 + (*s++ & 15);
            brick[i][j] = x;
        }
    }
    fclose(f);

    qsort(brick, N, sizeof *brick, z_asc);
    for (int i = 0; i < N; ++i)
        printf("%u,%u,%u~%u,%u,%u\n", brick[i][0], brick[i][1], brick[i][2], brick[i][3], brick[i][4], brick[i][5]);

    return 0;
}
