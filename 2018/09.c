/**
 * Advent of Code 2018
 * Day 9: Marble Mania
 * https://adventofcode.com/2018/day/9
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 09.c
 * Enable timer:
 *     cc -std=gnu17 -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 09.c
 * Get minimum runtime from timer output:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 24 ms
 *     Mac Mini 2020 (M1 3.2 GHz)    :  ? ms
 *     Raspberry Pi 5 (2.4 GHz)      :  ? ms
*/

#include <stdio.h>
#include <stdlib.h>  // calloc, free
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2018-09-input.txt"
#define KEEP 23U  // puzzle rule

typedef unsigned int u32;

typedef struct Marble {
    struct Marble *prev, *next;
    u32 val;
} Marble;

static Marble *circle;
static u32 *score;

// Play a complete game
// ret: high score
static u32 play(const u32 players, const u32 marbles)
{
    Marble *cur, *p, *q;
    u32 i, freeindex = 0, num = 0, player = players - 1;  // start with player = -1

    // Init zeroth marble
    cur = &circle[freeindex++];
    *cur = (Marble){cur, cur, num++};

    // Reset scores
    for (i = 0; i < players; ++i)
        score[i] = 0;

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
            score[player % players] += num++ + p->val;
            // Remove marble;
            q->next = cur;
            cur->prev = q;
        }
    }

    // Determine hiscore
    u32 hi = 0;
    for (i = 0; i < players; ++i)
        if (score[i] > hi)
            hi = score[i];
    return hi;
}

int main(void)
{
    FILE *f = fopen(FNAME, "r");
    if (!f)
        return 1;
    u32 inp1 = 0, inp2 = 0;
    if (fscanf(f, "%u players; last marble is worth %u", &inp1, &inp2) != 2)
        return 2;
    fclose(f);

    const u32 players = inp1;
    const u32 marbles1 = inp2;
    const u32 marbles2 = marbles1 * 100;  // part 2

    circle = calloc(marbles2, sizeof *circle);
    score = calloc(players, sizeof *score);

#ifdef TIMER
    starttimer();
#endif

    printf("Part 1: %u\n", play(players, marbles1));  // 390093
    printf("Part 2: %u\n", play(players, marbles2));  // 3150377341

#ifdef TIMER
    printf("Time: %.0f ms\n", stoptimer_ms());
#endif
    free(score);
    free(circle);
}
