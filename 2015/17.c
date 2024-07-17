#include <stdio.h>
#include <stdlib.h>  // qsort
#include "../combinations.h"

#define N 20        // number of lines in input file
#define EGGNOG 150  // from puzzle description

static int container[N];

static int cmpintasc(const void *p, const void *q)
{
    const int a = *(int *)p;
    const int b = *(int *)q;
    if (a < b) return -1;
    if (a > b) return 1;
    return 0;
}

int main(void)
{
    // Read input file
    FILE *f = fopen("../aocinput/2015-17-input.txt", "r");
    if (!f) return 1;
    int n = 0;
    while (n < N && fscanf(f, "%d", &container[n]))
        ++n;
    fclose(f);
    if (n != N) return 2;

    // Sort containers by size (ascending)
    qsort(container, N, sizeof *container, cmpintasc);

    // Determine minimum and maximum amount of containers to take
    int mintake = 1, maxtake = 0;
    int sumsmall = 0, sumlarge = 0;
    for (int i = 0; i < N; ++i) {
        sumsmall += container[i];  // add small container sizes
        if (sumsmall <= EGGNOG)
            maxtake++;
        sumlarge += container[N - 1 - i];  // add large container sizes
        if (sumlarge < EGGNOG)
            mintake++;
    }

    int part1 = 0, part2 = 0, minmatch = 0;
    for (int k = mintake; k <= maxtake; ++k) {
        int *index = NULL;
        while ((index = combinations(N, k))) {
            int i = 0, sum = 0;
            while (i < k && sum < EGGNOG)
                sum += container[index[i++]];
            if (i == k && sum == EGGNOG) {
                ++part1;  // any combination that works
                if (!minmatch) {
                    minmatch = k;
                    ++part2;
                } else if (k == minmatch)
                    ++part2;  // working combinations with minimum amount
            }
        }
    }
    printf("%d %d\n", part1, part2);  // 1304 18
    return 0;
}
