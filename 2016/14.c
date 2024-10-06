#include <stdio.h>
#include <stdlib.h>  // div, qsort
#include <stdint.h>  // uint32_t
#include <string.h>  // memcpy
#include <stdbool.h>
#include "../startstoptimer.h"

#define EXAMPLE 0
#if EXAMPLE == 1
static const char salt[] = "abc";
#else
static const char salt[] = "zpqevtbw";
#endif
#define QSIZE 1000

typedef struct {
    size_t len, pop, ins;
    int q[QSIZE];
} queue_t;

static const size_t saltlen = sizeof salt - 1;
static queue_t rep3[16];

// Dequeue = pop off the tail of the queue
static bool deq(queue_t *q, int *val)
{
    if (!q || !q->len)
        return false;
    q->len--;
    *val = q->q[q->pop++];
    if (q->pop == QSIZE)
        q->pop = 0;
    return true;
}

// Enqueue = push onto the head of the queue
static bool enq(queue_t *q, const int val)
{
    if (!q || q->len == QSIZE)
        return false;
    q->len++;
    q->q[q->ins++] = val;
    if (q->ins == QSIZE)
        q->ins = 0;
    return true;
}

// Single hex character from value [0..15], hopefully fast
static char hexc(const uint8_t val)
{
    return "0123456789abcdef"[val];
}

// Value [0..15] from hex character, hopefully fast
static uint8_t valc(const char hex)
{
    switch (hex) {
        case '0': return 0x0;
        case '1': return 0x1;
        case '2': return 0x2;
        case '3': return 0x3;
        case '4': return 0x4;
        case '5': return 0x5;
        case '6': return 0x6;
        case '7': return 0x7;
        case '8': return 0x8;
        case '9': return 0x9;
        case 'a': return 0xa;
        case 'b': return 0xb;
        case 'c': return 0xc;
        case 'd': return 0xd;
        case 'e': return 0xe;
        case 'f': return 0xf;
    }
    return 0;
}

// Reverses string between pointers first and last (exclusive)
static void revstr(char *first, char *last)
{
    for (--last; first < last; ++first, --last) {
        char c = *last;
        *last = *first;
        *first = c;
    }
}

// Render decimal number in string, like sprintf(buf, "%d", val) but faster
static int sprintnum(char * const buf, const int val)
{
    char *c = buf;
    div_t d = {.quot = val};
    do {
        d = div(d.quot, 10);
        *c++ = "0123456789"[d.rem];
    } while (d.quot);
    *c = '\0';
    revstr(buf, c);
    return (int)(c - buf);
}

// Return char of first triplet in string, or NUL
static char triplet(const char * const s)
{
    const char *a = s, *b = s;
    while (*a) {
        while (*(++b) == *a);
        if (b - a >= 3)
            return *a;
        a = b;
    }
    return '\0';
}

// Find all sets of five or more repeated chars
//   store unique char in null terminated buf (size>=7)
//   return number of different sets found
static int quintuplets(const char * const s, char * const found)
{
    int count = 0;
    bool dupe[16] = {0};
    const char *a = s, *b = s;
    while (*a) {
        while (*(++b) == *a);
        if (b - a >= 5 && !dupe[valc(*a)]) {
            dupe[valc(*a)] = true;
            found[count++] = *a;
        }
        a = b;
    }
    found[count] = '\0';
    return count;
}

// Ref.: https://en.wikipedia.org/wiki/MD5#Algorithm
//   message buffer size must be null terminated string strlen <= 55
//   digest  buffer size must be >= 33, will become null terminated hex string strlen = 32
static void md5(const char * const message, char * const digest, int stretch)
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

    // Copy message to local chunk[]
    // Padding: append 0x80 and pad with 0x00 bytes so that the message length in bytes ≡ 56 (mod 64).
    uint8_t chunk[64];
    uint32_t bytes = 0;
    for (; bytes < 55 && message[bytes]; ++bytes)
        chunk[bytes] = (uint8_t)message[bytes];

    while (stretch--) {
        memcpy(chunk + bytes, pad, (size_t)(64 - bytes));

        // Store length of original message *in bits *in last 64 bits of chunk (little-endian!)
        chunk[56] = (uint8_t)(bytes << 3);
        chunk[57] = (uint8_t)(bytes >> 5);
        // Because bytes <= 55, not needed:
        // chunk[58] = (uint8_t)(bytes >> 13);  // etc. up to & incl. chunk[63]

        // Break 512-bit message into sixteen 32-bit words M[j]
        uint32_t M[16];
        for (int j = 0; j < 16; ++j) {
            int k = j << 2;  // j * 4
            M[j] = (uint32_t)chunk[k + 3] << 24  // high to low prec: cast, shift, or
                 | (uint32_t)chunk[k + 2] << 16
                 | (uint32_t)chunk[k + 1] <<  8
                 | (uint32_t)chunk[k];
        }

        uint32_t A = 0x67452301, B = 0xefcdab89, C = 0x98badcfe, D = 0x10325476;
        uint32_t buf[4] = {A, B, C, D}, F, g;

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

        buf[0] += A;
        buf[1] += B;
        buf[2] += C;
        buf[3] += D;

        // Save hex digest[0..31]
        char *c = digest;
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j) {
                *c++ = hexc(buf[i] >> 4 & 0xf);  // high to low prec: shift, and
                *c++ = hexc(buf[i] & 0xf);
                buf[i] >>= 8;
            }
        *c = '\0';  // digest[32] = NUL

        // For stretching: copy from digest to chunk, set bytes
        memcpy(chunk, digest, 32);
        bytes = 32;
    }
}

static int compare_ints(const void *a, const void *b)
{
    const int arg1 = *(const int*)a;
    const int arg2 = *(const int*)b;

    if (arg1 < arg2) return -1;
    if (arg1 > arg2) return  1;
    return 0;
}

static int part(int stretch)
{
    char msg[64], hash[64], f[8], *dst = msg, c;
    const char *src = salt;
    size_t keycount = 0;
    int keys[128], index = 0, n, x;

    while ((*dst++ = *src++));
    memset(rep3, 0, sizeof rep3);

    while (keycount < 66) {  // 64 keys + 2 margin
        (void)sprintnum(msg + saltlen, index);
        md5(msg, hash, stretch);
        n = quintuplets(hash, f);
        while (n) {
            queue_t * const q = &rep3[valc(f[--n])];
            while (deq(q, &x))
                if (index - x <= 1000)
                    keys[keycount++] = x;
        }
        if ((c = triplet(hash)))
            (void)enq(&rep3[valc(c)], index);
        ++index;
    }
    qsort(keys, keycount, sizeof *keys, compare_ints);
    return keys[63];
}

int main(void)
{
    starttimer();
    printf("Part 1: %d\n", part(1));     // example = 22728, input = 16106
    printf("Part 2: %d\n", part(2017));  // example = 22551, input = 22423
    printf("Time: %.2f s\n", stoptimer_s());
    return 0;
}
