/**
 * M4: 8.78 µs
 */
#include <stdio.h>
#include <stdint.h>  // uint64_t
#include <inttypes.h>  // PRIu64
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define LEN 1000
#define PRE 25

static uint64_t data[LEN];

static bool valid(const int k)
{
    for (int i = k - PRE; i < k - 1; ++i) {
        const uint64_t res = data[k] - data[i];
        for (int j = i + 1; j < k; ++j)
            if (data[j] == res)
                return true;
    }
    return false;
}

int main(void)
{
    FILE *f = fopen("../aocinput/2020-09-input.txt", "r");
    if (!f)
        return 1;
    for (int i = 0; i < LEN; ++i)
        fscanf(f, "%"PRIu64, &data[i]);
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    int k = PRE;
    while (valid(k))
        ++k;
    printf("%"PRIu64, data[k]);  // 15353384

    int i = 0, j = 0;
    uint64_t sum = data[i];
    while (sum != data[k]) {
        while (sum < data[k])
            sum += data[++j];
        while (sum > data[k])
            sum -= data[i++];
    }
    uint64_t min = data[i], max = data[i];
    for (++i; i <= j; ++i) {
        if (data[i] < min) min = data[i];
        if (data[i] > max) max = data[i];
    }
    printf(" %"PRIu64"\n", min + max);  // 2466556

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
