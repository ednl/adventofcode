// As /u/mebeim writes at
// https://www.reddit.com/r/adventofcode/comments/kjtg7y/2020_day_25_solutions/ggz4bpl/
// the problem can be rephased in terms of the Diffie-Hellman key exchange algorithm:
// "Given the public base g = 7, the public modulus p = 20201227, and the two public keys A and B
// (obtained as A = g^a mod p, B = g^b mod p), calculate the secret key s = A^b mod p = B^a mod p."
// Ref. https://en.wikipedia.org/wiki/Diffie-Hellman_key_exchange

// My version: pick the earliest matching key (p or q) in the loop,
// then use the loop count (e) with the other key (q or p) to
// calculate the secret key with modular exponentiation.

#include <stdio.h>     // printf
#include <stdint.h>    // uint64_t, uint_fast32_t, UINT32_C
#include <inttypes.h>  // PRIuFAST32
#include <stdbool.h>   // bool, true, false
#include <time.h>      // clock_gettime

#define BASE (UINT32_C(7))
#define MOD  (UINT32_C(20201227))

static double timer(void)
{
    static bool start = true;
    static struct timespec t0;
    struct timespec t1;

    if (start) {
        start = false;
        clock_gettime(CLOCK_MONOTONIC_RAW, &t0);
        return 0;
    } else {
        clock_gettime(CLOCK_MONOTONIC_RAW, &t1);
        start = true;
        return 1.0 * t1.tv_sec + 1e-9 * t1.tv_nsec - (1.0 * t0.tv_sec + 1e-9 * t0.tv_nsec);
    }
}

// Break Diffie-Hellman!!1!
static uint_fast32_t dhke(uint_fast32_t p, uint_fast32_t q)
{
    // Naive discrete logarithm
    uint_fast32_t e = 0, k = 1U;
    while (k != p && k != q) {    // symmetry in p, q
        k = k * BASE % MOD;       // all 32-bit numbers (BASE: 3bit, MOD: 25bit => k * BASE 28bit)
        ++e;                      // exponent = multiplication count
    }
    uint64_t b = k == q ? p : q;  // new base for next part is a 64-bit int

    // Modular exponentiation with fixed modulus
    // https://en.wikipedia.org/wiki/Modular_exponentiation#Right-to-left_binary_method
    uint64_t r = 1U, m = MOD;     // only use 64bit (except e) to avoid conversions
    b %= m;
    while (e) {
        if (e & 1U) {
            r = r * b % m;        // r * b can be >32bit so use 64bit here
        }
        b = b * b % m;            // b * b can be >32bit
        e >>= 1;
    }
    return (uint_fast32_t)r;      // r is mod m, and m is 25bit, so no truncation
}

static void result(uint_fast32_t p, uint_fast32_t q)
{
    int i, warmup = 3, loop = 100;
    volatile uint_fast32_t r1 = 0, r2 = 0;
    double t, t1 = 0, t2 = 0, t1min = 10, t2min = 10, t1max = 0, t2max = 0;

    for (i = 0; i < warmup; ++i) {
        r1 = dhke(p, q);
        r2 = dhke(q, p);
    }
    for (i = 0; i < loop; ++i) {
        timer(); r1 = dhke(p, q); t = timer(); if (t < t1min) { t1min = t; } if (t > t1max) { t1max = t; } t1 += t;
        timer(); r2 = dhke(q, p); t = timer(); if (t < t2min) { t2min = t; } if (t > t2max) { t2max = t; } t2 += t;
    }
    printf("  %8"PRIuFAST32" %8"PRIuFAST32" : %8"PRIuFAST32" (min %.5f avg %.5f max %.5f s)\n", p, q, r1, t1min, t1 / loop, t1max);
    printf("  %8"PRIuFAST32" %8"PRIuFAST32" : %8"PRIuFAST32" (min %.5f avg %.5f max %.5f s)\n", q, p, r2, t2min, t2 / loop, t2max);
}

int main(void)
{
    printf("\nalgorithm (ednl)\n");
    result(15113849, 4206373);

    printf("\nejolson\n");
    result(6270530, 14540258);

    printf("\nlurk101\n");
    result(16915772, 18447943);

    printf("\n");
    return 0;
}
