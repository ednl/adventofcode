/**
 * Advent of Code 2022
 * Day 20: Grove Positioning System
 * https://adventofcode.com/2022/day/20
 * By: E. Dronkert https://github.com/ednl
 *
 * Benchmark with the internal timer on a Mac Mini M1 using this Bash oneliner:
 *   m=50000;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * gives a shortest runtime for my input file (not the example) of [TBD] µs.
 * On a Raspberry Pi 4 with the CPU in performance mode: [TBD] µs.
 *   echo performance | sudo tee /sys/devices/system/cpu/cpufreq/policy0/scaling_governor
 *   /boot/config.txt: arm_boost=1, no overclock
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define EXAMPLE 0
#if EXAMPLE == 1
    #define NAME "../aocinput/2022-20-example.txt"
    #define N 7
#else
    #define NAME "../aocinput/2022-20-input.txt"
    #define N 5000
#endif
#define M (N - 1)

typedef struct link {
    int prev, next;
} Link;

typedef struct node {
    int64_t val;
    int shift, prev, next;
} Node;

static Node ring[N];

#if EXAMPLE
static void show(void)
{
    printf("%2d", ring[0].val);
    for (int i = ring[0].next; i; i = ring[i].next)
        printf(",%2d", ring[i].val);
    putchar('\n');
}
#endif

static int closest(const int64_t x)
{
    const int a = x % M;
    if (a >= 0) {
        const int b = a - M;
        return a <= -b ? a : b;
    }
    const int b = a + M;
    return b <= -a ? b : a;
}

static Link walk(int index, int steps)
{
    if (steps < 0) {
        do {
            index = ring[index].prev;
        } while (++steps);
        return (Link){ring[index].prev, index};
    }
    if (steps > 0) {
        do {
            index = ring[index].next;
        } while (--steps);
        return (Link){index, ring[index].next};
    }
    return (Link){ring[index].prev, ring[index].next};
}

int main(void)
{
    FILE *f = fopen(NAME, "r");
    if (!f)
        return 1;
    char buf[8];
    int zero = -1;
    for (int i = 0; i < N && fgets(buf, sizeof buf, f); ++i) {
        const int val = atoi(buf);
        if (!val)
            zero = i;
        ring[i] = (Node){val, closest(val), i - 1, i + 1};
    }
    fclose(f);
    if (zero < 0)
        return 2;
    ring[0].prev = M;
    ring[M].next = 0;
#if EXAMPLE
    show();
#endif

    for (int i = 0; i < N; ++i)
        if (ring[i].shift) {
            const Link link = walk(i, ring[i].shift);
            ring[ring[i].prev].next = ring[i].next;
            ring[ring[i].next].prev = ring[i].prev;
            ring[link.prev].next = i;
            ring[link.next].prev = i;
            ring[i].prev = link.prev;
            ring[i].next = link.next;
#if EXAMPLE
            show();
#endif
        }

    int sum = 0;
    const int a = 1000 % N;
    const int b = a - N;
    const int step = a <= -b ? a : b;
    for (int i = 0; i < 3; ++i) {
        zero = step > 0 ? (walk(zero, step)).prev : (walk(zero, step)).next;
        sum += ring[zero].val;
    }
    printf("Part 1: %d\n", sum);  // example=4-3+2=3, input=?
    return 0;
}
