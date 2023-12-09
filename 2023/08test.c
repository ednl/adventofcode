// Advent of Code 2023 Day 8 Part 2
// Optimistic estimate for the runtime of a brute-force solution
// -----------------------------------------------------------------
// Answer to part 2 for my input is 21,003,205,388,413 = 21 trillion
// and change. How long would it take to simply count that?
// This C program counts down from 21003205388 (last three digits
// cut off) to zero. The "volatile" keyword ensures a full run.
//
// Runtime is 35.3 s on a 2013 iMac (3.2 GHz Intel i5 Haswell 4570)
// so the actual answer (1000x greater) would take about 10 hours.
// On a Raspberry Pi 4 at 1.8 GHz it takes 1m33.4s x1000 = 26 hours.

#include <stdio.h>
#include <stdint.h>

int main(void)
{
    // Actual answer for my input has 413 appended,
    // so is about 1000x greater.
    volatile int64_t n = 21003205388; //..413;
    while (n--); // count down to zero
    return 0;
}
