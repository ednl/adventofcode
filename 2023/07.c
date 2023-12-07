/**
 * Advent of Code 2023
 * Day 7: ?
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

typedef enum type {
    HIGHCARD, ONEPAIR, TWOPAIR, THREEKIND, FULLHOUSE, FOURKIND, FIVEKIND
} Type;

typedef struct hand {
    char face[CARDS + 3];  // +1 for \0, +2 for alignment
    int facesval, bid;
    Type type;
} Hand;

typedef struct facecount {
    int faceval, count;
} Facecount;

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

// Value [0..12] for faces
//   [2..9,T,J,Q,K,A] (part 1)
//   [J,2..9,T,Q,K,A] (part 2)
static int facevalue(const char face, const bool part2)
{
    if (face <= '9')
        return (face & 15) - (part2 ? 1 : 2);
    switch (face) {
        case 'T': return part2 ? 9 : 8;
        case 'J': return part2 ? 0 : 9;
        case 'Q': return 10;
        case 'K': return 11;
        case 'A': return 12;
    }
    return 0;
}

// Set facesval and type for one hand (part 1)
static void analyze(Hand* const hand, const bool ispart2)
{
    // Init
    Facecount histo[FACES] = {0};
    for (int i = 0; i < FACES; ++i)
        histo[i].faceval = i;

    // Set facesval, histogram of faces
    int val = 0;
    for (int i = 0; i < CARDS; ++i) {
        const int v = facevalue(hand->face[i], ispart2);
        val = (val << 4) | v;
        histo[v].count++;
    }
    hand->facesval = val;  // second sorting key for rank

    // Count jokers
    const int jokerval = facevalue('J', ispart2);
    const int jokers = histo[jokerval].count;  // not sorted yet

    // Set type (first sorting key for rank)
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
        case 1: hand->type = HIGHCARD; break;
        case 2: hand->type = second->count == 2 ? TWOPAIR : ONEPAIR; break;
        case 3: hand->type = second->count == 2 ? FULLHOUSE : THREEKIND; break;
        case 4: hand->type = FOURKIND; break;
        case 5: hand->type = FIVEKIND; break;
    }
}

// Sort hands by type descending, facesval descending
static int rank_desc(const void *p, const void *q)
{
    const Hand* a = (const Hand*)p;
    const Hand* b = (const Hand*)q;
    if (a->type > b->type) return -1;
    if (a->type < b->type) return  1;
    if (a->facesval > b->facesval) return -1;
    if (a->facesval < b->facesval) return  1;
    return 0;
}

static int winnings(const bool ispart2)
{
    for (int i = 0; i < HANDS; ++i)
        analyze(&game[i], ispart2);
    qsort(game, HANDS, sizeof *game, rank_desc);

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
