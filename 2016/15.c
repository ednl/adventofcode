#include <stdio.h>
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRId64

#define N 6  // 6 discs in input file

typedef struct {
    int64_t m1, m2;
} bezout_t;

typedef struct {
    int64_t id, size, pos, rem;
} disc_t;

static disc_t disc[N];

// Disc remainder depends on its ID (=vertical position), position (=initial rotation)
static int64_t rem(const int64_t id, const int64_t pos)
{
    return -(id + pos);
}

static void show(int n)
{
    putchar('\n');
    for (int i = 0; i < n; ++i) {
        const disc_t * const d = &disc[i];
        printf("disc %"PRId64" size %2"PRId64" pos %2"PRId64
            "  =>  (t+%"PRId64"+%2"PRId64") %% %2"PRId64" = 0"
            "  =>  t = %3"PRId64" %% %2"PRId64
            "  =>  t \u2261 %2"PRId64" (mod %2"PRId64")\n",
            d->id, d->size, d->pos,
            d->id, d->pos, d->size,
            d->rem, d->size,
            (d->rem % d->size + d->size) % d->size, d->size);
    }
    putchar('\n');
}

// Bézout coefficients of two numbers a,b
// Ref.: https://en.wikipedia.org/wiki/Chinese_remainder_theorem#Computation
//       https://en.wikipedia.org/wiki/Extended_Euclidean_algorithm#Pseudocode
static bezout_t bezout(const int64_t a, const int64_t b)
{
    int64_t q, tmp;
    int64_t r0 = a, r = b;
    int64_t s0 = 1, s = 0;
    int64_t t0 = 0, t = 1;

    while (r) {
        q = r0 / r;
        tmp = r0 - q * r; r0 = r; r = tmp;
        tmp = s0 - q * s; s0 = s; s = tmp;
        tmp = t0 - q * t; t0 = t; t = tmp;
    }
    return (bezout_t){s0, t0};
}

// Reduce two congruence pairs of the same variable to one
// Ref.: https://en.wikipedia.org/wiki/Chinese_remainder_theorem#Existence_(constructive_proof)
static void reduce(int64_t * const a1, int64_t * const n1, const int64_t a2, const int64_t n2)
{
    bezout_t bz = bezout(*n1, n2);  // Bézout coefficients of two moduli n1,n2
    int64_t n = *n1 * n2;  // new modulus
    int64_t a = (*a1 * bz.m2 * n2 + a2 * bz.m1 * *n1) % n;  // new remainder
    if (a < 0)  // times can't be negative
        a += n;
    *a1 = a;
    *n1 = n;
}

int main(void)
{
    FILE *f = fopen("../aocinput/2016-15-input.txt", "r");
    if (!f)
        return 1;

    int n = 0;
    int64_t id, size, pos;
    while (n < N && fscanf(f, "Disc #%"PRId64" has %"PRId64" positions; at time=0, it is at position %"PRId64". ", &id, &size, &pos) == 3)
        disc[n++] = (disc_t){id, size, pos, rem(id, pos)};
    fclose(f);

    // Check
    show(n);

    // Part 1
    for (int i = 1; i < n; ++i)
        reduce(&disc[0].rem, &disc[0].size, disc[i].rem, disc[i].size);
    printf("Part 1: t \u2261 %"PRId64" (mod %"PRId64")\n", disc[0].rem, disc[0].size);  // 400589

    // Part 2: add 1 disc of size 11 at position 0
    reduce(&disc[0].rem, &disc[0].size, rem(n + 1, 0), 11);
    printf("Part 2: t \u2261 %"PRId64" (mod %"PRId64")\n", disc[0].rem, disc[0].size);  // 3045959

    return 0;
}
