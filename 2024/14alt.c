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
 *     Mac Mini 2020 (M1 3.2 GHz)                       :  319 µs
 *     Raspberry Pi 5 (2.4 GHz)                         :  818 µs
 *     Macbook Air 2013 (Core i5 Haswell 4250U 1.3 GHz) : 1049 µs
 *     Raspberry Pi 4 (1.8 GHz)                         : 1399 µs
 */

#include <stdio.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define N 500  // robots
#define X 101  // width
#define Y 103  // height
#define T 100  // part 1: 100 seconds

static int px[N];  // x-position
static int py[N];  // y-position
static int vx[N];  // x-velocity
static int vy[N];  // y-velocity

// Non-negative remainder (for positive m)
static int mod(const int a, const int m)
{
    const int b = a % m;
    return b >= 0 ? b : b + m;
}

// https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Welford%27s_online_algorithm
static int var(const int *const p)
{
    int mean = 0, M2 = 0;
    for (int i = 0; i < N; ++i) {
        // Update running mean and unscaled variance
        const int delta = p[i] - mean;
        mean += delta / (i + 1);
        M2 += delta * (p[i] - mean);
    }
    // Scale as population variance (= exact variance of given data)
    return M2 / N;
}

// Find timestep where set of coordinates (either x or y) has minimal variance.
static int best(int *const p, const int *const v, const int len)
{
    int tmin = 0, varmin = var(p);
    for (int t = 0; t < len; ++t) {  // 'len' steps = reset to start
        for (int i = 0; i < N; ++i)
            p[i] = mod(p[i] + v[i], len);
        const int cur = var(p);
        if (cur < varmin) {
            varmin = cur;
            tmin = t + 1;  // t=0: 1 step done
        }
    }
    return tmin;
}

// https://en.wikipedia.org/wiki/Modular_multiplicative_inverse
// b = inv(a) (mod m) if a*b (mod m) = 1
static int inv(const int a, const int m)
{
    for (int b = 2; b < m; ++b)
        if (a * b % m == 1)
            return b;
    return 0;
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

    // Part 2
    // (this part first because positions get reset to start)
    // x and y are independent
    // period of px+t*vx is t=X
    // period of py+t*vy is t=Y
    // find tx in t=[0,X) where variance of px is minimal (= x-coordinates clustered)
    // find ty in t=[0,Y) where variance of py is minimal (= y-coordinates clustered)
    const int tx = best(px, vx, X);
    const int ty = best(py, vy, Y);
    // t = tx (mod X) => t = tx+k*X
    // t = ty (mod Y) => tx+k*X = ty (mod Y) => k = invX*(ty-tx) (mod Y)
    // => t = tx+invX*(ty-tx)*X (mod X*Y)
    //   where invX (mod Y) = pow(101,-1,103) = 51, because 101*51 = 5151 = 1 (mod 103)
    const int invX = inv(101, 103);  // 51
    const int t = mod(tx + invX * (ty - tx) * X, X * Y);
    printf("Part 2: %d\n", t);  // 7858

    // Part 1
    union {
        int a[4];     // access as 1D array
        int m[2][2];  // access as 2D array (matrix)
    } quad = {0};
    for (int i = 0; i < N; ++i) {
        px[i] = mod(px[i] + vx[i] * T, X);  // 100 time steps in one
        py[i] = mod(py[i] + vy[i] * T, Y);
        if (px[i] != (X >> 1) && py[i] != (Y >> 1))  // skip central lines
            ++quad.m[py[i] / ((Y >> 1) + 1)][px[i] / ((X >> 1) + 1)];
    }
    const int prod = quad.a[0] * quad.a[1] * quad.a[2] * quad.a[3];
    printf("Part 1: %d\n", prod);  // 221655456

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
    return 0;
}
