#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define DRAW 100  // how many numbers drawn, range 0..DRAW-1
#define CARD 100  // number of cards in the game
#define DIM    5  // number of rows and columns on every card

static unsigned char draw[DRAW] = {0};            // numbers drawn
static unsigned char card[CARD][DIM][DIM] = {0};  // bingo cards
static unsigned char crossrow[CARD][DIM] = {0};   // count crosses in rows
static unsigned char crosscol[CARD][DIM] = {0};   // count crosses in cols

// Reverse index to quickly locate a number on any card
// For every number, a (single linked) list keeps track of where it is on which cards
typedef struct Index {
    struct Index *next;
    unsigned char card, row, col;
} Index;
static Index *index[DRAW] = {0};

int main(void)
{
    FILE *f = fopen("../aocinput/2021-04-input.txt", "r");
    unsigned int i = 0;
    unsigned char a;
    if (fscanf(f, "%hhu", &a) == 1)
        draw[i++] = a;  // first number drawn
    while (i < DRAW && fscanf(f, ",%hhu", &a) == 1)
        draw[i++] = a;  // 99 more numbers drawn with a comma separator

    unsigned char b = 0, r = 0, c = 0;  // b = bingo card, r = row, c = column
    Index *p, *q;
    while (b < CARD && fscanf(f, " %hhu", &a) == 1) {  // read next number with any whitespace separation
        // Fill in the cards
        card[b][r][c] = a;

        // Enter a reference in the reverse index
        p = malloc(sizeof(Index));
        *p = (Index){NULL, b, r, c};
        if ((q = index[a])) {  // already an entry for this number?
            while (q->next)
                q = q->next;
            q->next = p;  // add to end of list (order not important)
        } else {
            index[a] = p;  // first entry into the array of linked list base pointers
        }

        // Next position on the card, or next card
        if (++c == DIM) {
            c = 0;
            if (++r == DIM) {
                r = 0;
                ++b;
            }
        }
    }
    fclose(f);

    unsigned char wins = 0;   // total number of cards that won
    bool won[CARD] = {false};
    for (i = 0; i < DRAW; ++i) {
        p = index[draw[i]];  // first entry in reverse index of number drawn
        while (p) {          // as long as there are entries
            // Only process cards that have not yet won
            if (!won[p->card]) {
                // Cross out = set number on card to 100 (bigger than any number used)
                card[p->card][p->row][p->col] = DRAW;
                crossrow[p->card][p->row]++;  // can't do this in the "if" because of short-circuit evaluation
                crosscol[p->card][p->col]++;
                // Full row or column?
                if (crossrow[p->card][p->row] == DIM || crosscol[p->card][p->col] == DIM) {
                    ++wins;
                    won[p->card] = true;
                    // Calculate score for first and last wins
                    if (wins == 1 || wins == DRAW) {
                        unsigned int cardsum = 0;
                        for (unsigned int j = 0; j < DIM; ++j)
                            for (unsigned int k = 0; k < DIM; ++k)
                                if (card[p->card][j][k] < DRAW)  // not crossed out?
                                    cardsum += card[p->card][j][k];
                        // Part 1: 28082
                        // Part 2: 8224
                        printf("Part %d: %u\n", 1 + (wins == DRAW), draw[i] * cardsum);
                    }
                }
            }
            p = p->next;  // next entry in reverse index of number drawn
        }
    }

    // Free allocated memory
    for (i = 0; i < DRAW; ++i) {
        p = index[i];
        while (p) {
            q = p->next;
            free(p);
            p = q;
        }
    }
    return 0;
}

