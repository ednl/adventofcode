/**
 * Advent of Code 2015
 * Day 10: Elves Look, Elves Say
 * https://adventofcode.com/2015/day/10
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    cc -std=c17 -Wall -Wextra -pedantic 10.c -lm
 * Enable timer:
 *    cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 10.c -lm
 * Get minimum runtime from timer output in bash:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 22 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :  ? µs
 *     Raspberry Pi 5 (2.4 GHz)      :  ? µs
 */

#include <stdio.h>
#include <stdlib.h>    // qsort, bsearch
#include <string.h>    // strlen
#include <math.h>      // exp, log
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

// Personalised input
static const char input[] = "1113222113";

// Decay steps parts 1 and 2
#define N1 40
#define N2 50

// https://en.wikipedia.org/wiki/Look-and-say_sequence#Cosmological_decay
#define ELEMENTS 92
#define DECAYSIZE 6

// https://www.wolframalpha.com/input/?i=conway%27s+constant
#define CONWAY 1.303577269034

// Decay process:
//  0: Po
//  1: Bi
//  2: Pm                                                   .Pb
//  3: Nd                                                   .Tl
//  4: Pr                                                   .Hg
//  5: Ce                                                   .Au
//  6: La                     .H .Ca   .Co                  .Pt
//  7: Ba                     .H .K    .Fe                  .Ir
//  8: Cs                     .H .Ar   .Mn                  .Os
//  9: Xe                     .H .Cl   .Cr            .Si   .Re
// 10: I                      .H .S    .V             .Al   .Ge                           .Ca.W
// 11: Ho   .Te               .H .P    .Ti            .Mg   .Ho   .Ga                     .K .Ta
// 12: Dy   .Eu      .Ca.Sb   .H .Ho.Si.Sc            .Pm.Na.Dy   .Eu      .Ca.Ac.H .Ca.Zn.Ar.Hf.Pa.H .Ca.W
// 13: Tb   .Sm      .K .Pm.Sn.H .Dy.Al.Ho.Pa.H .Ca.Co.Nd.Ne.Tb   .Sm      .K .Ra.H .K .Cu.Cl.Lu.Th.H .K .Ta
// 14: Ho.Gd.Pm.Ca.Zn.Ar.Nd.In.H .Tb.Mg.Dy.Th.H .K .Fe.Pr.F .Ho.Gd.Pm.Ca.Zn.Ar.Fr.H .Ar.Ni.S .Yb.Ac.H .Ar.Hf.Pa.H .Ca.W
//     etc.
// But these are elements that don't interact, so order doesn't matter which means
// that extra elements can just be added to the end. Or: keep a count per element.

typedef struct pair {
    int32_t key, val;
} Pair;

static Pair    elmname  [ELEMENTS];  // element name+index
static int32_t elmdecay [ELEMENTS][DECAYSIZE];  // element splits into which elements
static int     elmsplit [ELEMENTS];  // element splits into how many elements when decaying
static int     elmlen   [ELEMENTS];  // element length, e.g. "12311" = 5
static int     elmcounta[ELEMENTS];  // element count in the sequence (a)
static int     elmcountb[ELEMENTS];  // element count in the sequence (b)

// Qsort *and* bsearch helper
// First field of struct must be int32_t
static int byname(const void *p, const void *q)
{
    const int32_t a = *(const int32_t *)p;
    const int32_t b = *(const int32_t *)q;
    if (a < b) return -1;
    if (a > b) return  1;
    return 0;
}

// Decay elements from a to b
static void decay_ab(const int *restrict a, int *restrict b)
{
    memset(b, 0, sizeof *b * ELEMENTS);
    for (int i = 0; i < ELEMENTS; ++i)
        for (int j = 0; j < elmsplit[i]; ++j)
            b[elmdecay[i][j]] += a[i];
}

// Decay sequence an even number of steps
// Return final sequence length
static int decay(const int steps)
{
    for (int i = 0; i < steps; i += 2) {
        decay_ab(elmcounta, elmcountb);
        decay_ab(elmcountb, elmcounta);
    }
    int len = 0;
    for (int i = 0; i < ELEMENTS; ++i)
        len += elmcounta[i] * elmlen[i];
    return len;
}

int main(void)
{
#ifdef TIMER
    starttimer();
#endif

    // Parse the elements table = text version of
    // https://en.wikipedia.org/wiki/Look-and-say_sequence#Cosmological_decay
    // without the last column "abundance"
    // Assumes input is an element
    FILE *f = fopen("10.txt", "r");
    if (!f) return 1;
    char snum[4], sname[4], satoms[48], sdecay[20];
    for (int i = 0; i < ELEMENTS && fscanf(f, "%2s %2s %42s %16s", snum, sname, satoms, sdecay) == 4; ++i) {
        elmname[i] = (Pair){.key = sname[0] | sname[1] << 8, .val = i};
        int j = 0;
        for (const char *s = sdecay; j < DECAYSIZE; ++s) {
            int x = *s++;
            if (*s && *s != '.')
                x |= *s++ << 8;
            elmdecay[i][j++] = x;
            if (!*s)
                break;
        }
        elmsplit[ i] = j;
        elmlen   [i] = strlen(satoms);
        elmcounta[i] = !strcmp(satoms, input);  // starting element
    }
    fclose(f);

    // Replace element names with indexes
    // Assumes all elements can be found
    qsort(elmname, ELEMENTS, sizeof *elmname, byname);
    for (int i = 0; i < ELEMENTS; ++i)
        for (int j = 0; j < elmsplit[i]; ++j)
            elmdecay[i][j] = ((const Pair *)bsearch(&elmdecay[i][j], elmname, ELEMENTS, sizeof *elmname, byname))->val;

#ifdef DEBUG
    // Check result
    for (int i = 0; i < ELEMENTS; ++i) {
        printf("%c%2d [%2d] %d->", elmcounta[i] ? '*' : ' ', i, elmlen[i], elmsplit[i]);
        for (int j = 0; j < elmsplit[i]; ++j)
            printf("%3d", elmdecay[i][j] + 1);
        printf("\n");
    }
#endif

    const int p1 = decay(N1);
    const int p2 = decay(N2 - N1);
    printf("%d\n", p1);  // 252594
    printf("%d\n", p2);  // 3579328

#ifdef TIMER
    double us = stoptimer_us();
#endif

    // True in the limit: p2 = p1 * pow(CONWAY, N2 - N1)
    // => log(p2 / p1) = (N2 - N1) * log(CONWAY)
    // => CONWAY = exp(log(p2 / p1) / (N2 - N1))
    // Approximation good to 5 decimals! (rounded)
    printf("Conway's Constant : %.9f\n", CONWAY);
    printf("This approximation: %.9f\n", exp(log((double)p2 / p1) / (N2 - N1)));

#ifdef TIMER
    printf("Time: %.0f us\n", us);
#endif
}
