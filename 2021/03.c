#include <stdio.h>
#include <stdbool.h>

#define N 1000  // lines in input file
#define W 12    // word size (12-bit numbers)
#define S "12"  // word length in format string

typedef struct Data {
    char word[W + 1];
    bool selected;
} Data;
static Data data[N] = {0};

// What's the most or least common bit at index 'pos'?
// most==true : choose the most  common bit, if equal prefer 1
// most==false: choose the least common bit, if equal prefer 0
static char common(int pos, bool most)
{
    int remaining = 0, ones = 0;
    for (int i = 0; i < N; ++i)
        if (data[i].selected) {  // important for part 2!
            ++remaining;
            ones += data[i].word[pos] == '1';
        }
    int zeros = remaining - ones;
    return ones >= zeros ? '0' + most : '1' - most;
}

static int gamma(void)
{
    int g = 0;
    for (int i = 0; i < W; ++i)
        g = (g << 1) | (common(i, true) == '1');
    return g;
}

// Value of selected bit string
static int selectedval(void)
{
    int i = 0, val = 0;
    while (i < N && !data[i].selected)
        ++i;
    if (i < N)
        for (int j = 0; j < W; ++j)
            val = (val << 1) | (data[i].word[j] == '1');
    return val;
}

// Reduce to 1 item where each bit is most or least common
static int reduce(bool most)
{
    for (int i = 0; i < N; ++i)
        data[i].selected = true;  // reset selection
    int remaining = N, pos = 0;
    while (remaining > 1 && pos < W) {
        char c = common(pos, most);
        for (int i = 0; i < N; ++i)
            if (data[i].selected && data[i].word[pos] != c) {
                data[i].selected = false;
                if (--remaining == 1)
                    return selectedval();
            }
        ++pos;
    }
    return 0;
}

int main(void)
{
    FILE *f = fopen("../aocinput/2021-03-input.txt", "r");
    for (int i = 0; i < N && fscanf(f, "%"S"s ", data[i].word) == 1; ++i)
        data[i].selected = true;
    fclose(f);

    int gam = gamma(), eps = gam ^ ((1<<W) - 1);  // eps = inverted gam
    int oxy = reduce(true), co2 = reduce(false);

    printf("Part 1: %d\n", gam * eps);  // 4147524
    printf("Part 2: %d\n", oxy * co2);  // 3570354
    return 0;
}
