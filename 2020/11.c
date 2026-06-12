// Advent of Code 2020, day 11: "Seating System"
// https://adventofcode.com/2020/day/11

#include <stdio.h>   // fopen, fclose, getline
#include <stdlib.h>  // free
#include <string.h>  // memcpy
#include <stdint.h>  // int8_t
#include "../startstoptimer.h"

#define INPUT_FLOOR '.'
#define INPUT_EMPTY 'L'
#define INPUT_OCCUP '#'
#define STATE_FLOOR  -1
#define STATE_EMPTY   0
#define STATE_OCCUP   1
#if EXAMPLE
    // For terminal display
    #define HOME    "\e[H"
    #define CLEAR   "\e[2J"
    #define HIDECUR "\e[?25l"
    #define H (10 + 2)
    #define W (10 + 2)
    const char *inp = "../aocinput/2020-11-example.txt";
#else
    #define H (91 + 2)
    #define W (98 + 2)
    const char *inp = "../aocinput/2020-11-input.txt";
#endif

// Original data and two copies for evolving
// signed because state = -1 | 0 | 1
#define LEN (H * W)
int8_t data[LEN], area1[LEN], area2[LEN];

// Read and parse the input file
// '.' = floor (-1) never changes
// 'L' = empty seat (0) can become '#' = occupied (1)
void read(void)
{
    FILE *fp;
    char *s = NULL;
    size_t t = 0;
    int i, k;

    // Surround with empty seats
    k = W * (H - 1);
    for (i = 0; i < W; ++i) {
        data[i] = data[k + i] = STATE_EMPTY;
    }
    k = 0;
    for (i = 1; i < H; ++i) {
        k += W;
        data[k] = data[k - 1] = STATE_EMPTY;
    }

    if ((fp = fopen(inp, "r")) != NULL) {
        k = W + 1;  // first non-boundary index in data
        while (getline(&s, &t, fp) > 0) {
            i = 0;  // input column
            while (s[i] != '\n' && s[i] != '\0') {
                if (k < LEN) {  // shouldn't be necessary if H and W fit the input
                    data[k++] = s[i] == INPUT_EMPTY ? STATE_EMPTY : STATE_FLOOR;
                }
                ++i;
            }
            k += 2;  // skip 2x boundary
        }
        free(s);
        fclose(fp);
    }
}

// Fresh copies of input data
void init(void)
{
    memcpy(area1, data, sizeof data);
    memcpy(area2, data, sizeof data);
}

int neighbours(int8_t *area, int index, int threshold, int part)
{
    int nb = 0;
    for (int i = -1; i <= 1; ++i) {      // row offset (direction)
        int dk = i * W - 1;              // first change in index
        for (int j = -1; j <= 1; ++j) {  // col offset (direction)
            if (i || j) {                // not the centre
                int k = index + dk;      // first new index in this direction
                while (1) {              // area is surrounded by empty seats, so loop will break
                    // Part 1 = only direct neighbours, part 2 = 8 lines of sight
                    if (area[k] != STATE_FLOOR || part == 1) {
                        // Check & stop looking in this direction when reaching a seat
                        if (area[k] == STATE_OCCUP) {
                            if (++nb == threshold)  // reached threshold?
                                return 1;        // no point in counting more
                        }
                        break;
                    }
                    k += dk;
                }
            }
            ++dk;
        }
    }
    return 0;  // did not reach threshold
}

int evolve(int8_t *restrict a1, int8_t *restrict a2, int part)
{
    int k = W, threshold = part + 3, changed = 0;
    for (int r = 1; r < H - 1; ++r) {
        for (int c = 1; c < W - 1; ++c) {
            ++k;
            if (a1[k] == STATE_EMPTY) {
                if (neighbours(a1, k, 1, part)) {  // any neighbours?
                    a2[k] = STATE_EMPTY;
                } else {
                    a2[k] = STATE_OCCUP;
                    changed = 1;
                }
            } else if (a1[k] == STATE_OCCUP) {
                if (neighbours(a1, k, threshold, part)) {  // at least 4 or 5 neighbours?
                    a2[k] = STATE_EMPTY;
                    changed = 1;
                } else {
                    a2[k] = STATE_OCCUP;
                }
            }
        }
        k += 2;
    }
    return changed;
}

// Print map of occupied seats on a (large) terminal
#if EXAMPLE
void show(const int8_t *const area)
{
    printf("%s", CLEAR);
    printf("%s", HOME);
    for (int i = 0; i < H; ++i) {
        for (int j = 0; j < W; ++j) {
            int k = i * W + j;
            char c;
            switch (area[k]) {
                case STATE_FLOOR: c = INPUT_FLOOR; break;
                case STATE_EMPTY: c = INPUT_EMPTY; break;
                case STATE_OCCUP: c = INPUT_OCCUP; break;
                default: c = '?'; break;
            }
            printf("%c", c);
        }
        printf("\n");
    }
    printf("\n");
    for (volatile unsigned long delay = 0; delay < 20000000; ++delay);
}
#endif

// Count number of occupied seats
int occupied(const int8_t *const area)
{
    int n = 0;
    for (int i = 0; i < LEN; ++i)
        n += area[i] == STATE_OCCUP;
    return n;
}

int main(void)
{
    read();
    starttimer();
    for (int part = 1; part <= 2; ++part) {
        init();
        int8_t *p = area1;
        int8_t *q = area2;
        while (evolve(p, q, part)) {
            void *const tmp = p;
            p = q;
            q = tmp;
        #if EXAMPLE
            show(p);
        #endif
        }
        printf("%d\n", occupied(p));  // part 1: 2303, part 2: 2057
    }
    printf("Time: %.0f us\n", stoptimer_us());
}
