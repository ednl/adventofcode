/**
 * Advent of Code 2024
 * Day 14: Restroom Redoubt
 * https://adventofcode.com/2024/day/14
 * By: E. Dronkert https://github.com/ednl
 *
 * Guided by /u/i_have_no_biscuits as layed out in
 * https://www.reddit.com/r/adventofcode/comments/1he0asr/2024_day_14_part_2_why_have_fun_with_image/
 *
 * Compile:
 *    clang -std=gnu17 -Wall -Wextra 14.c
 *    gcc   -std=gnu17 -Wall -Wextra 14.c
 * Enable timer:
 *    clang -DTIMER -O3 -march=native 14.c ../startstoptimer.c
 *    gcc   -DTIMER -O3 -march=native 14.c ../startstoptimer.c
 * Get minimum runtime from timer output:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Mac Mini 2020 (M1 3.2 GHz)                       :  ? µs
 *     Raspberry Pi 5 (2.4 GHz)                         :  ? µs
 *     Macbook Air 2013 (Core i5 Haswell 4250U 1.3 GHz) : 72 µs
 *     Raspberry Pi 4 (1.8 GHz)                         :  ? µs
 */

#include <stdio.h>
#include <limits.h>  // INT_MAX
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define N 500  // robots
#define X 101  // width
#define Y 103  // height
#define T 100  // part 1: 100 seconds

// Part 2: inv{a} (mod m) = b, if a*b = 1 (mod m)
// inv{101} (mod 103) = 51, because 101*51 = 5151 = 1 (mod 103)
// https://en.wikipedia.org/wiki/Modular_multiplicative_inverse
#define INVX 51

static int px[N];  // x-position
static int py[N];  // y-position
static int vx[N];  // x-velocity
static int vy[N];  // y-velocity

// Find timestep where set of coordinates (either x or y) has minimal variance.
static int best(const int *const p, const int *const v, const int mod)
{
    int tmin = -1, varmin = INT_MAX;
    for (int t = 0; t < mod; ++t) {
        int sum = 0, sum2 = 0;
        for (int i = 0; i < N; ++i) {
            const int q = (p[i] + (v[i] + mod) * t) % mod;
            sum += q;
            sum2 += q * q;
        }
        const int var = (sum2 - sum * sum / N) / N;
        if (var < varmin) {
            varmin = var;
            tmin = t;
        }
    }
    return tmin;
}

int main(void)
{
    FILE *f = fopen("../aocinput/2024-14-input.txt", "r");
    for (int i = 0; i < N; ++i)
        fscanf(f, "p=%d,%d v=%d,%d ",
            &px[i], &py[i], &vx[i], &vy[i]);
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    // Part 1
    union {
        int a[4];     // access as 1D array
        int m[2][2];  // access as 2D array (matrix)
    } quad = {0};
    for (int i = 0; i < N; ++i) {
        const int qx = (px[i] + (vx[i] + X) * T) % X;
        const int qy = (py[i] + (vy[i] + Y) * T) % Y;
        if (qx != (X >> 1) && qy != (Y >> 1))  // skip central lines
            ++quad.m[qy / ((Y >> 1) + 1)][qx / ((X >> 1) + 1)];
    }
    const int prod = quad.a[0] * quad.a[1] * quad.a[2] * quad.a[3];
    printf("Part 1: %d\n", prod);  // 221655456

    // Part 2
    // (this part first because positions get reset to start)
    // x and y are independent
    // period of px+t*vx is t=X
    // period of py+t*vy is t=Y
    // find tx in t=[0,X) where variance of px is minimal (= x-coordinates clustered)
    // find ty in t=[0,Y) where variance of py is minimal (= y-coordinates clustered)
    const int tx = best(px, vx, X);
    const int ty = best(py, vy, Y);
    // t = tx (mod X) => t = tx + k * X
    // t = ty (mod Y) => tx + k * X = ty (mod Y) => k = invX * (ty - tx) (mod Y)
    // => t = tx + (invX * (ty - tx) (mod Y)) * X
    // where invX (mod Y) = pow(101,-1,103) = 51, because 101 * 51 = 5151 = 1 (mod 103)
    const int t = tx + (INVX * (ty - tx + Y) % Y) * X;  // tx may be greater than ty
    printf("Part 2: %d\n", t);  // 7858

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
    return 0;
}
