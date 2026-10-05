/**
 * Advent of Code 2024
 * Day 5: Print Queue
 * https://adventofcode.com/2024/day/5
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 05.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 05.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  4.5 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :    ? µs
 *     Raspberry Pi 5 (2.4 GHz)      :    ? µs
 */

#include <stdio.h>
#include <unistd.h>  // isatty, fileno
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2024-05-input.txt"
#define FSIZE ((1<<14)|(1<<8))  // 16640, needed for my input: 16408
#define UPDATES 209
#define PAGES   23

static char input[FSIZE];
static bool rule[100][100];       // rule[a][b] is true when a comes before b
static int page[UPDATES][PAGES];  // every page of every "update"
static int pagecount[UPDATES];    // actual number of pages in this "update"

static void swap(int *const a, int *const b)
{
    const int tmp = *a;
    *a = *b;
    *b = tmp;
}

// Standard partition process of QuickSort
// Take last element as pivot, moves smaller to the left of it
static int partition(int *const arr, const int l, const int r)
{
    const int x = arr[r];
    int i = l;
    for (int j = l; j < r; ++j)
        if (rule[arr[j]][x])  // order by the rules
            swap(&arr[i++], &arr[j]);
    swap(&arr[i], &arr[r]);
    return i;
}

// https://www.geeksforgeeks.org/dsa/quickselect-algorithm/
// but k is zero-based
static int sortedindex(int *const arr, const int l, const int r, const int k)
{
    // Partition the array around the last
    // element and get the position of the pivot
    // element in the sorted array.
    const int index = partition(arr, l, r);

    // If position is the same as k
    if (index - l == k)
        return arr[index];

    // If position is more, recur for the left subarray
    if (index - l > k)
        return sortedindex(arr, l, index - 1, k);

    // Else recur for the right subarray
    return sortedindex(arr, index + 1, r, k - index + l - 1);
}

// Parse 2-digit number
static int readnum(const char *const s)
{
    return *s * 10 + *(s + 1) - '0' * 11;
}

int main(void)
{
    if (isatty(fileno(stdin))) {
        // Read input file from disk
        FILE *f = fopen(FNAME, "rb");  // fread() requires binary mode
        if (!f) { fputs("File not found: "FNAME, stderr); return 1; }
        fread(input, 1, sizeof input, f);  // read single bytes until EOF
        fclose(f);
    } else
        // Read input or example file from pipe or redirected stdin
        fread(input, 1, sizeof input, stdin);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    const char *c = input;
    for (; *c & 16; c += 6)  // until blank line
        rule[readnum(c)][readnum(c + 3)] = true;  // pair ordered as a<b
    // Start second part at blank line: *c=='\n'
    for (int i = 0; *(c + 1); ++i) {
        int j = 0;
        do {
            page[i][j++] = readnum(c + 1);
            c += 3;
        } while (*c == ',');
        pagecount[i] = j;
    }

    int sum1 = 0, sum2 = 0;
    for (int i = 0; i < UPDATES; ++i) {
        for (int j = 1; j < pagecount[i]; ++j)
            // No need to check every pair, only consecutive ones; without loops,
            // ordering is transitive (if a<b and b<c then a<c) and so, for the final
            // order to be uniquely determined, there can be no loops. Or at least not
            // across the middle element we want; but my input was nice enough.
            if (rule[ page[i][j] ][ page[i][j - 1] ]) {
                // Pages out of order, so this is part 2
                sum2 += sortedindex(&page[i][0], 0, pagecount[i] - 1, pagecount[i] >> 1);
                goto next_i;  // break + continue
            }
        // All pages were ordered, so this is part 1
        sum1 += page[i][pagecount[i] >> 1];  // pick middle element
    next_i:;
    }
    printf("%u %u\n", sum1, sum2);  // 5747 5502

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
