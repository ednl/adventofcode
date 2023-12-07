/**
 * Advent of Code 2023
 * Day 7: Camel Cards
 * https://adventofcode.com/2023/day/7
 * By: E. Dronkert https://github.com/ednl
 */

#include <stdio.h>    // fopen, fclose, fscanf, printf
#include <stdlib.h>   // qsort
#include <stdbool.h>  // bool

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
#define CARDS 5
#define FACES 13

// All possible types of a hand of 5 cards, from lowest to highest
typedef enum type {
    HIGHCARD, ONEPAIR, TWOPAIR, THREEKIND, FULLHOUSE, FOURKIND, FIVEKIND
} Type;

// All info for one hand
// ::face and ::bid from input
// ::type is first sorting key for hand strength
// ::facesval is second sorting key for hand strength
typedef struct hand {
    char face[CARDS + 3];  // +1 for \0, +2 for alignment
    int facesval, bid;
    Type type;
} Hand;

// Histogram of faces in one hand
typedef struct facecount {
    int faceval, count;
} Facecount;

// Complete game with all hands
static Hand game[HANDS];

// Sort facecounts by count descending
static int fc_desc(const void *p, const void *q)
{
    const int a = ((const Facecount*)p)->count;
    const int b = ((const Facecount*)q)->count;
    if (a > b) return -1;
    if (a < b) return  1;
    return 0;
}

// Value [0..12] for 13 different faces =
//   [2..9,T,J,Q,K,A] (part 1)
//   [J,2..9,T,Q,K,A] (part 2)
static int facevalue(const char face, const bool ispart2)
{
    if (face <= '9')  // no check for face >= '2' because all faces are
        return (face & 15) - 2 + ispart2;  // part 2: make room for J at bottom
    switch (face) {
        case 'T': return 8 + ispart2;   // part 1: T=8, part 2: T=9
        case 'J': return 9 * !ispart2;  // part 1: J=9, part 2: J=0
        case 'Q': return 10;
        case 'K': return 11;
        case 'A': return 12;
    }
    return 0;  // shouldn't reach this
}

// Set facesval and type of a hand
static void analyze(Hand* const hand, const bool ispart2)
{
    // Init
    Facecount histo[FACES] = {0};
    for (int i = 0; i < FACES; ++i)
        histo[i].faceval = i;

    // Set facesval, make histogram of faces
    int val = 0;
    for (int i = 0; i < CARDS; ++i) {
        const int v = facevalue(hand->face[i], ispart2);
        val = (val << 4) | v;
        histo[v].count++;
    }
    hand->facesval = val;  // facesval is second sorting key for hand strength

    // Count jokers
    const int jokerval = facevalue('J', ispart2);
    const int jokers = histo[jokerval].count;  // histogram not sorted yet

    // Set type (= first sorting key for hand strength)
    qsort(histo, FACES, sizeof *histo, fc_desc);
    Facecount* most = &histo[0];  // sorted, so histo[0] has the highest count
    Facecount* second = most + 1;
    if (ispart2 && jokers) {
        if (most->faceval == jokerval) {
            ++most;
            ++second;
        } else if (second->faceval == jokerval)
            ++second;
        most->count += jokers;  // best option is always to add all jokers to most
    }
    switch (most->count) {
        case 1: hand->type = HIGHCARD; break;  // must re-init for part 2
        case 2: hand->type = second->count == 2 ? TWOPAIR : ONEPAIR; break;
        case 3: hand->type = second->count == 2 ? FULLHOUSE : THREEKIND; break;
        case 4: hand->type = FOURKIND; break;
        case 5: hand->type = FIVEKIND; break;
    }
}

// Sort hands by type descending, facesval descending
static int strength_desc(const void *p, const void *q)
{
    const Hand* a = (const Hand*)p;
    const Hand* b = (const Hand*)q;
    if (a->type > b->type) return -1;
    if (a->type < b->type) return  1;
    if (a->facesval > b->facesval) return -1;
    if (a->facesval < b->facesval) return  1;
    return 0;
}

// By Grabthar, what a ... winnings
static int winnings(const bool ispart2)
{
    for (int i = 0; i < HANDS; ++i)
        analyze(&game[i], ispart2);
    qsort(game, HANDS, sizeof *game, strength_desc);

    int score = 0;
    for (int i = 0; i < HANDS; ++i)
        score += game[i].bid * (HANDS - i);
    return score;
}

int main(void)
{
    FILE* f = fopen(NAME, "r");
    if (!f)
        return 1;
    for (int i = 0; i < HANDS; ++i)
        fscanf(f, "%"STR(CARDS)"s %d", game[i].face, &game[i].bid);
    fclose(f);

    printf("Part 1: %d\n", winnings(1 == 2));  // example: 6440, input: 250957639
    printf("Part 2: %d\n", winnings(2 == 2));  // example: 5905, input: 251515496
    return 0;
}
