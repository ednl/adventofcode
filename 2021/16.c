#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <limits.h>
#include <stdbool.h>
#include "startstoptimer.h"

#define N      1396     // hex chars in input
#define NIBBLE    4     // hex char = 4 bits
#define M (N * NIBBLE)  // bits in input
#define VLEN      3     // version = 3 bits
#define TLEN      3     // type = 3 bits
#define SLEN     15     // LenID=0: subpacket size  = 15 bits
#define CLEN     11     // LenID=1: subpacket count = 11 bits

static bool bit[M] = {0};
static int index = 0;
static int versionsum = 0;

static inline int64_t min(int64_t a, int64_t b)
{
    return a < b ? a : b;
}

static inline int64_t max(int64_t a, int64_t b)
{
    return a > b ? a : b;
}

static int64_t getnumber(int len)
{
    int64_t n = 0;
    while (len--)
        n = (n << 1) | bit[index++];
    return n;
}

static int64_t getliteral(void)
{
    int64_t val = 0;
    bool more;
    do {
        more = bit[index++];
        val = (val << NIBBLE) | getnumber(NIBBLE);
    } while (more);
    return val;
}

static int64_t getpacket(void)
{
    versionsum += (int)getnumber(VLEN);
    int typeid = (int)getnumber(TLEN);
    if (typeid == 4)
        return getliteral();

    int count, size;
    if (bit[index++]) {
        count = (int)getnumber(CLEN);
        size  = INT_MAX;
    } else {
        count = INT_MAX;
        size  = (int)getnumber(SLEN);
    }
    int64_t val = 0;
    if (typeid < 4) {
        switch (typeid) {
            case 1: val = 1; break;  // prod
            case 2: val = INT64_MAX; break;  // min
            case 3: val = INT64_MIN; break;  // max
        }
        while (count > 0 && size > 0) {
            int ix = index;
            int64_t a = getpacket();
            switch (typeid) {
                case 0: val += a; break;  // sum
                case 1: val *= a; break;  // prod
                case 2: val = min(val, a); break;  // min
                case 3: val = max(val, a); break;  // max
            }
            count--;
            size -= index - ix;
        }
    } else {
        int64_t a = getpacket();
        int64_t b = getpacket();
        switch (typeid) {
            case 5: val = a > b;  break;  // gt
            case 6: val = a < b;  break;  // lt
            case 7: val = a == b; break;  // eq
        }
    }
    return val;
}

int main(void)
{
    starttimer();

    FILE *f = fopen("../aocinput/2021-16-input.txt", "r");
    char hex[17] = {0};
    int k = 0;
    while (fscanf(f, "%16s", hex) == 1) {
        char *end;
        uint64_t val = strtoull(hex, &end, 16);
        ssize_t bitlen = (end - hex) * 4;
        uint64_t mask = UINT64_C(1) << (bitlen - 1);
        while (mask) {
            bit[k++] = val & mask;
            mask >>= 1;
        }
    }
    fclose(f);

    int64_t part2 = getpacket();
    printf("Part 1: %d\n", versionsum);    // 936
    printf("Part 2: %"PRId64"\n", part2);  // 6802496672062

    printf("%.0f µs\n", stoptimer_us());
    return 0;
}
