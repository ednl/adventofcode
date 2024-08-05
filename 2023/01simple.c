/**
 * Advent of Code 2023
 * Day 1: Trebuchet?!
 * https://adventofcode.com/2023/day/1
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -Ofast -march=native -Wall -Wextra 01simple.c
 *    gcc   -std=gnu17 -Ofast -march=native -Wall -Wextra 01simple.c
 */

#include <stdio.h>
#include <string.h>  // strlen
#include <ctype.h>   // isdigit

// Digit count from 0 to 9
#define DIGITS 10

// Max line length in input file
#define LINELEN 64

static const char *word[DIGITS] = {
    "",  // not in input, only used to align index with word
    "one", "two", "three", "four", "five", "six", "seven", "eight", "nine"
};
static int wordlen[DIGITS];

int main(void)
{
    // Init
    for (int i = 1; i < DIGITS; ++i)  // zero not used
        wordlen[i] = strlen(word[i]);

    // Open input file
    FILE *f = fopen("../aocinput/2023-01-input.txt", "r");
    if (!f)
        return 1;

    int sum1 = 0, sum2 = 0;
    char buf[LINELEN];
    while (fgets(buf, sizeof buf, f)) {
        // Part 1
        int firstdigitindex = LINELEN, lastdigitindex = -1;  // invalid values
        char *c = buf;
        for (int i = 0; *c != '\n'; ++i, ++c)  // test every char in string
            if (isdigit(*c)) {
                if (i < firstdigitindex)  // only update if not set yet
                    firstdigitindex = i;
                lastdigitindex = i;  // always update to get last digit
            }
        // Values
        int firstdigit = buf[firstdigitindex] - '0';
        int lastdigit = buf[lastdigitindex] - '0';
        // Sum
        sum1 += firstdigit * 10 + lastdigit;

        // Part 2
        buf[firstdigitindex] = '\0';  // make sure first word search stops here
        int firstwordindex = LINELEN, lastwordindex = -1;
        int firstword = 0, lastword = 0;  // initialising only to be safe
        for (int i = 1; i < DIGITS; ++i) {  // search for every word
            // First word
            char *pos = strstr(buf, word[i]);  // search from start
            if (pos) {  // if found
                int j = pos - buf;  // index into buf
                if (j < firstwordindex) {  // smaller or not set yet
                    firstwordindex = j;
                    firstword = i;
                }
            }
            // Last word
            int j = lastdigitindex + 1;  // search from (1 past) last digit
            while ((pos = strstr(&buf[j], word[i]))) {  // loop because same word can be repeated
                int k = pos - buf;  // index into buf
                if (k > lastwordindex) {  // larger or not set yet
                    lastwordindex = k;
                    lastword = i;
                }
                j += wordlen[i];  // same words have no overlap
            }
        }
        // Values
        int first = firstdigitindex < firstwordindex ? firstdigit : firstword;
        int last = lastdigitindex > lastwordindex ? lastdigit : lastword;
        // Sum
        sum2 += first * 10 + last;
    }
    fclose(f);

    // Result
    printf("%d %d\n", sum1, sum2);  // 54630 54770 for my input
    return 0;
}
