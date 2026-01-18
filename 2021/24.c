/**
 * Advent of Code 2021
 * Day 24: Arithmetic Logic Unit
 * https://adventofcode.com/2021/day/24
 * By: E. Dronkert https://github.com/ednl
 *
 * Validate 14-digit numbers where each digit is from 1 to 9.
 * Part 1: find largest valid number.
 * Part 2: find smallest valid number.
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 24.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 24.c
 * Get minimum runtime from timer output in bash:
 *     m=9999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 13 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 22 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :  ? µs
 */

#include <stdio.h>
#include <stdlib.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2021-24-input.txt"
#define N 14
#define M (N >> 1)

static int rule[N];
static int index[M];
static int value[M];
char max[N + 1];  // part 1
char min[N + 1];  // part 2

// Manually translated from input file:
// int z = w[0] + 2;          // (w0+2)
// z = z * 26 + w[1] + 13;    // (w0+2)(w1+13)
// z = z * 26 + w[2] + 13;    // (w0+2)(w1+13)(w2+13)
// if (z % 26 - 13 == w[3])   // w2 + 13 - 13 == w3
//     z /= 26;               // (w0+2)(w1+13)
// z = z * 26 + w[4] + 15;    // (w0+2)(w1+13)(w4+15)
// if (z % 26 - 13 == w[5])   // w4 + 15 - 13 == w5
//     z /= 26;               // (w0+2)(w1+13)
// if (z % 26 - 7 == w[6])    // w1 + 13 - 7 == w6
//     z /= 26;               // (w0+2)
// z = z * 26 + w[7] + 5;     // (w0+2)(w7+5)
// z = z * 26 + w[8] + 16;    // (w0+2)(w7+5)(w8+16)
// z = z * 26 + w[9] + 1;     // (w0+2)(w7+5)(w8+16)(w9+1)
// if (z % 26 - 4 == w[10])   // w9 + 1 - 4 == w10
//     z /= 26;               // (w0+2)(w7+5)(w8+16)
// if (z % 26 - 9 == w[11])   // w8 + 16 - 9 == w11
//     z /= 26;               // (w0+2)(w7+5)
// if (z % 26 - 13 == w[12])  // w7 + 5 - 13 == w12
//     z /= 26;               // (w0+2)
// if (z % 26 - 9 == w[13])   // w0 + 2 - 9 == w13
//     z /= 26;               // (0)
// return z;

// Rule summary:
// w2 + 13 - 13 == w3    =>   w2     == w3
// w4 + 15 - 13 == w5    =>   w4 + 2 == w5
// w1 + 13 -  7 == w6    =>   w1 + 6 == w6
// w9 +  1 -  4 == w10   =>   w9 - 3 == w10
// w8 + 16 -  9 == w11   =>   w8 + 7 == w11
// w7 +  5 - 13 == w12   =>   w7 - 8 == w12
// w0 +  2 -  9 == w13   =>   w0 - 7 == w13

//  index: 01234567890123
// Part 1: 93997999296912 (largest)
// Part 2: 81111379141811 (smallest)

int main(void)
{
#ifdef TIMER
    starttimer();
#endif

    FILE *f = fopen(FNAME, "r");
    if (!f)
        return 1;
    char buf[16];  // very short lines in input file
    for (int i = 0, add = 0; i < N && fgets(buf, sizeof buf, f); )
        if (buf[0] == 'i')  // new digit
            add = 0;        // counter reset
        else if (add < 4 && buf[0] == 'a' && (buf[4] == 'x' || buf[4] == 'y') && buf[6] < 'w') {
            // For every digit (under "inp w") there are 4 lines "add [xy] [0-9-]+".
            // If the first is negative, put it in the rules.
            // Otherwise, put the 4th in the rules.
            if ((add == 0 && buf[6] == '-') || add == 3) {
                rule[i++] = atoi(&buf[6]);
                continue;
            }
            ++add;
        }
    fclose(f);

    // My input:
    // rule[] = {2, 13, 13, -13, 15, -13, -7, 5, 16, 1, -4, -9, -13, -9};

    for (int i = 0, sp = 0; i < N; ++i) {
        if (rule[i] > 0) {
            index[sp] = i;
            value[sp++] = rule[i];
        } else {
            const int j = index[--sp];
            const int k = value[sp] + rule[i];
            // num[j] + k == num[i];
            if (k >= 0) {
                max[j] = '9' - k;
                max[i] = '9';
                min[j] = '1';
                min[i] = '1' + k;
            } else {
                max[j] = '9';
                max[i] = '9' + k;
                min[j] = '1' - k;
                min[i] = '1';
            }
        }
    }
    printf("Part 1: %s\n", max);  // 93997999296912
    printf("Part 2: %s\n", min);  // 81111379141811

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
}
