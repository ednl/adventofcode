/**
 * Advent of Code 2023
 * Day 24: Never Tell Me The Odds
 * https://adventofcode.com/2023/day/24
 * By: E. Dronkert https://github.com/ednl
 */

#include <stdio.h>
#include <math.h>     // sqrt
#include <stdint.h>   // int64_t
#include <stdbool.h>

#define EXAMPLE 1
#if EXAMPLE
#define NAME "../aocinput/2023-24-example.txt"
#define N    5
#define MIN  7
#define MAX 27
#else
#define NAME "../aocinput/2023-24-input.txt"
#define N   300
#define MIN 200000000000000
#define MAX 400000000000000
#endif

typedef struct vec {
    int64_t x, y, z;
} Vec;

static Vec p[N], v[N];

static bool iszero2(const Vec a)
{
    return a.x == 0 && a.y == 0;
}

static bool iszero3(const Vec a)
{
    return a.x == 0 && a.y == 0 && a.z == 0;
}

static bool isequal2(const Vec a, const Vec b)
{
    return a.x == b.x && a.y == b.y;
}

static bool isequal3(const Vec a, const Vec b)
{
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

static int64_t dot2(const Vec a, const Vec b)
{
    return a.x * b.x + a.y * b.y;
}

static int64_t dot3(const Vec a, const Vec b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static double len2(const Vec a)
{
    return sqrt(dot2(a, a));
}

static double len3(const Vec a)
{
    return sqrt(dot3(a, a));
}

static int64_t cross2(const Vec a, const Vec b)
{
    return a.x * b.y - a.y * b.x;
}

static Vec cross3(const Vec a, const Vec b)
{
    return (Vec){
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x};
}

static bool isparallel2(const Vec a, const Vec b)
{
    return cross2(a, b) == 0;
}

static bool isparallel3(const Vec a, const Vec b)
{
    return iszero3(cross3(a, b));
}

static Vec sub3(const Vec a, const Vec b)
{
    return (Vec){a.x - b.x, a.y - b.y, a.z - b.z};
}

// t=0: p0=(x0,y0)
// t=1: p1=(x1,y1) = p0+v = (x0+vx,y0+vy)
// line: (y-y0)(x1-x0) = (y1-y0)(x-x0)
//  => (y-y0)(x0+vx-x0) = (y0+vy-y0)(x-x0)
// <=> (y-y0)vx = vy(x-x0)
// <=> vy.x - vy.x0 - vx.y + vx.y0 = 0
// <=> vy.x - vx.y + (vx.y0 - vy.x0) = 0
//  => U = (vy,-vx,vx.y0 - vy.x0)
static Vec homogenous2(const Vec pos, const Vec vel)
{
    return (Vec){vel.y, -vel.x, vel.x * pos.y - vel.y * pos.x};
}

static void div_r(Vec* )
{

}
int main(void)
{
    FILE* f = fopen(NAME, "r");
    if (!f) { fputs("File not found.", stderr); return 1; }
    for (int i = 0; i < N; ++i)
        fscanf(f, "%lld, %lld, %lld @ %lld, %lld, %lld", &p[i].x, &p[i].y, &p[i].z, &v[i].x, &v[i].y, &v[i].z);
    fclose(f);

    for (int i = 0; i < N; ++i)
        printf("%lld, %lld, %lld @ %lld, %lld, %lld\n", p[i].x, p[i].y, p[i].z, v[i].x, v[i].y, v[i].z);
    printf("\n");

    const Vec min = (Vec){MIN, MIN, MIN};
    const Vec max = (Vec){MAX, MAX, MAX};
    for (int i = 0; i < N - 1; ++i) {
        const Vec hi = homogenous2(p[i], v[i]);
        printf("%lld, %lld, %lld @ %lld, %lld, %lld = (%lld,%lld,%lld)\n", p[i].x, p[i].y, p[i].z, v[i].x, v[i].y, v[i].z, hi.x, hi.y, hi.z);
        for (int j = i + 1; j < N; ++j) {
            const Vec hj = homogenous2(p[j], v[j]);
            printf("\t%lld, %lld, %lld @ %lld, %lld, %lld = (%lld,%lld,%lld) -> ", p[j].x, p[j].y, p[j].z, v[j].x, v[j].y, v[j].z, hj.x, hj.y, hj.z);
            const Vec hs = cross3(hi, hj);
            if (hs.z != 0) {
                printf("(%.2f,%.2f)", (double)hs.x / hs.z, (double)hs.y / hs.z);
            } else
                printf("parallel");
            printf("\n");
        }
    }
    return 0;
}
