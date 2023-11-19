#include <stdio.h>
#include <stdint.h>

static int duel1(uint64_t a, uint64_t b)
{
    int n = 40000000, k = 0;
    while (n--) {
        a = a * 16807ULL % 2147483647ULL;
        b = b * 48271ULL % 2147483647ULL;
        k += (a & 0xffff) == (b & 0xffff);
    }
    return k;
}

static int duel2(uint64_t a, uint64_t b)
{
    int n = 5000000, k = 0;
    while (n--) {
        do {
            a = a * 16807ULL % 2147483647ULL;
        } while (a & 0x3);
        do {
            b = b * 48271ULL % 2147483647ULL;
        } while (b & 0x7);
        k += (a & 0xffff) == (b & 0xffff);
    }
    return k;
}

int main(void)
{
    printf("Part 1: %i\n", duel1(783, 325));
    printf("Part 2: %i\n", duel2(783, 325));
    return 0;
}
