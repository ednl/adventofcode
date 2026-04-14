#include <stdio.h>
#include "../startstoptimer.h"

#define FNAME "../aocinput/2018-14-input.txt"
#define SIZE (20 * 1024 * 1024)  // 20971520, needed for my input: 20207082
#define LEN 20  // initial scoreboard size
#define ELF1 8  // initial position of elf 1
#define ELF2 4  // initial position of elf 2

static char score[SIZE] = {3,7,1,0,1,0,1,2,4,5,1,5,8,9,1,6,7,7,9,2};  // initial scoreboard

int main(void)
{
    char inp[8] = {0};
    int inplen = 0, inpval = 0;

    FILE *f = fopen(FNAME, "r");
    if (!f) return 1;
    for (int c; (c = fgetc(f)) >= '0'; ) {
        const int digit = c & 15;
        inp[inplen++] = digit;
        inpval = inpval * 10 + digit;
    }
    fclose(f);

    starttimer();

    int test = 0, match = 0;
    for (int i = ELF1, j = ELF2, len = LEN; match < inplen; ) {
        const char sum = score[i] + score[j];
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

    for (int i = 0; i < 10; ++i)
        putchar('0' + score[inpval + i]);
    putchar('\n');  // part 1: 6289129761
    if (match == inplen)
        printf("%d\n", test - match);  // part 2: 20207075

    printf("Time: %.0f us\n", stoptimer_us());
}
