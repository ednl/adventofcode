/**
 * Advent of Code 2021
 * Day 10: Syntax Scoring
 * https://adventofcode.com/2021/day/10
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -Wall -Wextra 10.c
 *    gcc   -std=gnu17 -Wall -Wextra 10.c
 * Enable timer:
 *    clang -DTIMER -O3 -march=native 10.c ../startstoptimer.c
 *    gcc   -DTIMER -O3 -march=native 10.c ../startstoptimer.c
 * Get minimum runtime:
 *     m=9999999;for((i=0;i<5000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime:
 *     Macbook Pro 2024 (M4 4.4 GHz)                    : 480 µs
 *     Mac Mini 2020 (M1 3.2 GHz)                       :   ? µs
 *     iMac 2013 (Core i5 Haswell 4570 3.2 GHz)         :   ? µs
 *     Raspberry Pi 5 (2.4 GHz)                         :   ? µs
 */

#include <stdio.h>
#include <stdlib.h>    // qsort
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRId64
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define STACKSIZE 16  // 15 for my input
#define INCSIZE   48  // 47 for my input
static int stack[STACKSIZE] = {0};
static int64_t incomplete[INCSIZE] = {0};

// Sort i64 array in ascending order
static int asc(const void *a, const void *b)
{
    const int64_t p = *(const int64_t *)a;
    const int64_t q = *(const int64_t *)b;
    return (q < p) - (p < q);
}

int main(void)
{
    int c, id, score = 0, syntaxerror = 0;
    size_t sp = 0, ip = 0;
    FILE *f = fopen("../aocinput/2021-10-input.txt", "r");
    if (!f) return 1;

#ifdef TIMER
    // Timer includes reading from disk
    starttimer();
#endif
    while (!feof(f)) {
        switch ((c = fgetc(f))) {
            case '(' : id = 0x01; break;
            case '[' : id = 0x02; break;
            case '{' : id = 0x03; break;
            case '<' : id = 0x04; break;
            case ')' : id = 0x10; score = 3; break;
            case ']' : id = 0x20; score = 57; break;
            case '}' : id = 0x30; score = 1197; break;
            case '>' : id = 0x40; score = 25137; break;
            case '\n': id = 0xff; break;
            default  : id = 0x00; break;
        }

        if (id == 0xff) {                             // newline?
            if (sp) {                                 // incomplete?
                if (ip != INCSIZE) {                  // room on the stack?
                    int64_t i = 0;                    // determine incomplete score
                    while (sp)
                        i = i * 5 + stack[--sp];      // pop opening bracket (value 1-4)
                    incomplete[ip++] = i;             // save incomplete score
                } else {
                    fprintf(stderr, "Incomplete-score stack overflow");
                    return 1;
                }
            }
        } else if (id & 0x0f) {                       // opening bracket?
            if (sp != STACKSIZE) {                    // room on the stack?
                stack[sp++] = id;                     // push opening bracket (value 1-4)
            } else {
                fprintf(stderr, "Bracket stack overflow");
                return 2;
            }
        } else if (id & 0xf0) {                       // closing bracket?
            if (!sp || stack[--sp] != id >> 4) {      // empty stack or no match?
                syntaxerror += score;                 // count corruption!!
                while (c != '\n' && !feof(f))         // discard rest of line
                    c = fgetc(f);
                sp = 0;                               // start a new line
            }
        }
    }
    fclose(f);
    qsort(incomplete, ip, sizeof *incomplete, asc);

    printf("Part 1: %d\n", syntaxerror);              // 413733
    printf("Part 2: %"PRId64"\n", incomplete[ip/2]);  // 3354640192
#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
    return 0;
}
