#include <stdio.h>

// Values from my input file
#define PLAYERS      471U
#define MARBLES1   72026U
#define MARBLES2 7202600U  // x100

typedef struct Marble {
    struct Marble *prev, *next;
    unsigned int val;
} Marble;

static Marble circle[MARBLES2] = {0};
static unsigned int score[PLAYERS] = {0};

// Play a complete game
// ret: high score
static unsigned int play(unsigned int marbles)
{
    Marble *cur, *p, *q;
    unsigned int i, freeindex = 0, num = 0, player = PLAYERS - 1;  // start with player = -1

    // Init zeroth marble
    cur = &circle[freeindex++];
    *cur = (Marble){cur, cur, num++};

    // Reset scores
    for (i = 0; i < PLAYERS; ++i) {
        score[i] = 0;
    }

    // Use all the marbles
    while (num <= marbles) {
        // Insert 22 marbles
        i = 0;
        while (i++ < 22U && num <= marbles) {
            // Get left & rigt of new insertion point
            p = cur->next;
            q = p->next;
            // Make new marble
            cur = &circle[freeindex++];
            *cur = (Marble){p, q, num++};
            // Insert new marble
            p->next = cur;
            q->prev = cur;
        }
        // Remove the 23rd marble
        if (num <= marbles) {
            // Get right neighbour of marble to remove
            for (i = 0; i < 6; ++i) {
                cur = cur->prev;
            }
            // Get marble to remove & left neighbour
            p = cur->prev;
            q = p->prev;
            // Give points
            player += 23U;
            score[player % PLAYERS] += num++ + p->val;
            // Remove marble;
            q->next = cur;
            cur->prev = q;
        }
    }

    // Determine hiscore
    unsigned int hi = 0;
    for (i = 0; i < PLAYERS; ++i) {
        if (score[i] > hi) {
            hi = score[i];
        }
    }
    return hi;
}

int main(void)
{
    printf("Part 1: %u\n", play(MARBLES1));
    printf("Part 2: %u\n", play(MARBLES2));
    return 0;
}
