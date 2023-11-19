#include <stdio.h>
#include <stdint.h>    // uint32_t, uint64_t
#include <inttypes.h>  // PRIu32, PRIu64
#include <stdbool.h>
#include "startstoptimer.h"

#define POS1  1U  // from input
#define POS2 10U  // from input
#define LOOP 10U
#define WIN_PART1 1000U
#define WIN_PART2   21U
#define CACHE_SZ 88200U  // max hash index = (((9*10 + 9)*21 + 20)*21 + 20)*2 + 1 = 88199

static uint64_t cache[CACHE_SZ] = {0};  // cache of intermediate results
static bool incache[CACHE_SZ] = {0};

// Number of ways of summing to N with 3 rolls of 3-sided die [1,2,3],
// e.g. sum 4 = (1+1+2) or (1+2+1) or (2+1+1) = 3 different ways.
static const uint32_t combinations[10] = {0,0,0,1,3,6,7,6,3,1};

static uint32_t practicegame(uint32_t pos1, uint32_t pos2)
{
    uint32_t rollcount = 3, rollsum = 6, score1 = 0, score2 = 0;
    while (1) {
        if (rollcount & 1U) {
            pos1 = (pos1 - 1U + rollsum) % LOOP + 1U;
            score1 += pos1;
            if (score1 >= WIN_PART1)
                return rollcount * score2;  // use losing score
        } else {
            pos2 = (pos2 - 1U + rollsum) % LOOP + 1U;
            score2 += pos2;
            if (score2 >= WIN_PART1)
                return rollcount * score1;  // use losing score
        }
        rollcount += 3;
        rollsum += 9;
    }
}

// Only calculates the number of wins for player 1,
// so, not suitable for inputs where player 2 would win.
// (Could be done with a struct to return multiple values in one.)
static uint64_t diracdice(uint32_t pos1, uint32_t pos2, uint32_t score1, uint32_t score2, bool player1)
{
    // Return immediately if one player wins (prioritise player1)
    if (score1 >= WIN_PART2) return 1U;
    if (score2 >= WIN_PART2) return 0;

    // Return cached result if available (hash should be unique)
    uint32_t hash;
    hash = ((((pos1 - 1)*10 + pos2 - 1)*21 + score1)*21 + score2)*2 + player1;
    if (incache[hash])
        return cache[hash];

    // Check every possible progression from this state and add up all wins for player 1
    uint64_t wins = 0;
    if (player1) {
        for (uint32_t rollsum = 3; rollsum < 10; ++rollsum) {
            uint32_t p = (pos1 - 1U + rollsum) % LOOP + 1U;
            wins += combinations[rollsum] * diracdice(p, pos2, score1 + p, score2, false);
        }
    } else {
        for (uint32_t rollsum = 3; rollsum < 10; ++rollsum) {
            uint32_t p = (pos2 - 1U + rollsum) % LOOP + 1U;
            wins += combinations[rollsum] * diracdice(pos1, p, score1, score2 + p, true);
        }
    }

    // Save result and return it
    incache[hash] = true;
    cache[hash] = wins;
    return wins;
}

int main(void)
{
    starttimer();
    printf("Part 1: %"PRIu32"\n", practicegame(POS1, POS2));        // 428736
    printf("Part 2: %"PRIu64"\n", diracdice(POS1, POS2, 0, 0, 1));  // 57328067654557
    printf("%.2f ms\n", stoptimer_ms());
    return 0;
}
