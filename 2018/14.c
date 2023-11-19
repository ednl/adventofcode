#include <stdio.h>
#include <stdint.h>

#define SIZE (20 * 1024 * 1024)
#define INP 825401
static const uint8_t inp[] = {8,2,5,4,0,1};
static const size_t inplen = sizeof inp / sizeof *inp;

static uint8_t score[SIZE] = {3,7,1,0,1,0,1,2,4,5,1,5,8,9,1,6,7,7,9,2};

int main(void)
{
    uint32_t i = 4, j = 8, len = 20, test = 0, match = 0;
    while (match < inplen) {
        uint8_t sum = score[i] + score[j];
        if (sum > 9) {
            score[len++] = 1;
            score[len++] = sum - 10;
        } else
            score[len++] = sum;
        i += 1 + score[i];
        j += 1 + score[j];
        if (i >= len) i -= len;
        if (j >= len) j -= len;

        if (match) {
            if (score[test] == inp[match]) {
                ++test;
                ++match;
            } else
                match = 0;
        }
        if (!match)
            if (score[len - 1] == inp[0]) {
                test = len;
                match = 1;
            }
    }
    printf("Part 1: ");
    for (i = 0; i < 10; ++i)
        printf("%u", score[INP + i]);
    putchar('\n');  // part 1: 6289129761
    if (match == inplen)
        printf("Part 2: %u\n", test - match);  // part 2: 20207075
    return 0;
}
