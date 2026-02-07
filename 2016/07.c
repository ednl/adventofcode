/**
 * Advent of Code 2016
 * Day 7: Internet Protocol Version 7
 * https://adventofcode.com/2016/day/7
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile with warnings:
 *     cc -std=c17 -Wall -Wextra -pedantic 07.c
 * Compile for speed, with timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 07.c
 * Run program:
 *     ./a.out                  read input file from internal file name
 *     ./a.out < input.txt      read input file using redirected input
 *     cat input.txt | ./a.out  read input file using piped input
 * Get minimum runtime from timer output in bash:
 *     m=9999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 *     (optionally replace './a.out' with 2nd or 3rd run command above)
 *     output redirection is needed to only select the timing info, which is printed to stderr
 * Minimum runtime measurements including result output which is redirected to /dev/null in shell:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 153 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :   ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : 522 µs
 */

#include <stdio.h>
#include <unistd.h>  // isatty, fileno
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
    #define LOOP 1000
#endif

#define FNAME "../aocinput/2016-07-input.txt"
#define LINES    2000  // 2000 lines in input file
#define IPMAXLEN  160  // max needed for my input: 136+"\n\0"=138
#define SEQMINLEN  15  // every sequence has length 15..19
#define ALPHLEN ('z' - 'a' + 1)

static char input[LINES][IPMAXLEN];

// aba[x][y] = last line number 1..2000 where pattern 'xyx' appeared in a supernet sequence
// bab[y][x] = last line number 1..2000 where pattern 'yxy' appeared in a hypernet sequence
static int aba[ALPHLEN][ALPHLEN];
static int bab[ALPHLEN][ALPHLEN];

// end - s must be >= 4 (length of every section in input is >= 15)
static bool has_abba(const char *s, const char *end)
{
    for (end -= 3; s != end; ++s)
        if (*s == *(s + 3) && *(s + 1) == *(s + 2) && *s != *(s + 1))
            return true;  // any occurence is good, so we're done here
    return false;
}

// line in 1..2000 for supernet
static bool has_aba(const char *s, const char *end, const int line)
{
    for (end -= 2; s != end; ++s)
        if (*s == *(s + 2) && *s != *(s + 1)) {
            const int i = *s - 'a';
            const int j = *(s + 1) - 'a';
            if (bab[j][i] == line)  // opposite pattern in opposite sequence on this line?
                return true;        // any match is good, so we're done
            aba[i][j] = line;       // save pattern on this line
        }
    return false;
}

// line in 1..2000 for hypernet
static bool has_bab(const char *s, const char *end, const int line)
{
    for (end -= 2; s != end; ++s)
        if (*s == *(s + 2) && *s != *(s + 1)) {
            const int i = *s - 'a';
            const int j = *(s + 1) - 'a';
            if (aba[j][i] == line)  // opposite pattern in opposite sequence on this line?
                return true;        // any match is good, so we're done
            bab[i][j] = line;       // save pattern on this line
        }
    return false;
}

int main(void)
{
    int lines = 0;
    if (isatty(fileno(stdin))) {
        // Read lines of input file from disk
        FILE *f = fopen(FNAME, "r");
        if (!f) { fprintf(stderr, "File not found: "FNAME"\n"); return 1; }
        for (; lines < LINES && fgets(&input[lines][0], IPMAXLEN, f); ++lines);
        fclose(f);
    } else
        // Read lines of input or example file from pipe or redirected stdin
        for (; lines < LINES && fgets(&input[lines][0], IPMAXLEN, stdin); ++lines);

#ifdef TIMER
    starttimer(); for (int loop = 0; loop < LOOP; ++loop) {
#endif

    int tls = 0, ssl = 0;
    for (int line = 0; line < lines; ) {
        const char *cur = &input[line++][0];
        // line numbers 1-based from here, to distinguish from zeroed static value
        bool abba = false;  // 'abba' pattern detected in supernet sequence?
        bool baab = false;  // 'abba' pattern detected in hypernet sequence?
        bool supports_ssl = false;  // aba-bab pattern pair detected?
        for (bool supernet = true; *cur; supernet = !supernet) {
            // Look for end of current (start of next) sequence
            const char *next = cur + SEQMINLEN;
            while (*next >= 'a')  // until bracket or newline
                ++next;
            if (supernet) {
                // supernet sequence = outside brackets
                if (!abba && !baab)
                    abba = has_abba(cur, next);
                if (!supports_ssl)
                    supports_ssl = has_aba(cur, next, line);
            } else {
                // hypernet sequence = inside brackets
                if (!baab)
                    baab = has_abba(cur, next);
                if (!supports_ssl)
                    supports_ssl = has_bab(cur, next, line);
            }
            cur = next + 1;  // skip bracket or newline
        }
        tls += abba && !baab;
        ssl += supports_ssl;
    }
    printf("%d %d\n", tls, ssl);  // 115 231

#ifdef TIMER
    } fprintf(stderr, "Time: %.0f us\n", stoptimer_ms()); // loop 1000x: ms=µs
#endif
}
