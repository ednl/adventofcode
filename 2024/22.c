#include <stdio.h>
#include <stdint.h>

#define EXAMPLE 0
#if EXAMPLE == 1
    #define FNAME "../aocinput/2024-22-example.txt"
    #define N 4
#else
    #define FNAME "../aocinput/2024-22-input.txt"
    #define N 2268
#endif

// Modulo 16777216 = 24 bit
#define M24 ((UINT32_C(1) << 24) - 1)

static uint32_t xorshift(uint32_t x)
{
    x ^= x << 6;     // mul by 64, mix
    x &= M24;        // prune
    x ^= x >> 5;     // div by 32, mix
    x ^= x << 11;    // (prune not needed) mul by 2048, mix
    return x & M24;  // pruned
}

int main(void)
{
    FILE *f = fopen(FNAME, "r");
    if (!f) { fprintf(stderr, "File not found: %s\n", FNAME); return 1; }

    uint64_t sum = 0;
    uint32_t x, a, b, c, d;
    while (fscanf(f, "%u ", &x) == 1) {
        for (int i = 0; i < 500; ++i) {
            a = xorshift(x);
            b = xorshift(a);
            c = xorshift(b);
            d = xorshift(c);
            //part 2 goes here
            x = d;
        }
        sum += x;
    }
    fclose(f);
    printf("%lld\n", sum);  // example: 37327623, input: 19150344884

    return 0;
}
