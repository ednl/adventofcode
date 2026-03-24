#define SQRT
#define DIFF

#include <stdio.h>
#ifdef SQRT
#include <math.h>
#endif
#include "../startstoptimer.h"

#define P 10
static const int prime[P] = {2,3,5,7,11,13,17,19,23,29};

int main(void)
{
    starttimer();
    const int magic = 65;
    const int step = 17;
    const int start = magic * 100 + 100000;
    const int stop = start + 17000;
    int composite = 0;
    for (int n = start; n <= stop; n += step) {
        for (int i = 0; i < P; ++i)
            if (n % prime[i] == 0) {
                composite++;
                goto next;
            }
    #if defined(SQRT)
        const int root = floor(sqrt(n));
        for (int factor = prime[P - 1] + 2; factor <= root; factor += 2)
    #elif defined(DIFF)
        const int f0 = prime[P - 1] + 2;
        int sq = f0 * f0;
        int df = (f0 + 1) << 2;
        for (int factor = f0; sq <= n; factor += 2, sq += df, df += 8)
    #else
        for (int factor = prime[P - 1] + 2; factor * factor <= n; factor += 2)
    #endif
            if (n % factor == 0) {
                composite++;
                break;
            }
    next:;
    }
    printf("%d\n", composite);  // 917
    printf("Time: %.0f ns\n", stoptimer_ns());
}
