#include <stdio.h>
#include <stdbool.h>

static bool triangle(unsigned int a, unsigned int b, unsigned int c)
{
    return (a + b > c) && (a + c > b) && (b + c > a);
}

int main(void)
{
    FILE *f = fopen("../aocinput/2016-03-input.txt", "r");
    if (!f)
        return 1;

    unsigned int a[9], n = 0, m = 0;
    while (fscanf(f, " %u %u %u %u %u %u %u %u %u", a, a+1, a+2, a+3, a+4, a+5, a+6, a+7, a+8) == 9)
    {
        n += triangle(a[0], a[1], a[2]);
        n += triangle(a[3], a[4], a[5]);
        n += triangle(a[6], a[7], a[8]);

        m += triangle(a[0], a[3], a[6]);
        m += triangle(a[1], a[4], a[7]);
        m += triangle(a[2], a[5], a[8]);
    }
    fclose(f);

    printf("Part 1: %u\nPart 2: %u\n", n, m);
    return 0;
}
