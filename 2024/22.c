#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>  // qsort
#include <string.h>  // memset
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define EXAMPLE 0
#if EXAMPLE == 1
    #define FNAME "../aocinput/2024-22-example1.txt"
    #define N 4
#elif EXAMPLE == 2
    #define FNAME "../aocinput/2024-22-example2.txt"
    #define N 4
#elif EXAMPLE == 3
    #define FNAME "22.txt"
    #define N 1
#else
    #define FNAME "../aocinput/2024-22-input.txt"
    #define N 2268
#endif

#define LEN 2000  // series of consecutive secret numbers generated
#define M24 ((UINT32_C(1) << 24) - 1)  // modulo 16777216 = 24 bit

static uint8_t dif[LEN];  // range 0..18
static uint8_t  seq[20][20][20][20];  // index 0..18 = dif -9..+9, value 1..10
static uint16_t uni[20][20][20][20];  // index 0..18 = dif -9..+9, value (0..9)*X

static int desc(const void *p, const void *q)
{
    const uint16_t a = *(uint16_t *)p;
    const uint16_t b = *(uint16_t *)q;
    if (a < b) return 1;
    if (a > b) return -1;
    return 0;
}

static uint32_t xorshift(uint32_t x)
{
    x ^= x << 6;     // mul by 64, mix
    x &= M24;        // prune
    x ^= x >> 5;     // div by 32, mix
    // x &= M24;        // prune
    x ^= x << 11;    // (prune not needed) mul by 2048, mix
    return x & M24;  // prune, return
}

int main(void)
{
#ifdef TIMER
    starttimer();
#endif

    FILE *f = fopen(FNAME, "r");
    if (!f) { fprintf(stderr, "File not found: %s\n", FNAME); return 1; }

    uint64_t sum = 0;
    uint32_t x;
    while (fscanf(f, "%u ", &x) == 1) {
        uint8_t prev = x % 10;
        for (int j = 0; j < 3; ++j) {
            x = xorshift(x);
            const uint8_t cur = x % 10;  // value 0..9
            dif[j] = 9 + cur - prev;     // index 0..18
            prev = cur;
        }
        for (int j = 3; j < LEN; ++j) {
            x = xorshift(x);
            const uint8_t cur = x % 10;  // value 0..9
            dif[j] = 9 + cur - prev;     // index 0..18
            if (!seq[dif[j-3]][dif[j-2]][dif[j-1]][dif[j]])  // sequence not seen yet?
                seq[dif[j-3]][dif[j-2]][dif[j-1]][dif[j]] = cur + 1;  // range 1..10 = value 0..9
            prev = cur;
        }
        sum += x;  // part 1
        for (int a = 0; a < 19; ++a)
            for (int b = 0; b < 19; ++b)
                for (int c = 0; c < 19; ++c)
                    for (int d = 0; d < 19; ++d)
                        if (seq[a][b][c][d]) {  // range 1..10 = value 0..9
                            uni[a][b][c][d] += seq[a][b][c][d] - 1;
                            seq[a][b][c][d] = 0;  // reset sequence for next x
                        }
    }
    fclose(f);
    printf("%lld\n", sum);  // example1: 37327623, example2: 37990510, input: 19150344884
    qsort(uni, 20*20*20*20, sizeof ****uni, desc);
    printf("%u\n", uni[0][0][0][0]);  // example1: 24, example2: 23, input: 2121

#ifdef TIMER
    printf("Time: %.0f ms\n", stoptimer_ms());
#endif
    return 0;
}
