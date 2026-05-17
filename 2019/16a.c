/**
 * Advent of Code 2019
 * Day 16: Flawed Frequency Transmission, part 1
 * https://adventofcode.com/2019/day/16
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 16a.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 16a.c
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 2.84 ms
 *     Mac Mini 2020 (M1 3.2 GHz)    :    ? ms
 *     Raspberry Pi 5 (2.4 GHz)      :    ? ms
 */

#include <stdio.h>   // fopen, fgetc, printf
#include <stdlib.h>  // abs
#include <ctype.h>   // isdigit
#ifdef TIMER
#include "../startstoptimer.h"
#endif

#define MAXLEN 1500

static int finddigits(const char * const s, int *a)
{
    FILE *fp;
    const char * pc;
    int c, i = 0;

    if ((fp = fopen(s, "r")) != NULL)
    {
        while (i < MAXLEN && (c = fgetc(fp)) != EOF)
            if (isdigit(c))
                a[i++] = c - '0';
        fclose(fp);
    } else
    {
        pc = s;
        while (i < MAXLEN && (c = *pc++))
            if (isdigit(c))
                a[i++] = c - '0';
    }
    return i;
}

static void show(int *a)
{
    int i;

    for (i = 0; i < 8; ++i)
        printf("%d", a[i]);
    printf("\n");
}

int main(void)
{
    // char *data = "12345678";  // 4: 01029498
    // char *data = "80871224585914546619083218645595";  // 100: 24176176
    // char *data = "19617804207202209144916044189917";  // 100: 73745418
    // char *data = "69317163492948606335995924319873";  // 100: 52432133
    char *data = "../aocinput/2019-16-input.txt";  // 100: 25131128

    int i, j, k, phase, sum, len, third, half, start, step, offset = 0;
    int vec[MAXLEN];

    if (!(len = finddigits(data, vec)))
        return 1;

#ifdef TIMER
    starttimer();
#endif

    for (i = 0; i < 7; ++i)
        offset = offset * 10 + vec[i];
    // printf("%07d\n", offset);

    third = len / 3;
    half = len / 2;

    // 100 Repeated phases
    for (phase = 0; phase < 100; ++phase)
    {
        // Only add and subtract, skip the zeros
        start = -1;
        step = 0;
        for (i = 0; i < third; ++i)
        {
            start += 3;
            step += 4;
            sum = 0;
            for (k = 0; k <= i; ++k)
            {
                for (j = i + k; j < len; j += step)
                    sum += vec[j];
                for (j = start + k; j < len; j += step)
                    sum -= vec[j];
            }
            vec[i] = abs(sum) % 10;
        }
        // Only adding from here
        for (; i < half; ++i)
        {
            for (j = i + 1; j <= i * 2; ++j)
                vec[i] += vec[j];
            vec[i] %= 10;
        }
        // From here i * 2 >= len
        for (; i < len - 1; ++i)
        {
            for (j = i + 1; j < len; ++j)
                vec[i] += vec[j];
            vec[i] %= 10;
        }
    }
    show(vec);  // 25131128

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
}
