/**
 * Advent of Code 2016
 * Day 4: Security Through Obscurity
 * https://adventofcode.com/2016/day/4
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile with warnings:
 *     cc -std=c17 -Wall -Wextra -pedantic ../topn.c 04.c
 * Compile for speed, with timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c ../topn.c 04.c
 * Run program:
 *     ./a.out                  read input file from internal file name
 *     ./a.out < input.txt      read input file using redirected input
 *     cat input.txt | ./a.out  read input file using piped input
 * Get minimum runtime from timer output in bash:
 *     m=9999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 *     (optionally replace './a.out' with 2nd or 3rd run command above)
 *     output redirection is needed to only select the timing info, which is printed to stderr
 * Minimum runtime measurements including result output which is redirected to /dev/null in shell:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  93 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :   ? µs
 *     Raspberry Pi 5 (2.4 GHz)      :   ? µs
 */

#include <stdio.h>
#include <unistd.h>  // isatty, fileno
#include <stdint.h>  // int16_t, int8_t
#include "../topn.h"
#ifdef TIMER
    #include "../startstoptimer.h"
    #define LOOP 1000
#endif

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define FNAME "../aocinput/2016-04-input.txt"
#define LINES 1024  // needed for my input: 935
#define LINELEN 80  // needed for my input: 66+2=68

#define ALPHLEN ('z' - 'a' + 1)
#define NAMELEN  64  // needed for my input: 56
#define CHKSUMLEN 5  // always 5

// Assumes little-endian system
typedef union hist {
    int16_t val;
    struct { int8_t bin, count; };
} Hist;

static char input[LINES][LINELEN];
static Hist hist[ALPHLEN];

// Sort by count descending, letter ascending
static int int16_desc(const void *p, const void *q)
{
    const int16_t a = *(const int16_t *)p;
    const int16_t b = *(const int16_t *)q;
    if (a > b) return -1;
    if (a < b) return +1;
    return 0;
}

static char caesar(const char encrypted, const int decyphershift)
{
    return 'a' + (encrypted - 'a' + decyphershift) % ALPHLEN;
}

int main(void)
{
    int lines = 0;
    if (isatty(fileno(stdin))) {
        // Read lines of input file from disk
        FILE *f = fopen(FNAME, "r");
        if (!f) { fprintf(stderr, "File not found: "FNAME"\n"); return 1; }
        for (; lines < LINES && fgets(&input[lines][0], LINELEN, f); ++lines);
        fclose(f);
    } else
        // Read lines of input or example file from pipe or redirected stdin
        for (; lines < LINES && fgets(&input[lines][0], LINELEN, stdin); ++lines);

#ifdef TIMER
    starttimer(); for (int loop = 0; loop < LOOP; ++loop) {
#endif

    int sectorsum = 0;
    for (int line = 0; line < lines; ++line) {
        const char *name = &input[line][0];
        const char *s = name;

        // Reset histogram array
        for (int i = 0; i < ALPHLEN; ++i)
            hist[i] = (Hist){(ALPHLEN - 1) - i};  // sorting value of bin i = 25-i, to turn desc into asc

        // Letter frequencies
        for (; *s >= 'a' || *s == '-'; ++s)
            if (*s != '-')
                hist[*s - 'a'].count++;  // count the letters in bins from 0=a to 25=z

        // Remember start of Sector ID
        const char *const id = s;

        // Sort histogram
        topn(hist, CHKSUMLEN, ALPHLEN, sizeof *hist, int16_desc);

        // Compare checksum to the 5 most frequent letters
        int match = 0;
        for (s += 4; match < CHKSUMLEN && *s == 'z' - hist[match].bin; ++s, ++match);

        // If valid checksum then sum Sector ID and decrypt name
        if (match == CHKSUMLEN) {
            // Part 1
            const int sectorid = (*id & 15) * 100 + (*(id + 1) & 15) * 10 + (*(id + 2) & 15);
            sectorsum += sectorid;
            // Part 2
            const char c = caesar(*name, sectorid);
            if (c == 'n') {
                printf("Part 2: %d (%c", sectorid, c);  // 501 (northpole object storage)
                for (++name; name < id - 1; ++name)
                    if (*name != '-')
                        putchar(caesar(*name, sectorid));
                    else
                        putchar(' ');
                puts(")");
            }
        }
    }
    printf("Part 1: %d\n", sectorsum);  // 137896

#ifdef TIMER
    } fprintf(stderr, "Time: %.0f us\n", stoptimer_ms()); // loop 1000x: ms=µs
#endif
}
