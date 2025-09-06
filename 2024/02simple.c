/**
 * Advent of Code 2024
 * Day 2: Red-Nosed Reports
 * https://adventofcode.com/2024/day/2
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -Wall -Wextra 02simple.c
 *    gcc   -std=gnu17 -Wall -Wextra 02simple.c
 * Enable timer:
 *    clang -DTIMER -O3 -march=native 02simple.c ../startstoptimer.c
 *    gcc   -DTIMER -O3 -march=native 02simple.c ../startstoptimer.c
 * Get minimum runtime from timer output:
 *     m=999999;for((i=0;i<5000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz)                    :   ? µs
 *     Mac Mini 2020 (M1 3.2 GHz)                       : 300 µs
 *     Raspberry Pi 5 (2.4 GHz)                         :   ? µs
 *     iMac 2013 (Core i5 Haswell 4570 3.2 GHz)         :   ? µs
 *     Macbook Air 2013 (Core i5 Haswell 4250U 1.3 GHz) :   ? µs
 *     Raspberry Pi 4 (1.8 GHz)                         :   ? µs
 */

#include <stdio.h>  // fopen, fclose, fgets, sscanf, printf
#include <ctype.h>  // isdigit, isspace
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME   "../aocinput/2024-02-input.txt"
#define MINDIST 1
#define MAXDIST 3

// Safety check for continuous array part.
// first may be equal to last which means length is 1 (=safe)
// function fails if last can't be reached from first by adding step
static bool issafe(const char *const a, const int first, const int last, const int step)
{
    for (int i = first; i != last; i += step) {
        const int gap = a[i + step] - a[i];
        if (gap < MINDIST || gap > MAXDIST)
            return false;
    }
    return true;
}

// Safety check for both part 1 and 2.
// Return: 0=not safe, 1=safe for part 1 or 2, 2=safe for part 2 only
// len is always >=2
static int dampener(const char *const a, const int len)
{
    // Part 1
    if (issafe(a, 0, len - 1, 1) || issafe(a, len - 1, 0, -1))
        return 1;
    // Part 2, remove first element
    if (issafe(a, 1, len - 1, 1) || issafe(a, len - 1, 1, -1))
        return 2;
    // Part 2, remove last element
    if (issafe(a, 0, len - 2, 1) || issafe(a, len - 2, 0, -1))
        return 2;
    // Part 2, remove element in the middle
    for (int i = 1; i < len - 1; ++i)
        if ((issafe(a, 0, i - 1, 1) && issafe(a, i - 1, i + 1, 2) && issafe(a, i + 1, len - 1, 1)) ||
            (issafe(a, len - 1, i + 1, -1) && issafe(a, i + 1, i - 1, -2) && issafe(a, i - 1, 0, -1)))
            return 2;
    return 0;
}

int main(void)
{
#ifdef TIMER
    starttimer();
#endif

    FILE *f = fopen(FNAME, "r");
    if (!f) { fputs("File not found.\n", stderr); return 1; }
    int safe = 0, damp = 0;
    char *s, report[BUFSIZ];  // BUFSIZ is defined in stdio, probably as 1024
    while ((s = fgets(report, sizeof report, f))) {  // get one report (= one line from input file)
        // Parse "levels" in report, all values in input file are between 0 and 100
        int level, len = 0;
        while (sscanf(s, "%d", &level) == 1) {  // read one number
            while (isdigit(*s)) ++s;  // skip all digits
            while (isspace(*s)) ++s;  // skip all spaces (or newline)
            // 's' now points to either the next number or the string terminator '\0'
            report[len++] = level & 127;  // save value 0<=level<128 to char array, increment len
        }
        const int check = dampener(report, len);
        safe += check == 1;
        damp += check != 0;
    }
    fclose(f);
    printf("%d %d\n", safe, damp);

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
    return 0;
}
