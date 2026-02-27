#include <stdio.h>

#define FNAME "../aocinput/2015-24-input.txt"
#define N 32

static int weight[N];

int main(void)
{
    FILE *f = fopen(FNAME, "r");
    if (!f) return 1;
    int count = 0, sum = 0;
    for (int x; count < N && fscanf(f, "%d", &x) == 1;) {
        weight[count++] = x;
        sum += x;
    }
    fclose(f);
    int target = sum / 3;
    int maxlen = count / 3;
    int minlen = 0;
    for (int i = count, w = 0; i > 0 && w < target; ++minlen)
        w += weight[--i];

    printf("%d %d %d %d %d\n", count, sum, target, minlen, maxlen);
}
