#include <stdio.h>
#include <stdint.h>  // uint64_t
#include <inttypes.h>  // PRIu64
#include <stdbool.h>

#define N 1000
#define P 25

static uint64_t data[N];

static bool valid(const int k)
{
    for (int i = k - P; i < k - 1; ++i)
        for (int j = i + 1; j < k; ++j)
            if (data[i] + data[j] == data[k])
                return true;
    return false;
}

int main(void)
{
    FILE *f = fopen("../aocinput/2020-09-input.txt", "r");
    if (!f)
        return 1;
    for (int i = 0; i < N; ++i)
        fscanf(f, "%"PRIu64, &data[i]);
    fclose(f);

    int k = P;
    while (valid(k))
        ++k;
    printf("Part 1: %"PRIu64"\n", data[k]);  // 15353384

    int i = 0, j = 0;
    uint64_t sum = data[0];
    while (sum != data[k]) {
        while (sum < data[k])
            sum += data[++j];
        if (sum == data[k])
            break;
        while (sum > data[k])
            sum -= data[i++];
    }
    uint64_t min = data[i], max = data[i];
    for (++i; i <= j; ++i) {
        if (data[i] < min) min = data[i];
        if (data[i] > max) max = data[i];
    }
    printf("Part 2: %"PRIu64"\n", min + max);  // 2466556
    return 0;
}
