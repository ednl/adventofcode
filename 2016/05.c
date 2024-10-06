#include <stdio.h>
#include <stdint.h>  // uint32_t
#include <string.h>  // strlen, memcpy
#include <stdbool.h>
#include "../startstoptimer.h"

// My puzzle input
// #define DOORID "abc"    // example
#define DOORID "ojvtpuvg"  // input

static char pass1[9], pass2[9];  // 8 chars + NUL
static uint8_t passindex1, passindex2;  // keep track of pass indices filled

static char hexdigit(const uint32_t val)
{
    char x = (char)(val & 0xf);
    return x < 10 ? '0' + x : 'W' + x;
}

// Ref.: https://en.wikipedia.org/wiki/MD5#Algorithm
// Message buffer size must be >= 64
static bool md5(void)
{
    static const uint32_t rot[64] = {
        7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,
        5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,
        4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,
        6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21};

    static const uint32_t K[64] = {
        0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
        0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
        0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
        0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
        0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
        0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
        0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
        0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
        0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
        0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
        0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
        0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
        0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
        0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
        0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
        0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391};

    static const uint8_t pad[64] = {0x80};  // rest is zero
    static uint32_t index = 1469591;

    // Padding: append 0x80 and pad with 0x00 bytes so that the message length in bytes ≡ 56 (mod 64).
    unsigned char chunk[64];
    int bytelen = sprintf((char*)chunk, "%s%u", DOORID, index++);
    memcpy(&chunk[bytelen], pad, (size_t)(64 - bytelen));

    // Store length of original message *in bits *in last 64 bits of message (little-endian!)
    chunk[56] = (unsigned char)(bytelen << 3);
    chunk[57] = (unsigned char)(bytelen >> 5);
    // Because bytelen <= 56, not needed:
    // chunk[58] = (unsigned char)(bytelen >> 13);
    // chunk[59] = (unsigned char)(bytelen >> 21);
    // etc.

    // Break 512-bit message chunk into sixteen 32-bit words M[j]
    uint32_t M[16];
    for (uint32_t j = 0; j < 16; ++j) {
        uint32_t k = j << 2;
        M[j] = (uint32_t)chunk[k + 3] << 24
             | (uint32_t)chunk[k + 2] << 16
             | (uint32_t)chunk[k + 1] <<  8
             | (uint32_t)chunk[k];
    }

    uint32_t A = 0x67452301, B = 0xefcdab89, C = 0x98badcfe, D = 0x10325476;
    uint32_t AA = A, F, g;

    for (uint32_t i = 0; i < 64; ++i) {
        switch (i >> 4) {
            case 0:
                F = D ^ (B & (C ^ D));
                g = i;
                break;
            case 1:
                F = C ^ (D & (B ^ C));
                g = (i * 5 + 1) & 0xf;
                break;
            case 2:
                F = B ^ C ^ D;
                g = (i * 3 + 5) & 0xf;
                break;
            default:
                F = C ^ (B | ~D);
                g = (i * 7) & 0xf;
                break;
        }
        F += A + K[i] + M[g];
        A = D;
        D = C;
        C = B;
        B += F << rot[i] | F >> (32 - rot[i]);
    }

    AA += A;

    // Digest starts with 5 zeroes?
    if (AA & 0x00f0ffff)
        return false;

    // Advent of Code day 5 part 1: next digit in pass is 6th hex digit after 5 zeroes
    uint32_t h6 = (AA & 0x000f0000) >> 16;
    if (passindex1 < 8) {
        // printf("1: %8u\n", index - 1);  // 1469591,1925351,4310992,4851204,6610226,6840976,9504234,10320588
        pass1[passindex1++] = hexdigit(h6);
        if (passindex1 == 8)
            printf("Part 1: %s\n", pass1);  // example = 18f47a30, input = 4543c154
    }

    // Advent of Code day 5 part 2: place = 6th hex digit after 5 zeroes, set pass[place] = 7th hex digit
    if (h6 > 7 || (passindex2 & (1U << h6)))  // valid index 0..7, not already set
        return false;
    // printf("2: %8u\n", index - 1);  // 1469591,1925351,4851204,6840976,13615245,16706030,23173947,27649592
    uint32_t h7 = (AA & 0xf0000000) >> 28;  // 7th hex digit in digest
    pass2[h6] = hexdigit(h7);
    passindex2 |= (1U << h6);
    if (passindex2 != 255)
        return false;
    printf("Part 2: %s\n", pass2);  // example = 05ace8e3, input = 1050cbbd
    return true;
}

int main(void)
{
    starttimer();
    while (!md5());
    printf("Time: %.2f s\n", stoptimer_s());
    return 0;
}
