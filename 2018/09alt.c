#include <stdio.h>
#include <stdlib.h>

#define KEEP 23U
// Values from my input file
#define PLAYERS    471U
#define MARBLES1 72026U
// Derived values
#define MARBLES2 (MARBLES1 * 100U)
#define LEN      (MARBLES2 * 2U)

static unsigned int score[PLAYERS] = {0};
static unsigned int line[LEN] = {0};

int main(void)
{
    unsigned int i = 0, j = 1, n = 1, m = KEEP + 1, k = KEEP, hi = 0;

    for (unsigned int t = 0; t < 6U; ++t) {
        line[j++] = line[i++];      // skip one
        line[j++] = n++;            // 1-6
    }

    while (k <= MARBLES1) {
        for (unsigned int t = 0; t < 12U; ++t) {
            line[j++] = line[i++];  // skip one
            line[j++] = n++;        // 7-18
        }

        // Keep multiple of 23 + take marble at index i
        score[k % PLAYERS] += k + line[i];

        line[i] = n++;              // 19
        line[j++] = line[i++];      // skip one

        for (unsigned int t = 0; t < 3U; ++t) {
            line[j++] = line[i++];  // skip one
            line[j++] = m++;        // 24, 26, 28
            line[j++] = n++;        // 20, 21, 22
            line[j++] = m++;        // 25, 27, 29
        }

        n += KEEP - 16U;  // 16 already counted, need 7 more to make 23
        m += KEEP - 6U;   // 6 already counted, need 17 more to make 23
        k += KEEP;
    }

    for (unsigned int t = 0; t < PLAYERS; ++t) {
        if (score[t] > hi)
            hi = score[t];
    }
    printf("Part 1: %u\n", hi);

    while (k <= MARBLES2) {
        for (unsigned int t = 0; t < 12U; ++t) {
            line[j++] = line[i++];  // skip one
            line[j++] = n++;        // 7-18
        }

        // Keep multiple of 23 + take marble at index i
        score[k % PLAYERS] += k + line[i];

        line[i] = n++;              // 19
        line[j++] = line[i++];      // skip one

        for (unsigned int t = 0; t < 3U; ++t) {
            line[j++] = line[i++];  // skip one
            line[j++] = m++;        // 24, 26, 28
            line[j++] = n++;        // 20, 21, 22
            line[j++] = m++;        // 25, 27, 29
        }

        n += KEEP - 16U;  // 16 already counted, need 7 more to make 23
        m += KEEP - 6U;   // 6 already counted, need 17 more to make 23
        k += KEEP;
    }

    for (unsigned int t = 0; t < PLAYERS; ++t) {
        if (score[t] > hi)
            hi = score[t];
    }
    printf("Part 2: %u\n", hi);
    printf("j=%u N/23*37=%u\n", j, MARBLES2 / 23 * 37);
    return 0;
}
