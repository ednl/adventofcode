/**
 * Advent of Code 2021
 * Day 18: Snailfish
 * https://adventofcode.com/2021/day/18
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -Wall -Wextra 18.c
 *    gcc   -std=gnu17 -Wall -Wextra 18.c
 * Enable timer:
 *    clang -DTIMER -O3 -march=native 18.c ../startstoptimer.c
 *    gcc   -DTIMER -O3 -march=native 18.c ../startstoptimer.c
 * Get minimum runtime:
 *     m=999999;for((i=0;i<5000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime:
 *     Macbook Pro 2024 (M4 4.4 GHz)                    :  7.88 ms
 *     Mac Mini 2020 (M1 3.2 GHz)                       :  ?    ms
 *     iMac 2013 (Core i5 Haswell 4570 3.2 GHz)         :  ?    ms
 *     Raspberry Pi 5 (2.4 GHz)                         :  ?    ms
 */

#include <stdio.h>   // getline
#include <stdlib.h>  // free (after getline)
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define LINES  100  // snailfish numbers in input file
#define DIGITS 300  // stack size for digit expansion while splitting, max for my input = 274
static const char *fname = "../aocinput/2021-18-input.txt";

typedef struct {
    int level, value;
    int prev, next;
} SFDigit;
typedef struct {
    int len, last, size;
    SFDigit digit[DIGITS];
} SFNumber;

static SFNumber homework[LINES] = {0};

// Parse string to snailfish number
static int strtosfn(const char *const str, SFNumber *const pn, const int size)
{
    int i = 0, level = 0;
    const char *c = str;
    while (*c) {
        switch (*c) {
            case '[': ++level; break;
            case ']': --level; break;
            default:
                if (*c >= '0' && *c <= '9') {
                    if (i >= size)
                        return -1;
                    pn->digit[i] = (SFDigit){level, *c - '0', i - 1, i + 1};
                    ++i;
                }
                break;
        }
        ++c;
    }
    if (i)
        pn->digit[i - 1].next = -1;
    pn->len = pn->size = i;
    pn->last = i - 1;
    return i;
}

// Save input to string array and as snailfish numbers, return lines read
static size_t read(void)
{
    size_t i = 0;
    FILE *f = fopen(fname, "r");
    if (f) {
        char *line = NULL;
        size_t size;
        ssize_t len;
        while (i < LINES && (len = getline(&line, &size, f)) > 0)
            strtosfn(line, &homework[i++], DIGITS);
        free(line);
        fclose(f);
    }
    return i;
}

// Copy and untangle a snailfish number, return number of digits
static int copy(SFNumber *const dst, const SFNumber *const src)
{
    int i = 0;
    for (int j = 0; j != -1; j = src->digit[j].next) {
        dst->digit[i] = (SFDigit){src->digit[j].level, src->digit[j].value, i - 1, i + 1};
        ++i;
    }
    if (i)
        dst->digit[i - 1].next = -1;
    dst->len = dst->size = i;
    dst->last = i - 1;
    return i;
}

// Replace embedded pairs [a,b] that are too high level with single number 0 one level lower
// Replaces all pairs, if any, then returns whether any were replaced
//   (this to enable repeated explode/split checks)
static bool explode(SFNumber *const pa)
{
    static const int toohigh = 5;  // snailfish numbers must be normalised every time, so >5 will not occur
    bool found = false;
    for (int i = 0, j, k; i != -1; i = pa->digit[i].next) {
        if (pa->digit[i].level == toohigh) {
            pa->digit[i].level--;
            if ((j = pa->digit[i].prev) != -1)                 // is there a digit to the left?
                pa->digit[j].value += pa->digit[i].value;      // shift left value to the left
            pa->digit[i].value = 0;                            // reset left value to zero
            if ((j = pa->digit[i].next) != -1) {               // is there a digit to the right? Should be!
                found = true;                                  // found a pair to be replaced
                if ((k = pa->digit[j].next) == -1) {           // is there no digit to the right of that?
                    pa->last = i;                              // adjust endpoint marker
                } else {
                    pa->digit[k].value += pa->digit[j].value;  // shift right value to the right
                    pa->digit[k].prev = i;                     // remove right value from linked list (backward)
                }
                pa->digit[i].next = k;                         // remove right value from linked list (forward)
                pa->len--;                                     // number of digits decreased
            } else {
                fprintf(stderr, "Explode: corrupt data\n");
                return false;
            }
        }
    }
    return found;
}

