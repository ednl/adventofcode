// Runtime is 35.3 s on a 2013 iMac (3.2 GHz quad-core i5 Haswell 4570)
// so the actual number (1000x greater) would take about 10 hours.
#include <stdio.h>
#include <stdint.h>

int main(void)
{
    volatile int64_t n = 21003205388;//413;
    while (n--);
    return 0;
}
