#include <stdio.h>     // printf
#include <stdlib.h>    // abs
#include <stdint.h>    // uint64_t, uint_fast32_t, UINT32_C
#include <inttypes.h>  // PRIuFAST32
#include <stdbool.h>   // bool, true, false
#include <time.h>      // clock_gettime
#include <math.h>      // sqrt

// #define BASE (INT32_C(7)) 
// #define MOD  (INT32_C(20201227))

typedef struct {
    int_fast32_t exp;
    int_fast32_t log;
} EXPLOG, *PEXPLOG;

// static double timer(void)
// {
//     static bool start = true;
//     static struct timespec t0;
//     struct timespec t1;

//     if (start) {
//         start = false;
//         clock_gettime(CLOCK_MONOTONIC_RAW, &t0);
//         return 0;
//     } else {
//         clock_gettime(CLOCK_MONOTONIC_RAW, &t1);
//         start = true;
//         return 1.0 * t1.tv_sec + 1e-9 * t1.tv_nsec - (1.0 * t0.tv_sec + 1e-9 * t0.tv_nsec);
//     }
// }

// Naive discrete logarithm by trial multiplication
// return e where BASE^e (mod MOD) = h
static void modlog(int_fast32_t arg, int_fast32_t base, int_fast32_t mod)
{
    int_fast32_t exp = 0, n = 1;
    while (n != arg) {            // the target is 'arg'
        n = n * base % mod;       // all 32-bit numbers (BASE: 3bit, MOD: 25bit => k * BASE 28bit)
        ++exp;                    // exponent = multiplication count
    }
    printf("%i ^ %i = %i (mod %i)\n\n", base, exp, arg, mod);
}

// Naive discrete logarithm by trial multiplication
// return {e,p} where BASE^e (mod MOD) = p
//     or {e,q} where BASE^e (mod MOD) = q
// for whichever e is smaller
// static EXPLOG modlog2(int_fast32_t p, int_fast32_t q)
// {
//     int_fast32_t e = 0, k = 1U;
//     while (k != p && k != q) {    // symmetry in p, q
//         k = k * BASE % MOD;       // all 32-bit numbers (BASE: 3bit, MOD: 25bit => k * BASE 28bit)
//         ++e;                      // exponent = multiplication count
//     }
//     return (EXPLOG){e, k == p ? p : q};  // but use the other one for next part of puzzle
// }

static int_fast32_t inverse(int_fast32_t a, int_fast32_t n)
{
    int_fast32_t q, tmp, t = 0, newt = 1, r = n, newr = a;

    while (newr) {
        q = r / newr;
        tmp = t; t = newt; newt = tmp - q * newt;
        tmp = r; r = newr; newr = tmp - q * newr;
    }

    if (r > 1) {
        return -1;
    }
    if (t < 0) {
        t += n;
    }
    return t;
}

static void ext_gcd(int64_t a, int64_t b)
{
    int64_t q, tmp, r0 = a, r = b, s0 = 1, s = 0, t0 = 0, t = 1;

    while (r) {
        q = r0 / r;
        tmp = r0 - q * r; r0 = r; r = tmp;
        tmp = s0 - q * s; s0 = s; s = tmp;
        tmp = t0 - q * t; t0 = t; t = tmp;
    }

    // Correct signs
    if (r0 < 0) {
        r0 = -r0;  // GCD should not be negative
    }
    if ((a < 0) ^ (t < 0)) {
        t = -t;    // t = reduced a, should have same sign as a
    }
    if ((b < 0) ^ (s < 0)) {
        s = -s;    // s = reduced b, should have same sign as b
    }

    printf("a,b     : %9"PRId64" %9"PRId64"\n", a, b);  // in the function call, a = B-b, b = n
    printf("Bezout  : %9"PRId64" %9"PRId64"\n", s0, t0);
    printf("GCD     : %9"PRId64"\n", r0);
    printf("a,b/gcd : %9"PRId64" %9"PRId64"\n\n", t, s);  // t = reduced a = (B-b)/gcd, s = reduced b = n/gcd
}