// Split the first digit with value v>9 into a pair of digits [floor(v/2),ceil(v/2)]
// Return whether anything changed (if so, go back to checking for levels>4)
static bool split(SFNumber *const pa)
{
    for (int i = 0, v; i != -1; i = pa->digit[i].next) {
        if ((v = pa->digit[i].value) > 9) {
            if (pa->size == DIGITS) {   // no more room in the inn?
                fprintf(stderr, "Split: out of memory\n");
                return false;
            }
            int j = pa->size++;         // allocate space for new digit
            int k = pa->digit[i].next;  // save index of digit after current
            pa->digit[i].next = j;      // insert digit (forward)
            pa->digit[j].level = ++(pa->digit[i].level);  // level up pair of digits
            pa->digit[j].value = v - (pa->digit[i].value >>= 1);  // distribute value
            pa->digit[j].prev = i;      // link in new digit (backward)
            pa->digit[j].next = k;      // link in new digit (forward)
            if (k == -1)                // was there no next digit?
                pa->last = j;           // adjust endpoint marker
            else
                pa->digit[k].prev = j;  // insert node (backward)
            pa->len++;                  // increase number of digits
            return true;                // a digit was split
        }
    }
    return false;                       // no digit was split
}

// Add snailfish number b to a, normalise, return number of digits
static int add(SFNumber *const pa, const SFNumber *const pb)
{
    if (!pa->len || !pb->len) {
        fprintf(stderr, "Add: empty number\n");
        return -1;
    }
    if (pa->len + pb->len > DIGITS) {
        fprintf(stderr, "Add: number overflow\n");
        return -2;
    }
    SFNumber sum;                          // accumulator
    copy(&sum, pa);                        // copy a to sum to untangle it (also sets len/last/size)
    int i = sum.size, k = sum.last;        // where to start appending
    sum.digit[k].next = i;                 // open up the end
    for (int j = 0; j != -1; j = pb->digit[j].next) {
        sum.digit[i] = (SFDigit){pb->digit[j].level, pb->digit[j].value, k, i + 1};  // append
        k = i++;                           // copies are sequential
    }
    sum.digit[i - 1].next = -1;            // terminate
    sum.len += pb->len;                    // adjust digit count
    sum.size = i;                          // adjust mem size
    sum.last = i - 1;                      // adjust last digit index
    for (int j = 0; j != -1; j = sum.digit[j].next)
        sum.digit[j].level++;              // level up because a + b = [a,b]
    while (explode(&sum) || split(&sum));  // normalise
    copy(pa, &sum);                        // copy result to a, also to untangle it (also sets len/last/size)
    return pa->len;                        // number of digits in result
}

// Magnitude of normalised snailfish number a (destroys number)
static int magnitude(SFNumber *const pa)
{
    for (int level = 4; level > 0; --level) {    // reduce from highest level down
        for (int i = 0; i != -1; i = pa->digit[i].next) {
            if (pa->digit[i].level == level) {   // ready to reduce this pair?
                int j = pa->digit[i].next;       // index of second digit
                if (j != -1 && pa->digit[j].level == level) {  // but is it a pair? Should be!
                    int k = pa->digit[j].next;   // index of digit after pair
                    pa->digit[i].level--;        // level down because [i,j] -> 3i + 2j
                    pa->digit[i].value = 3 * pa->digit[i].value + 2 * pa->digit[j].value;  // pair magnitude
                    pa->digit[i].next = k;       // remove second digit (forward)
                    if (k == -1)
                        pa->last = i;            // adjust endpoint marker
                    else
                        pa->digit[k].prev = i;   // remove second digit (backward)
                    pa->len--;                   // decrease number of digits
                } else {
                    fprintf(stderr, "Magnitude: corrupt data\n");
                    return -1;
                }
            }
        }
    }
    return pa->digit[0].value;  // after reduction, magnitude is value of first digit
}

int main(void)
{
#ifdef TIMER
    starttimer();
#endif

    size_t n = read();
    SFNumber sum = {0};
    copy(&sum, &homework[0]);     // init sum with first snailfish number
    for (size_t i = 1; i < n; ++i)
        add(&sum, &homework[i]);  // add all other snailfish numbers
    printf("Part 1: %d\n", magnitude(&sum));  // 3869

    int mag, maxmag = 0;
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < n; ++j)
            if (i != j) {  // sum not commutative so test all i,j where i!=j
                copy(&sum, &homework[i]);  // must copy each time because add() modifies first arg
                add(&sum, &homework[j]);
                if ((mag = magnitude(&sum)) > maxmag)
                    maxmag = mag;
            }
    printf("Part 2: %d\n", maxmag);  // 4671

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
    return 0;
}
