#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "startstoptimer.h"

#define TX0 (137)
#define TX1 (171)
#define TY0 (-98)
#define TY1 (-73)

static int dist(int v0, int t)
{
    return v0*t - t*(t - 1)/2;
}

static int maxdist(int v0)
{
    if (v0 <= 0)
        return 0;
    return dist(v0, v0);
}

static int height(int vy0, int t)
{
    if (t <= 0)
        return 0;
    return dist(vy0, t);
}

static int width(int vx0, int t)
{
    if (t <= 0)
        return 0;
    if (t >= vx0)
        return maxdist(vx0);
    return dist(vx0, t);
}

// y[t] = vy0*t - t*(t-1)/2 >= TY0
// t^2 - (2*vy0 + 1)*t + 2*TY0 <= 0
// t1,2 = vy0 + 0.5 ± sqrt[(vy0 + 0.5)^2 - 2*TY0]
// => t <= vy0 + 0.5 + sqrt[(vy0 + 0.5)^2 - 2*TY0]

// y[t] = vy0*t - t*(t-1)/2 <= TY1
// t^2 - (2*vy0 + 1)*t + 2*TY1 >= 0
// t1,2 = vy0 + 0.5 ± sqrt[(vy0 + 0.5)^2 - 2*TY1]
// => t >= vy0 + 0.5 + sqrt[(vy0 + 0.5)^2 - 2*TY1]

// maxdist[v] = v*v - v*(v - 1)/2
// => v*v + v - 2*137 = 0
// => v = (-1 ± sqrt[1 + 8*137])/2
// => v = sqrt[2*137 + 0.25] - 0.5 = 16.06 = 17

static double timebound(int v, int b)
{
    double f = 0.5 + v, g = f*f - 2*b;
    if (g >= 0)
        return sqrt(g) + f;
    return (double)NAN;
}

int main(void)
{
    starttimer();
    int ymax = 0;
    int hits = 0;
    const int minvx0 = ceil(sqrt(2*TX0 + 0.25) - 0.5);  // see above
    for (int vy0 = TY0; vy0 <= -TY0; ++vy0) {
        const int t1 = (int)ceil(timebound(vy0, TY1));
        const int t2 = (int)floor(timebound(vy0, TY0));
        for (int vx0 = minvx0; vx0 <= TX1; ++vx0) {
            bool hit = false;
            for (int t = t1, x, y; t <= t2 && (y = height(vy0, t)) >= TY0 && (x = width(vx0, t)) <= TX1; ++t) {
                if (y <= TY1) {
                    int ym = maxdist(vy0);
                    if (ym > ymax)
                        ymax = ym;
                    if (x >= TX0 && !hit) {
                        hit = true;
                        ++hits;
                    }
                }
            }
        }
    }
    printf("Part 1: %d\n", ymax);  // 4753
    printf("Part 2: %d\n", hits);  // 1546
    printf("%.0f µs\n", stoptimer_us());
    return 0;
}
