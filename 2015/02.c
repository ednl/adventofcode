#include <stdio.h>

static unsigned min(const unsigned a, const unsigned b)
{
    return a < b ? a : b;
}

int main(void)
{
    FILE *f = fopen("../aocinput/2015-02-input.txt", "r");
    if (!f) return 1;
    unsigned l, w, h, paper = 0, ribbon = 0;
    while (fscanf(f, "%ux%ux%u", &l, &w, &h) == 3) {
        const unsigned a = l * w, b = l * h, c = w * h;
        paper += ((a + b + c) << 1) + min(min(a, b), c);
        const unsigned p = l + w, q = l + h, r = w + h;
        ribbon += (min(min(p, q), r) << 1) + l * w * h;
    }
    fclose(f);
    printf("Part 1: %u\n", paper);   // 1598415
    printf("Part 2: %u\n", ribbon);  // 3812909
    return 0;
}
