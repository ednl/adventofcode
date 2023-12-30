/**
 * Advent of Code 2023
 * Day 7: Camel Cards
 * https://adventofcode.com/2023/day/7
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -Ofast -march=native -Wall -Wextra 07.c ../startstoptimer.c
 *    gcc   -std=gnu17 -Ofast -march=native -Wall -Wextra 07.c ../startstoptimer.c
 * Get minimum runtime:
 *     m=999999;for((i=0;i<5000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * Minimum runtime:
 *     Mac Mini 2020 (M1 3.2 GHz)          :  323 µs
 *     iMac 2013 (i5 Haswell 4570 3.2 GHz) :  499 µs
 *     Raspberry Pi 5 (2.4 GHz)            :  562 µs
 *     Raspberry Pi 4 (1.8 GHz)            : 1184 µs
 */

#include <stdio.h>    // fopen, fclose, fscanf, printf
#include <stdlib.h>   // qsort
#include <stdbool.h>  // bool
#include "../startstoptimer.h"

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define EXAMPLE 0
#if EXAMPLE == 1
    #define NAME "../aocinput/2023-07-example.txt"
    #define HANDS 5
#else
    #define NAME "../aocinput/2023-07-input.txt"
    #define HANDS 1000
#endif
#define HANDSIZE  5  // five card deal
#define VALRANGE 15  // face values range from 0 ('J' in part 2) to 14 ('A')

// Rankings for a hand of 5 cards, from lowest to highest.
// Called 'type' in the puzzle.
typedef enum rank {
    NONE, HIGHCARD, ONEPAIR, TWOPAIR, THREEKIND, FULLHOUSE, FOURKIND, FIVEKIND
} Rank;

// All info for one hand
// ::card and ::bid from input
// ::rank is first sorting key for hand strength
// ::deal is second sorting key for hand strength
typedef struct hand {
    char card[HANDSIZE + 1];  // +1 for '\0'
    Rank rank;
    int deal, bid;
} Hand;

// Complete game with all hands
static Hand game[HANDS];

// Qsort helper: sort game of hands by rank descending, deal descending
static int strength_desc(const void *p, const void *q)
{
    const Hand* a = (const Hand*)p;
    const Hand* b = (const Hand*)q;
    if (a->rank > b->rank) return -1;
    if (a->rank < b->rank) return  1;
    if (a->deal > b->deal) return -1;
    if (a->deal < b->deal) return  1;
    return 0;
}

// Value [2..14] for 13 different card faces from [2..9,T,J,Q,K,A]
static int facevalue(const char card)
{
    if (card <= '9')  // no check for card >= '2' because all cards are
        return card & 15;
    switch (card) {
        case 'T': return 10;
        case 'J': return 11;
        case 'Q': return 12;
        case 'K': return 13;
        case 'A': return 14;
    }
    return 0;  // shouldn't reach this
}

// Evaluate a hand by setting ::rank and ::deal
// ::rank is primary sort where 'five of a kind' wins
// ::deal is secondary sort by face value of first card in deal, then second, etc.
static void eval(Hand* const hand, const bool ispart2)
{
    // Make histogram of face values and set ::deal = hash of face values in deal order
    int count[VALRANGE] = {0};
    hand->deal = 0;
    for (int i = 0; i < HANDSIZE; ++i) {
        // const int val = facevalue(hand->card[i], ispart2);
        const int val = hand->card[i] == 'J' && ispart2 ? 0 : facevalue(hand->card[i]);
        count[val]++;
        hand->deal = (hand->deal << 4) | val;  // each value fits in 4 bits
    }

    // Count jokers and remove them for part 2.
    const int jokers = count[0];  // joker value is 0 in part 2, so count is at index=0
    if (ispart2)
        count[0] = 0;

    // Order histogram by most to least (without jokers for part 2).
    size_t len = 0, last = 14;
    while (len < last) {
        while (count[len]) ++len;  // find first count=0
        while (last && !count[last]) --last;  // find last count!=0
        if (len < last) {
            count[len++] = count[last];
            count[last--] = 0;
        }
    }
    while (count[len]) ++len;
    if (len > 1) {
        for (size_t i = 0, max = 0; i < len - 1; max = ++i) {
            for (size_t j = i + 1; j < len; ++j)
                if (count[j] > count[max])
                    max = j;
            if (max != i) {
                const int tmp = count[i];
                count[i] = count[max];
                count[max] = tmp;
            }
        }
    }

    // Best option is always to add all jokers to highest card count
    count[0] += jokers;  // jokers=0 in part 1 because count[0]=0

    // Set rank (= first sorting key for hand strength)
    switch (count[0]) {
        case 1: hand->rank = HIGHCARD; break;
        case 2: hand->rank = count[1] == 2 ? TWOPAIR : ONEPAIR; break;
        case 3: hand->rank = count[1] == 2 ? FULLHOUSE : THREEKIND; break;
        case 4: hand->rank = FOURKIND; break;
        case 5: hand->rank = FIVEKIND; break;
    }
}

// By Grabthar, what a ... winnings
static int winnings(const bool ispart2)
{
    for (int i = 0; i < HANDS; ++i)
        eval(&game[i], ispart2);
    qsort(game, HANDS, sizeof *game, strength_desc);
    int score = 0;
    for (int i = 0; i < HANDS; ++i)
        score += game[i].bid * (HANDS - i);
    return score;
}

int main(void)
{
    starttimer();
    FILE* f = fopen(NAME, "r");
    if (!f) { fputs("File not found.\n", stderr); return 1; }

    for (int i = 0; i < HANDS; ++i)
        fscanf(f, "%"STR(HANDSIZE)"s %d", game[i].card, &game[i].bid);
    fclose(f);

    printf("Part 1: %d\n", winnings(1 == 2));  // example: 6440, input: 250957639
    printf("Part 2: %d\n", winnings(2 == 2));  // example: 5905, input: 251515496
    printf("Time: %.0f us\n", stoptimer_us());
    return 0;
}
