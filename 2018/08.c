#include <stdio.h>

#define N 15000U

static unsigned char licence[N] = {0};

static unsigned int readlicence(void)
{
    FILE *f = fopen("../aocinput/2018-08-input.txt", "r");
    if (!f)
        return 0;

    unsigned int n = 0;
    unsigned char c;
    while (n < N && fscanf(f, "%hhu ", &c) == 1) {
        licence[n++] = c;
    }
    fclose(f);
    return n;
}

static unsigned int sumdata(void)
{
    static unsigned int i = 0;
    unsigned int sum = 0;
    unsigned char childcount = licence[i++];
    unsigned char datacount = licence[i++];
    while (childcount--)
        sum += sumdata();
    while (datacount--)
        sum += licence[i++];
    return sum;
}

static unsigned int sumnode(void)
{
    static unsigned int i = 0;
    unsigned int sum = 0;
    unsigned char childcount = licence[i++];
    unsigned char datacount = licence[i++];
    if (childcount == 0) {
        while (datacount--) {
            sum += licence[i++];
        }
    } else {
        unsigned int child[12];
        for (unsigned int j = 0; j < childcount; ++j) {
            child[j] = sumnode();
        }
        while (datacount--) {
            unsigned int k = licence[i++];
            if (k >= 1 && k <= childcount) {
                sum += child[k - 1];
            }
        }
    }
    return sum;
}

int main(void)
{
    readlicence();
    printf("Part 1: %u\n", sumdata());
    printf("Part 2: %u\n", sumnode());
    return 0;
}
