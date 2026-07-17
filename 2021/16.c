/**
 * Advent of Code 2021
 * Day 16: Packet Decoder
 * https://adventofcode.com/2021/day/16
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 16.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 16.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 1.93 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : ? µs
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRId64
#include <limits.h>    // INT_MAX, INT64_MAX, INT64_MIN
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2021-16-input.txt"
#define HEX  1396  // hex chars in input
#define NIB     4  // 1 hex char = 4 bits
#define BIT (HEX * NIB)  // bits in input
#define VERSION 3  // version = 3 bits
#define TYPE    3  // type = 3 bits
#define SIZE   15  // LenID=0: subpacket size  = 15 bits
#define COUNT  11  // LenID=1: subpacket count = 11 bits

static char input[HEX + 1];  // +'\0'
static bool bit[BIT];
static int index;
static int versionsum;

static int64_t min(const int64_t a, const int64_t b)
{
    return a < b ? a : b;
}

static int64_t max(const int64_t a, const int64_t b)
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
        val = (val << NIB) | getnumber(NIB);
    } while (more);
    return val;
}

static int64_t getpacket(void)
{
    versionsum += (int)getnumber(VERSION);
    int typeid = (int)getnumber(TYPE);
    if (typeid == 4)
        return getliteral();

    int count = INT_MAX, size = INT_MAX;
    if (bit[index++])
        count = (int)getnumber(COUNT);
    else
        size  = (int)getnumber(SIZE);
    int64_t val = 0;
    if (typeid < 4) {
        switch (typeid) {
            case 1: val = 1; break;  // prod
            case 2: val = INT64_MAX; break;  // min
            case 3: val = INT64_MIN; break;  // max
        }
        while (count > 0 && size > 0) {
            const int oldindex = index;
            const int64_t a = getpacket();
            switch (typeid) {
                case 0: val += a; break;  // sum
                case 1: val *= a; break;  // prod
                case 2: val = min(val, a); break;  // min
                case 3: val = max(val, a); break;  // max
            }
            count--;
            size -= index - oldindex;
        }
    } else {
        const int64_t a = getpacket();
        const int64_t b = getpacket();
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
    FILE *f = fopen(FNAME, "rb");  // fread() requires binary mode
    if (!f) return 1;
    fread(input, HEX, 1, f);  // read single block of HEX bytes
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
    index = versionsum = 0;
#endif

    // Assume current arch is little-endian
    // '1' = bitstream 0001 = 4 bytes 0,0,0,1 = int 0x01000000
    uint32_t *nib = (uint32_t *)bit;
    for (int i = 0; i < HEX; nib++, ++i)
        switch (input[i]) {
            case '0': *nib = 0x00000000; break;  // 0000
            case '1': *nib = 0x01000000; break;  // 0001
            case '2': *nib = 0x00010000; break;  // 0010
            case '3': *nib = 0x01010000; break;  // 0011
            case '4': *nib = 0x00000100; break;  // 0100
            case '5': *nib = 0x01000100; break;  // 0101
            case '6': *nib = 0x00010100; break;  // 0110
            case '7': *nib = 0x01010100; break;  // 0111
            case '8': *nib = 0x00000001; break;  // 1000
            case '9': *nib = 0x01000001; break;  // 1001
            case 'A': *nib = 0x00010001; break;  // 1010
            case 'B': *nib = 0x01010001; break;  // 1011
            case 'C': *nib = 0x00000101; break;  // 1100
            case 'D': *nib = 0x01000101; break;  // 1101
            case 'E': *nib = 0x00010101; break;  // 1110
            case 'F': *nib = 0x01010101; break;  // 1111
        }
    const int64_t part2 = getpacket();
    printf("%d %"PRId64"\n", versionsum, part2);  // 936 6802496672062

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
