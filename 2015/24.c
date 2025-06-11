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
 *     Mac Mini 2020 (M1 3.2 GHz)                       : 2.49 ms
 *     Macbook Air 2013 (Core i5 Haswell 4250U 1.3 GHz) : 7.08 ms
 *     Raspberry Pi 5 (2.4 GHz)                         : 7.69 ms
 */

#include <stdio.h>
#include <stdint.h>       // int64_t
#include <inttypes.h>     // PRId64
#include "../combperm.h"  // combinations
#include "../startstoptimer.h"

#define FNAME "../aocinput/2015-24-input.txt"
#define LINES 30  // actual lines in my input file = 29

typedef struct data {
    int weight[LINES];
    int len, sum;
} Data;
static Data data;

static int readinput(void)
{
    FILE *f = fopen(FNAME, "r");
    if (!f)
        return 0;
    int len = 0, sum = 0;
    for (int x; len < LINES && fscanf(f, "%d", &x) == 1; ++len) {
        data.weight[len] = x;
        sum += x;  // cheat by not timing the sum of all weights
    }
    fclose(f);
    data.len = len;
    data.sum = sum;
    return len;
}

static int64_t quantum(const int groups)
{
    // Assumes: groups > 0 && sum % groups == 0
    const int groupweight = data.sum / groups;
    // Assumes input is sorted from small to big
    int minlen = 0;
    for (int weight = 0, i = data.len - 1; weight < groupweight; --i, ++minlen)
        weight += data.weight[i];  // add from end (start with biggest)
    // Assumes unique solution exists
    int64_t min_qe = 0;
    for (int *index; !min_qe; ++minlen)
        while ((index = combinations(data.len, minlen))) {
            int weight = 0;
            int64_t qe = 1;
            for (int i = 0; i < minlen; ++i) {
                weight += data.weight[index[i]];  // sum
                qe *= data.weight[index[i]];  // product
            }
            if (weight == groupweight && (qe < min_qe || !min_qe))
                min_qe = qe;
        }
    return min_qe;
}

int main(void)
{
    if (!readinput())
        return 1;  // error
    starttimer();  // exclude disk read (and sum)
    printf("%"PRId64" %"PRId64"\n", quantum(3), quantum(4));  // 10723906903 74850409
    printf("Time: %.0f us\n", stoptimer_us());
    return 0;
}
