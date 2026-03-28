/**
 * Advent of Code 2015
 * Day 11: Corporate Policy
 * https://adventofcode.com/2015/day/11
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 11.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 11.c
 * Get minimum runtime from timer output in bash:
 *     m=9999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :    ? ms
 *     Mac Mini 2020 (M1 3.2 GHz)    : 5.04 ms
 *     Raspberry Pi 5 (2.4 GHz)      :    ? ms
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "../startstoptimer.h"

#define INPUT "hxbxwxba"
#define LEN 8

static const char digits[] = "abcdefghjkmnpqrstuvwxyz";
static const int64_t base = sizeof digits - 1;

static int64_t toval(const char pwd[static const LEN])
{
    int64_t val = 0;
    for (int i = 0; i < LEN; ++i) {
        int unit = pwd[i] - 'a';
        if (unit > 'o' - 'a')
            unit -= 3;
        else if (unit > 'l' - 'a')
            unit -= 2;
        else if (unit > 'i' - 'a')
            unit -= 1;
        val = val * base + unit;
    }
    return val;
}

static void tostr(char pwd[static const LEN], int64_t val)
{
    for (int i = LEN - 1; i >= 0; --i) {
        pwd[i] = digits[val % base];
        val /= base;
    }
}

static char next(const char a)
{
    const char b = a + 1;
    return b == 'i' || b == 'l' || b == 'o' ? b + 1 : b;
}

static bool isvalid(const char pwd[static const LEN])
{
    for (int i = 0, k = 0; i < LEN - 1; ++i)
        if (pwd[i] == pwd[i + 1]) {
            if (!k)
                k = pwd[i++];
            else if (k ^ pwd[i++])
                goto triplet;
        }
    return false;
triplet:
    for (int i = 0; i < LEN - 2; ++i)
        if (pwd[i] <= 'x' && next(pwd[i]) == pwd[i + 1] && next(pwd[i + 1]) == pwd[i + 2])
            return true;
    return false;
}

int main(void)
{
    starttimer();
    char pwd[] = INPUT;
    int64_t n = toval(pwd);
    for (int i = 1; i <= 2; ++i) {
        do {
            tostr(pwd, ++n);
        } while (!isvalid(pwd));
        printf("Part %d: %s\n", i, pwd);  // hxbxxyzz, hxcaabcc
    }
    printf("Time: %.0f us\n", stoptimer_us());
    return 0;
}
