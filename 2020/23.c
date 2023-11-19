#include <stdio.h>     // printf
#include <stdint.h>    // uint32_t, UINT32_C, uint64_t, UINT64_C
#include <inttypes.h>  // PRIu32, PRIu64

#define LABELS   (UINT32_C(9))
#define CUPCOUNT (UINT32_C(1000000))
#define MOVES    (UINT32_C(10000000))

static const uint_fast32_t init[LABELS] = {3, 6, 2, 9, 8, 1, 7, 5, 4};  // puzzle input
static uint_fast32_t next[CUPCOUNT + 1];  // index 0 not used, so, need one extra at the end

int main(void)
{
    uint_fast32_t i, moves, cur, ins, p1, p2, p3;

    // Arrange all cups in a circle (cup 0 is extra)
    // where "next[1] = 2" means that cup 2 comes after cup 1
    next[0] = next[CUPCOUNT] = init[0];
    for (i = 1; i < CUPCOUNT; ++i) {
        next[i] = i + 1;
    }

    // Use the puzzle input to seed the first few cup labels
    for (i = 0; i < LABELS - 1; ++i) {
        next[init[i]] = init[i + 1];
    }
    // And the last one; in a tight circle or are there more cups?
    next[init[LABELS - 1]] = CUPCOUNT == LABELS ? init[0] : LABELS + 1;

    cur = 0;  // entry point to the first cup in the circle
    moves = MOVES;
    while (moves--) {
        cur = next[cur];
        ins = cur - 1;
        if (!ins)
            ins = CUPCOUNT;
        p1 = next[cur];
        p2 = next[p1];
        p3 = next[p2];
        while (ins == p1 || ins == p2 || ins == p3) {
            --ins;
            if (!ins)
                ins = CUPCOUNT;
        }
        i = next[p3];
        next[p3] = next[ins];
        next[ins] = next[cur];
        next[cur] = i;
    }

    #if (CUPCOUNT < 10)
        i = next[1];
        while (i != 1) {
            printf("%"PRIuFAST32, i);
            i = next[i];
        }
        printf("\n");
    #else
        printf("%"PRIu64"\n", (uint64_t)next[1] * next[next[1]]);
    #endif

    return 0;
}
