#include <stdio.h>

#define N 2000
static int depth[N] = {0};

static int inc(int win)
{
    int n = 0;
    for (int i = win; i < N; ++i)
        n += depth[i] > depth[i - win];
    return n;
}

int main(void)
{
    FILE *f = fopen("../aocinput/2021-01-input.txt", "r");
    if (!f)
        return 1;

    int i = 0;
    while (i < N && fscanf(f, "%d ", &depth[i]) == 1)
        ++i;
    fclose(f);
    if (i != N)
        return 2;

    printf("Part 1: %d\n", inc(1));
    printf("Part 2: %d\n", inc(3));
    return 0;
}
