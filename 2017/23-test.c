#include <stdio.h>
#define SQRT
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
    #ifdef SQRT
        const int r = floor(sqrt(n));
        for (int d = prime[P - 1] + 2; d <= r; d += 2)
    #else
        for (int d = prime[P - 1] + 2; d * d <= n; d += 2)
    #endif
            if (n % d == 0) {
                composite++;
                break;
            }
    next:;
    }
    printf("%d\n", composite);  // 917
    printf("Time: %.0f ns\n", stoptimer_ns());
}
