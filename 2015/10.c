// https://en.wikipedia.org/wiki/Look-and-say_sequence

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

// Problem parameters part 1 & 2
#define ITER1  40
#define ITER2  50

// https://www.wolframalpha.com/input/?i=conway’s+constant
#define CONWAY 1.303577269034

// Personalised input from AoC
static const char *seed = "1113222113";

static void looksay(const char *const a, char *const b, size_t *const len)
{
    size_t i = 0, j, k = 0;
    while (i < *len) {
        j = i + 1;
        while (a[j] == a[i])  // a[j] == 0 != a[i] for all j >= len (assumes len < sizeof(a))
            ++j;
        b[k++] = (char)(j - i);  // assumes k < sizeof(b)
        b[k++] = a[i];
        i = j;
    }
    *len = k;
}

static size_t looksaytwice(char *const a, char *const b, size_t *const len, const size_t beg, const size_t end)
{
    for (size_t i = beg; i < end; i += 2) {
        looksay(a, b, len);
        looksay(b, a, len);
    }
    return *len;
}

int main(void)
{
    size_t len = strlen(seed);
    // Smallest size enough for ITER2=50: floor(CC^57)=3654234
    // This version ceil(CC^58)=4763577, which is fine too
    // size_t maxlen = (size_t)(ceil(pow(CONWAY, ITER2 + floor(log(len) / log(CONWAY)))));
    size_t maxlen = 4 * 1024 * 1024;  // nice & round 4 MB = 4194304 > 3579328
    // printf("maxlen=%zu\n", maxlen);

    // make all zero to recognise end of sequence
    char *a = calloc(maxlen, sizeof *a);
    char *b = calloc(maxlen, sizeof *b);

    // Replace chars by numerical values
    strcpy(a, seed);
    for (size_t i = 0; i < len; ++i)
        a[i] -= '0';

    size_t p1 = looksaytwice(a, b, &len, 0, ITER1);
    printf("Part 1: %zu\n", p1);  // 252594
    size_t p2 = looksaytwice(a, b, &len, ITER1, ITER2);
    printf("Part 2: %zu\n", p2);  // 3579328

    // Approximation good to 5 decimals (rounded)
    // printf("Conway's Constant : %.9f\n", CONWAY);
    // printf("This approximation: %.9f\n", exp((log(p2) - log(p1)) / (ITER2 - ITER1)));

    free(a);
    free(b);
    return 0;
}
