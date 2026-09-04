/**
 * Advent of Code 2023
 * Day 4: Scratchcards
 * https://adventofcode.com/2023/day/4
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 04.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 04.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  2.90 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :  4.85 µs
 *     iMac 2013 (i5 4570 3.2 GHz)   : 14.8  µs
 *     Raspberry Pi 5 (2.4 GHz)      : 17.0  µs
 */

#include <stdio.h>
#ifdef TIMER
    #include <string.h>  // memset
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2023-04-input.txt"
#define CARDS 202  // 202 cards (= lines in input file)
#define WINS  10   // 10 winning numbers
#define HAVE  25   // 25 numbers I have

// "Card xxx:" + "| " + newline = 12 chars
#define FSIZE (CARDS * ((WINS + HAVE) * 3 + 12))

static char input[FSIZE];
static int copies[CARDS];

static inline int min(const int a, const int b)
{
    return a < b ? a : b;
}

static inline int readnum(const char *s)
{
    return (*s & 15) * 10 + (*(s + 1) & 15);
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");  // fread requires binary mode
    if (!f) return 1;
    fread(input, FSIZE, 1, f);  // read as single block
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
    memset(copies, 0, sizeof copies);
#endif

    int part1 = 0, part2 = 0;
    const char *c = input;
    for (int card = 0; card < CARDS; ) {
        c += 10;  // skip "Card xxx: "
        __uint128_t wins = 0;
        for (int i = 0; i < WINS; c += 3, ++i)
            wins |= (__uint128_t)1 << readnum(c);

        c += 2;  // skip "| "
        int match = 0;  // count winning numbers on this card
        for (int i = 0; i < HAVE; c += 3, ++i)
            match += wins >> readnum(c) & 1;
        part1 += match ? (1 << (match - 1)) : 0;

        const int add = ++copies[card];  // count original card as one more copy
        part2 += add;

        const int lim = min(++card + match, CARDS);  // 1 past maximum index of extra copies to add
        for (int i = card; i < lim; ++i)  // add extra copies
            copies[i] += add;
    }
    printf("%u %u\n", part1, part2);  // 24733 5422730

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
