#include <stdio.h>
#include <stdlib.h>    // atoll
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRId64, PRIx64
#include <stdbool.h>

#define BIT32 (INT64_C(1) << 32)

// Modular multiplicative inverse
// https://en.wikipedia.org/wiki/Extended_Euclidean_algorithm#Modular_integers
static bool inverse(const int64_t a, const int64_t m,
    int64_t *const restrict gcd, int64_t *const restrict inv)
{
    int64_t t = 0, tnew = 1;
    int64_t r = m, rnew = a;
    while (rnew) {
        const int64_t q = r / rnew;
        int64_t tmp = t - q * tnew; t = tnew; tnew = tmp;
                tmp = r - q * rnew; r = rnew; rnew = tmp;
    }
    *gcd = r;
    *inv = t < 0 ? t + m : t;
    return r == 1;
}

int main(int argc, char *argv[])
{
    int64_t a = argc > 1 ? atoll(argv[1]) : 0;
    int64_t m = argc > 2 ? atoll(argv[2]) : BIT32;
    if (argc < 2 || argc > 3 || a < 1 || a >= m || m > BIT32) {
        fprintf(stderr, "Usage: %s <a> [m]\n"
            "  a is required, must be >0 and <2^32\n"
            "  m is optional, default 2^32, must be >a and <=2^32\n", argv[0]);
        return EXIT_FAILURE;
    }
    int64_t gcd, inv;
    const bool res = inverse(a, m, &gcd, &inv);
    printf("%"PRId64" is%s invertible (mod %"PRId64")\n",
        a, res ? "" : " not", m);
    printf("%s(%"PRId64",%"PRId64") = %"PRId64"\n",
        res ? "inv" : "gcd", a, m, res ? inv : gcd);
    printf("    %"PRId64" * %"PRId64" = %"PRId64" = %"PRId64" (mod %"PRId64")\n",
        a, inv, a * inv, gcd, m);
    printf("    %#"PRIx64" * %#"PRIx64" = %#"PRIx64" = %#"PRIx64" (mod %#"PRIx64")\n",
        a, inv, a * inv, gcd, m);
}
