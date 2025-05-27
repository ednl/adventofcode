/**
 * Advent of Code 2015
 * Day 24: It Hangs in the Balance
 * https://adventofcode.com/2015/day/24
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -O3 -march=native -Wall -Wextra 24.c ../combperm.c ../startstoptimer.c
 *    gcc   -std=gnu17 -O3 -march=native -Wall -Wextra 24.c ../combperm.c ../startstoptimer.c
 * Get minimum runtime:
 *     m=999999;for((i=0;i<2000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * Minimum runtime:
 *     Mac Mini 2020 (M1 3.2 GHz)                       : 2.54 ms
 *     Raspberry Pi 5 (2.4 GHz)                         :    ? ms
 */

#include <stdio.h>
#include <stdint.h>
#include "../combperm.h"
#include "../startstoptimer.h"

#define FNAME "../aocinput/2015-24-input.txt"
#define N 32  // actual lines in my input file = 29

static int data[N];

static int readinput(void)
{
    FILE *f = fopen(FNAME, "r");
    if (!f)
        return 0;
    int n = 0, x;
    while (n < N && fscanf(f, "%d", &x) == 1)
        data[n++] = x;
    fclose(f);
    // Reverse input from big to small
    for (int i = 0, j = n - 1; i < j; ++i, --j) {
        int tmp = data[i];
        data[i] = data[j];
        data[j] = tmp;
    }
    return n;
}

static int64_t quantum(const int len, const int sum, const int groups)
{
    if (len <= 0 || sum <= 0 || groups <= 0)
        return 0;
    const int groupweight = sum / groups;
    int minlen = 0, maxlen = len;
    for (int weight = 0; weight < groupweight && minlen < len; ++minlen)
        weight += data[minlen];
    for (int weight = sum; weight > groupweight && maxlen > 0; --maxlen)
        weight -= data[len - maxlen];
    int64_t min_qe = 0;
    for (int *index; !min_qe && minlen <= maxlen; ++minlen) {
        while ((index = combinations(len, minlen))) {
            int weight = 0;
            for (int i = 0; i < minlen; ++i)
                weight += data[index[i]];  // sum
            if (weight == groupweight) {
                int64_t qe = 1;
                for (int i = 0; i < minlen; ++i)
                    qe *= data[index[i]];  // product
                if (qe < min_qe || !min_qe)
                    min_qe = qe;
            }
        }
    }
    return min_qe;
}

int main(void)
{
    starttimer();
    int len = readinput(), sum = 0;
    for (int i = 0; i < len; ++i)
        sum += data[i];
    printf("%lld %lld\n", quantum(len, sum, 3), quantum(len, sum, 4));
    printf("Time: %.0f us\n", stoptimer_us());
    return 0;
}