// Calculate the discrete alpha'th-logarithm of beta (mod N)
// i.e. find gamma where alpha ** gamma = beta (mod N)
// https://en.wikipedia.org/wiki/Pollard%27s_rho_algorithm_for_logarithms
static void rholog(int64_t alpha, int64_t beta, int64_t N)
{
    int64_t x = 1, X = 1;
    int64_t i, a = 0, b = 0, A = 0, B = 0, n = N - 1;

    for (i = 1; i < n; ++i) {
        switch (x % 3) {
            case 0: x = x * x     % N; a =  a*2  % n; b =  b*2  % n; break;
            case 1: x = x * alpha % N; a = (a+1) % n;                break;
            case 2: x = x * beta  % N;                b = (b+1) % n; break;
        }
        switch (X % 3) {
            case 0: X = X * X     % N; A =  A*2  % n; B =  B*2  % n; break;
            case 1: X = X * alpha % N; A = (A+1) % n;                break;
            case 2: X = X * beta  % N;                B = (B+1) % n; break;
        }
        switch (X % 3) {
            case 0: X = X * X     % N; A =  A*2  % n; B =  B*2  % n; break;
            case 1: X = X * alpha % N; A = (A+1) % n;                break;
            case 2: X = X * beta  % N;                B = (B+1) % n; break;
        }
        if (x == X) break;
    }
    printf("i,n,N   : %9"PRId64" %9"PRId64" %9"PRId64"\n", i, n, N);
    printf("x,a,b   : %9"PRId64" %9"PRId64" %9"PRId64"\n", x, a, b);
    printf("X,A,B   : %9"PRId64" %9"PRId64" %9"PRId64"\n", X, A, B);

    int64_t p = B >= b ? B - b : n - b + B;
    int64_t q = a >= A ? a - A : n - A + a;
    printf("B-b,a-A : %9"PRId64" %9"PRId64"\n\n", p, q);

    // Now: (B-b) . gamma = (a-A) (mod n)  where: alpha ** gamma = beta (mod N)
    // We must invert B-b (mod n) to get gamma. First, get the GCD of B-b and n:
    ext_gcd(p, n);
    // And use this result: gamma = (a-A)/gcd . ((B-b)/gcd)^-1 (mod n/gcd)
    // Because of Bézout's identity (B-b).x + n.y = gcd(B-b,n) <=> (B-b)/gcd . x + n/gcd . y = 1
    // (mod n) it is inferred that the first Bézout coefficient x is de inverse of (B-b)/gcd
    // thus gamma0 = q/r0 . s0 (mod s)  where: q = a-A, r0 = gcd, s0 = Bezout_x, s = n/gcd
    // All solutions are gamma = (gamma0 + k . s) mod n  with k = 0..gcd-1
    // One of them satisfies alpha ** gamma = beta (mod N).
    // but which one??
}

// static uint_fast32_t babygiant(uint_fast32_t h)
// {
//     static const uint_fast32_t m = (uint_fast32_t)ceil(sqrt(MOD));
//     static bool maketable = true;

// }

// static uint_fast32_t nextpow2(uint_fast32_t num)
// {
//     uint_fast32_t count = 0;
//     while (num) {
//         num >>= 1;
//         ++count;
//     }
//     return (1 << count) % MOD;
// }

// static void result(uint_fast32_t p, uint_fast32_t q)
// {
//     int i, warmup = 3, loop = 100;
//     volatile uint_fast32_t r1 = {0}, r2 = {0};
//     double t, t1 = 0, t2 = 0, t1min = 10, t2min = 10, t1max = 0, t2max = 0;

//     for (i = 0; i < warmup; ++i) {
//         r1 = discrlog1(p);
//         r2 = discrlog1(q);
//     }
//     for (i = 0; i < loop; ++i) {
//         timer(); r1 = discrlog1(p); t = timer(); if (t < t1min) { t1min = t; } if (t > t1max) { t1max = t; } t1 += t;
//         timer(); r2 = discrlog1(q); t = timer(); if (t < t2min) { t2min = t; } if (t > t2max) { t2max = t; } t2 += t;
//     }
//     printf("  %"PRIu32" ^%9"PRIuFAST32" = %9"PRIuFAST32" (min %.5f avg %.5f max %.5f s)\n", BASE, r1, p, t1min, t1 / loop, t1max);
//     printf("  %"PRIu32" ^%9"PRIuFAST32" = %9"PRIuFAST32" (min %.5f avg %.5f max %.5f s)\n", BASE, r2, q, t2min, t2 / loop, t2max);
// }

int main(void)
{
    // printf("%i\n", inverse(59994, 166667));
    // printf("%i\n", inverse(359964, 1000002));

    modlog(15113849, 7, 20201227);
    rholog(7, 15113849, 20201227);

    // modlog(4206373, BASE, MOD);
    // rholog(BASE, 4206373, MOD);

    // modlog(262682, 5, 1000003);
    // rholog(5, 262682, 1000003);

    // printf("\nalgorithm (ednl)\n");
    // result(15113849, 4206373);

    // printf("\nejolson\n");
    // result(6270530, 14540258);

    // printf("\nlurk101\n");
    // result(16915772, 18447943);

    // printf("\n");
    return 0;
}
