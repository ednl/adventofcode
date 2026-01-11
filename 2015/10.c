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
 *     Macbook Pro 2024 (M4 4.4 GHz) :  4.47 ms
 *     Mac Mini 2020 (M1 3.2 GHz)    :     ? ms
 *     Raspberry Pi 5 (2.4 GHz)      : 19.0  ms
 */

#include <stdio.h>
#include <math.h>  // exp, log
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

// My input: element Po
static const char seed[] = "1113222113";

// Iteration (decay) steps parts 1 and 2
#define N1 40
#define N2 50

// https://en.wikipedia.org/wiki/Look-and-say_sequence#Cosmological_decay
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

// https://www.wolframalpha.com/input/?i=conway%27s+constant
#define CONWAY 1.303577269034

// Conway's constant is limit of growth factor between successive lengths
// so for L(0) = 10, L(50) ~= 10 * CC^50 ~= 5712667
#define SIZE (1 << 22)  // 4 * 1024 * 1024 = 4194304 is good enough for my input

// Back & forth look-and-say sequences, start with a, sequence to b, back to a, etc.
static char a[SIZE];
static char b[SIZE];

static void looksay(const char *const a, char *const b, int *const len)
{
    int i = 0, j, k = 0;
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

static int looksaytwice(char *const a, char *const b, int *const len, const int beg, const int end)
{
    for (int i = beg; i < end; i += 2) {
        looksay(a, b, len);
        looksay(b, a, len);
    }
    return *len;
}

int main(void)
{
#ifdef TIMER
    starttimer();
#endif

    // Strcpy and replace chars by numerical values
    int len = sizeof seed - 1;  // array length - NUL terminator
    for (int i = 0; i < len; ++i)
        a[i] = seed[i] - '0';

    int p1 = looksaytwice(a, b, &len, 0, N1);
    printf("Part 1: %d\n", p1);  // 252594

    int p2 = looksaytwice(a, b, &len, N1, N2);
    printf("Part 2: %d\n", p2);  // 3579328

#ifdef TIMER
    double us = stoptimer_us();
#endif

    // True in the limit: p2 = p1 * pow(CONWAY, N2 - N1)
    // => log(p2 / p1) = (N2 - N1) * log(CONWAY)
    // => CONWAY = exp(log(p2 / p1) / (N2 - N1))
    // Approximation good to 5 decimals (rounded)
    printf("Conway's Constant : %.9f\n", CONWAY);
    printf("This approximation: %.9f\n", exp(log((double)p2 / p1) / (N2 - N1)));

#ifdef TIMER
    printf("Time: %.0f us\n", us);
#endif
}
