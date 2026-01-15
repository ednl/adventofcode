/**
 * Advent of Code 2021
 * Day 4: Giant Squid
 * https://adventofcode.com/2021/day/4
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 04.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 04.c
 * Get minimum runtime from timer output in bash:
 *     m=9999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  7.71 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :     ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : 29.5  µs
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2021-04-input.txt"
#define FSIZE 7890 // input file size in bytes
static char input[FSIZE];

#define CALL 100  // numbers called
#define CARD 100  // number of bingo cards
#define SIZE 5    // bingo cards are 5x5
#define MULT 40   // each number called appears on how many cards max (my input: 13..37)

// Location of a number on a card by row/col
typedef struct pos {
    int card, row, col;
} Pos;

static int call[CALL];              // 100 numbers called
static int card[CARD][SIZE][SIZE];  // 100 bingo cards of size 5x5
static int markrow[CARD][SIZE];     // count of marked numbers for every 5 rows per card
static int markcol[CARD][SIZE];     // count of marked numbers for every 5 columns per card
static int mult[CALL];              // each number called is on how many cards ("multiplicity")
static Pos pos[CALL][MULT];         // all locations of called numbers
static bool bingo[CARD];            // did a card get bingo yet?

static int score(const int cardindex, const int lastdrawn)
{
    int sum = 0;
    for (int i = 0; i < SIZE; ++i)
        for (int j = 0; j < SIZE; ++j)
            sum += card[cardindex][i][j];  // marked numbers are zeroed out
    return sum * lastdrawn;
}

int main(void)
{
    // Read from disk, don't time
    FILE *f = fopen(FNAME, "rb");  // fread requires binary mode
    if (!f) { fprintf(stderr, "File not found: %s\n", FNAME); return 1; }
    fread(input, sizeof input, 1, f);  // read whole file at once
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    // Parse input
    const char *c = input;
    for (int i = 0; i < CALL; ++i, ++c) {  // 100 numbers called, skip comma or newline
        int num = *c++ & 15;
        if (*c > ',')  // not comma or newline = digit
            num = num * 10 + (*c++ & 15);
        call[i] = num;
    }
    for (int k = 0; k < CARD; ++k) {  // 100 bingo cards
        c++;  // skip blank line
        for (int i = 0; i < SIZE; ++i)  // 5 rows
            for (int j = 0; j < SIZE; ++j, c += 3) {  // 5 cols
                const int num = (*c & 15) * 10 + (*(c + 1) & 15);
                card[k][i][j] = num;
                pos[num][mult[num]++] = (Pos){k, i, j};
            }
    }

    // Call out numbers until all cards have bingo
    for (int i = 0, bingocount = 0; bingocount < CARD; ++i) {
        const int num = call[i];
        const int numcount = mult[num];  // not sure if compiler even needs this hint
        // On every card where this number appears, mark it zero dude
        for (int j = 0; j < numcount; ++j) {
            const Pos *const p = &pos[num][j];  // convenience pointer
            const int k = p->card;
            const int r = p->row;
            const int c = p->col;
            card[k][r][c] = 0;  // called numbers don't count in the score
            // Mark here, not in condition, to avoid boolean shortcircuit
            markrow[k][r]++;    // one more number called in this row
            markcol[k][c]++;    // one more number called in this column
            if (!bingo[k] && (markrow[k][r] == SIZE || markcol[k][c] == SIZE)) {
                bingo[k] = true;
                ++bingocount;
                if (bingocount == 1 || bingocount == CARD)
                    printf("%d\n", score(k, num));  // 28082 8224
            }
        }
    }

#ifdef TIMER
    printf("Time: %.0f ns\n", stoptimer_ns());
#endif
}
