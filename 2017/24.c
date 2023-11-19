#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>

#define N (57)  // number of lines in the input file
typedef struct comp {
    int a, b, c;
    bool used;
} Comp;

static Comp comp[N] = {0};
static int maxsum = 0;
static int maxlen = 0;
static int maxsum2 = 0;

static void load(void)
{
    FILE *f = fopen("../aocinput/2017-24-input.txt", "r");
    if (f != NULL) {
        int i = 0, a, b;
        while (i < N && fscanf(f, "%d/%d ", &a, &b) == 2) {
            comp[i++] = (Comp){ a, b, a + b, false };
        }
        fclose(f);
    }
}

static void bridge(int port, int len, int sum)
{
    if (sum > maxsum) {
        maxsum = sum;
    }
    if (len > maxlen) {
        maxlen = len;
        maxsum2 = sum;
    } else if (len == maxlen && sum > maxsum2) {
        maxsum2 = sum;
    }
    for (int i = 0; i < N; ++i) {
        if (!comp[i].used && (port == comp[i].a || port == comp[i].b)) {
            comp[i].used = true;
            bridge(port == comp[i].a ? comp[i].b : comp[i].a, len + 1, sum + comp[i].c);
            comp[i].used = false;
        }
    }
}

int main(void)
{
    load();
    bridge(0, 0, 0);
    printf("Part 1: %d\n", maxsum);   // right answer: 1868
    printf("Part 2: %d\n", maxsum2);  // right answer: 1841
    return 0;
}
