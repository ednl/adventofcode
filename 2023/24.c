/**
 * Advent of Code 2023
 * Day 24: Never Tell Me The Odds
 * https://adventofcode.com/2023/day/24
 * By: E. Dronkert https://github.com/ednl
 */

#include <stdio.h>   // fopen, fclose, fscanf, printf
#include <stdint.h>  // int64_t, INT64_C
#include <math.h>    // fabs
#include <stdbool.h>

#define EXAMPLE 0
#if EXAMPLE
#define NAME "../aocinput/2023-24-example.txt"
#define N   5
#define MIN INT64_C(7)
#define MAX INT64_C(27)
#else
#define NAME "../aocinput/2023-24-input.txt"
#define N   300
#define MIN INT64_C(200000000000000)
#define MAX INT64_C(400000000000000)
#endif

typedef struct vec {
    int64_t x, y, z;
} Vec;

typedef struct plane {
    Vec v;
    int64_t n;
} Plane;

static Vec p[N], v[N];

static bool iszero(const Vec a)
{
    return a.x == 0 && a.y == 0 && a.z == 0;
}

static Vec sub(const Vec a, const Vec b)
{
    return (Vec){a.x - b.x, a.y - b.y, a.z - b.z};
}

static int64_t dot(const Vec a, const Vec b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static Vec cross(const Vec a, const Vec b)
{
    return (Vec){
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x};
}

static Plane plane(const int i, const int j)
{
    const Vec dp = sub(p[i], p[j]);
    return (Plane){cross(dp, sub(v[i], v[j])), dot(dp, cross(v[i], v[j]))};
}

int main(void)
{
    FILE *f = fopen(NAME, "r");
    if (!f) { fputs("File not found.", stderr); return 1; }
    for (int i = 0; i < N; ++i)
        fscanf(f, "%lld, %lld, %lld @ %lld, %lld, %lld", &p[i].x, &p[i].y, &p[i].z, &v[i].x, &v[i].y, &v[i].z);
    fclose(f);

    #if EXAMPLE || defined(DEBUG)
    for (int i = 0; i < N; ++i)
        printf("%lld, %lld, %lld @ %lld, %lld, %lld\n", p[i].x, p[i].y, p[i].z, v[i].x, v[i].y, v[i].z);
    printf("\n");
    #endif

    int part1 = 0;
    for (int i = 0; i < N - 1; ++i) {
        const double ai = (double)v[i].y / v[i].x;
        const double bi = p[i].y - ai * p[i].x;
        for (int j = i + 1; j < N; ++j) {
            const double aj = (double)v[j].y / v[j].x;
            const double bj = p[j].y - aj * p[j].x;
            if (fabs(ai - aj) > 0) {
                const double x = (bj - bi) / (ai - aj);
                const double y = ai * x + bi;
                if (x >= MIN && x <= MAX &&
                    y >= MIN && y <= MAX) {
                    const double ti = (x - p[i].x) / v[i].x;
                    const double tj = (x - p[j].x) / v[j].x;
                    if (ti > 0 && tj > 0)
                        ++part1;
                }
            }
        }
    }
    printf("Part 1: %d\n", part1);  // example: 2, input: 20434

    int i = 0;
    // printf("%d: p=(%lld,%lld,%lld) v=(%lld,%lld,%lld)\n", i, p[i].x, p[i].y, p[i].z, v[i].x, v[i].y, v[i].z);
    int j = i + 1;
    while (iszero(cross(v[i], v[j])))
        ++j;
    // printf("%d: p=(%lld,%lld,%lld) v=(%lld,%lld,%lld)\n", j, p[j].x, p[j].y, p[j].z, v[j].x, v[j].y, v[j].z);
    int k = j + 1;
    while (iszero(cross(v[i], v[k])) || iszero(cross(v[j], v[k])))
        ++k;
    // printf("%d: p=(%lld,%lld,%lld) v=(%lld,%lld,%lld)\n", k, p[k].x, p[k].y, p[k].z, v[k].x, v[k].y, v[k].z);
    // Now 3 linearly independent lines: p,v[i] p,v[j] p,v[k]
    // Find rock position r and speed s such that:
    //   r = p[i] + (v[i] - s).t1
    //   r = p[j] + (v[j] - s).t2
    //   r = p[k] + (v[k] - s).t3

    Plane A = plane(i, j);
    Plane B = plane(i, k);
    Plane C = plane(j, k);
    // printf("a=(%lld,%lld,%lld)\n", A.v.x, A.v.y, A.v.z);
    // printf("b=(%lld,%lld,%lld)\n", B.v.x, B.v.y, B.v.z);
    // printf("c=(%lld,%lld,%lld)\n", C.v.x, C.v.y, C.v.z);
    const Vec bc = cross(B.v, C.v);
    const Vec ca = cross(C.v, A.v);
    const Vec ab = cross(A.v, B.v);
    // printf("bc=(%lld,%lld,%lld)\n", bc.x, bc.y, bc.z);
    // printf("ca=(%lld,%lld,%lld)\n", ca.x, ca.y, ca.z);
    // printf("ab=(%lld,%lld,%lld)\n", ab.x, ab.y, ab.z);
    const double sp = (double)A.n / dot(A.v, bc);
    const double sq = (double)B.n / dot(B.v, ca);
    const double sr = (double)C.n / dot(C.v, ab);
    const Vec s = (Vec){
        (int64_t)(round(sp * bc.x + sq * ca.x + sr * ab.x)),
        (int64_t)(round(sp * bc.y + sq * ca.y + sr * ab.y)),
        (int64_t)(round(sp * bc.z + sq * ca.z + sr * ab.z))};
    printf("%lld %lld %lld\n", s.x, s.y, s.z);
    v[i] = sub(v[i], s);
    v[j] = sub(v[j], s);
    const Vec vv = cross(v[i], v[j]);
    const int64_t rp = dot(vv, cross(p[j], v[j]));
    const int64_t rq = dot(vv, cross(p[i], v[i]));
    const int64_t rr = dot(p[i], vv);
    const int64_t vv2 = dot(vv, vv);
    const Vec r = (Vec){
        rp * v[i].x - rq * v[j].x + rr * vv.x,
        rp * v[i].y - rq * v[j].y + rr * vv.y,
        rp * v[i].z - rq * v[j].z + rr * vv.z};
    printf("%lld %lld %lld\n", r.x, r.y, r.z);
    printf("S=%lld\n", vv2);
    printf("%lld %lld %lld\n", r.x / vv2, r.y / vv2, r.z / vv2);
    printf("%lld %lld %lld\n", r.x % vv2, r.y % vv2, r.z % vv2);

    return 0;
}
