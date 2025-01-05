/**
 * Advent of Code 2024
 * Day 14: Restroom Redoubt
 * https://adventofcode.com/2024/day/14
 * By: E. Dronkert https://github.com/ednl
 *
 * Theory explained by /u/i_have_no_biscuits in
 * https://www.reddit.com/r/adventofcode/comments/1he0asr/2024_day_14_part_2_why_have_fun_with_image/
 *
 * Speed tips by /u/wurlin_murlin in
 * https://www.reddit.com/r/adventofcode/comments/1he0asr/2024_day_14_part_2_why_have_fun_with_image/m237y29/
 * - Avoid costly variance calculation with running mean divisions. Instead use the basic definition.
 * - Avoid storing all calculated positions back into the array on every loop, use local (register?) variable.
 * - Avoid branch to get the non-negative remainder, instead add (positive) period before taking the modulus.
 *
 * Compile:
 *    clang -std=gnu17 -Wall -Wextra 14alt.c
 *    gcc   -std=gnu17 -Wall -Wextra 14alt.c
 * Enable timer:
 *    clang -DTIMER -O3 -march=native 14alt.c ../startstoptimer.c
 *    gcc   -DTIMER -O3 -march=native 14alt.c ../startstoptimer.c
 * Get minimum runtime from timer output:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Mac Mini 2020 (M1 3.2 GHz)                       :  23 µs
 *     Macbook Air 2013 (Core i5 Haswell 4250U 1.3 GHz) :  72 µs
 *     Raspberry Pi 5 (2.4 GHz)                         : 356 µs
 *     Raspberry Pi 4 (1.8 GHz)                         : 531 µs
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

// Find timestep where set of coordinates (either x or y) has minimal variance
static int clustered(const int *const p, const int *const v, const int mod)
{
    int tmin = -1, varmin = INT_MAX;
    for (int t = 0; t < mod; ++t) {
        int sum = 0, sum2 = 0;  // basic definition of variance only has add and mul inside the loop
        // int mean = 0, M2 = 0;  // running mean version also has div and is 270 µs slower on M1
        for (int i = 0; i < N; ++i) {
            const int q = (p[i] + (v[i] + mod) * t) % mod;
            sum += q;
            sum2 += q * q;
            // https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Welford%27s_online_algorithm
            // const int delta = q - mean;
            // mean += delta / (i + 1);
            // M2 += delta * (q - mean);
        }
        const int var = (sum2 - sum * sum / N) / N;
        // const int var =  M2 / N;
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
    if (!f) { fputs("File not found.", stderr); return 1; }
    for (int i = 0; i < N; ++i)
        fscanf(f, "p=%d,%d v=%d,%d ",
            &px[i], &py[i], &vx[i], &vy[i]);
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    // Part 1
    int quad[2][2] = {0};
    for (int i = 0; i < N; ++i) {
        const int qx = (px[i] + (vx[i] + X) * T) % X;
        const int qy = (py[i] + (vy[i] + Y) * T) % Y;
        if (qx != (X >> 1) && qy != (Y >> 1))  // skip central lines as per the puzzle description
            ++quad[qy / ((Y >> 1) + 1)][qx / ((X >> 1) + 1)];
    }
    const int prod = quad[0][0] * quad[0][1] * quad[1][0] * quad[1][1];
    printf("Part 1: %d\n", prod);  // 221655456

    // Part 2
    // x and y are independent
    // period of px+t*vx is t=X
    // period of py+t*vy is t=Y
    // find tx in t=[0,X) where variance of px is minimal
    // find ty in t=[0,Y) where variance of py is minimal
    const int tx = clustered(px, vx, X);
    const int ty = clustered(py, vy, Y);
    // t = tx (mod X) => t = tx + k * X
    // t = ty (mod Y) => tx + k * X = ty (mod Y) => k = invX * (ty - tx) (mod Y)
    // => t = tx + (invX * (ty - tx) (mod Y)) * X
    // where invX (mod Y) = pow(101,-1,103) = 51, because 101 * 51 = 5151 = 1 (mod 103)
    const int t = tx + (INVX * (ty - tx + Y) % Y) * X;  // avoid negative ty-tx
    printf("Part 2: %d\n", t);  // 7858

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
    return 0;
}
