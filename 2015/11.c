/**
 * Advent of Code 2015
 * Day 11: Corporate Policy
 * https://adventofcode.com/2015/day/11
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -O3 -march=native -Wall -Wextra 11.c ../startstoptimer.c
 *    gcc   -std=gnu17 -O3 -march=native -Wall -Wextra 11.c ../startstoptimer.c
 * Get minimum runtime:
 *     m=999999;for((i=0;i<1000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * Minimum runtime:
 *     Mac Mini 2020 (M1 3.2 GHz)                       :  ? ms
 *     Macbook Air 2013 (Core i5 Haswell 4250U 1.3 GHz) :  ? ms
 *     iMac 2013 (Core i5 Haswell 4570 3.2 GHz)         : 11 ms
 *     Raspberry Pi 5 (2.4 GHz)                         :  ? ms
 *     Raspberry Pi 4 (1.8 GHz)                         :  ? ms
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
    bool twopair = false;
    for (int i = 0, k = 0; i < LEN - 1; ++i)
        if (pwd[i] == pwd[i + 1]) {
            if (!k)
                k = pwd[i++];
            else if ((twopair = k ^ pwd[i++]))
                break;
        }
    if (!twopair)
        return false;
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
        printf("Part %d: %s\n", i, pwd);
    }
    printf("Time: %.0f ms\n", stoptimer_ms());
    return 0;
}
