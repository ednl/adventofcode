/**
 * Advent of Code 2020
 * Day 22: Crab Combat
 * https://adventofcode.com/2020/day/22
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 22alt.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 22alt.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : ? ms
 *     Mac Mini 2020 (M1 3.2 GHz)    : ? ms
 *     Raspberry Pi 5 (2.4 GHz)      : ? ms
 */

#include <stdio.h>
#include <stdint.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2020-22-input.txt"
#define FSIZE 162  // same size for everyone: 1-50 divided up
#define N (1 << 6)  // 64
#define M (N - 1)   // 63
#define D 50  // deck size, value 1..50

typedef uint8_t u8;
typedef struct hand {
    int head, tail, size;
    u8 card[N];  // 16x sizeof (int)
} Hand;
typedef union game {
    struct { Hand player0, player1; };
    Hand player[2];
} Game;

static char input[FSIZE];

static u8 pop(Hand *const p)
{
    p->size--;
    return p->card[p->head++ & M];
}

static void push(Hand *const p, const u8 card1, const u8 card2)
{
    p->card[p->tail++ & M] = card1;
    p->card[p->tail++ & M] = card2;
    p->size += 2;
}

static int score(const Hand *const p)
{
    int sum = 0;
    for (int k = p->head, w = p->size; k < p->tail; --w, ++k)
        sum += p->card[k & M] * w;
    return sum;
}

// Play one game of Crab Combat
// return score of winner
static int combat1(Hand *const p0, Hand *const p1)
{
    while (p0->size && p1->size) {
        const u8 card0 = pop(p0);
        const u8 card1 = pop(p1);
        if (card0 > card1)
            push(p0, card0, card1);
        else
            push(p1, card1, card0);
    }
    return score(p0->size ? p0 : p1);
}

// Play recursive games of Crab Combat
// return: 0 = p0 won, 1 = p1 won
static int combat2(Hand *const p0, Hand *const p1)
{
    while (p0->size && p1->size) {
        //
    }
    return p1->size != 0;
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    fread(input, FSIZE, 1, f);
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    Game deal = {0};
    const char *c = input;
    const char *const end = input + FSIZE;
    for (int i = 0; i < 2; ++i) {
        c += 10;  // skip "Hand x:\n"
        int n = 0;  // number of cards in this hand
        for (; c != end && *c != '\n'; ++n)
            if (*(c + 1) != '\n') {
                deal.player[i].card[n] = *c * 10 + *(c + 1) - 11 * '0';
                c += 3;
            } else {
                deal.player[i].card[n] = *c & 15;
                c += 2;
            }
        deal.player[i].tail = deal.player[i].size = n;
        c++;  // skip empty line
    }

    // Part 1
    Game game = deal;
    printf("%d\n", combat1(&game.player0, &game.player1));  // part 1: 31629

    // Part 2
    const int win = combat2(&deal.player0, &deal.player1);
    const Hand *const winner = &deal.player[win];
    printf("%d\n", score(winner));  // part 2: 35196

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
