#include <stdio.h>

#define N   200
#define SUM 2020

static int data[N];

static int find2(const int n)
{
    for (int i = 0; i < n - 1; ++i)
        for (int j = i + 1; j < n; ++j)
            if (data[i] + data[j] == SUM)
                return data[i] * data[j];
    return 0;
}

static int find3(const int n)
{
    for (int i = 0; i < n - 2; ++i)
        for (int j = i + 1; j < n - 1; ++j) {
            const int part = data[i] + data[j];
            if (part >= SUM)
                continue;
            for (int k = j + 1; k < n; ++k)
                if (part + data[k] == SUM)
                    return data[i] * data[j] * data[k];
        }
    return 0;
}

int main(void)
{
    FILE *f = fopen("../aocinput/2020-01-input.txt", "r");
    if (!f)
        return 1;
    int n = 0;
    while (n < N && fscanf(f, "%d", &data[n]) == 1)
        ++n;
    fclose(f);
    printf("Part 1: %d\n", find2(n));  // 802011
    printf("Part 2: %d\n", find3(n));  // 248607374
    return 0;
}
