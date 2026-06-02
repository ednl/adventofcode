/**
 * Advent of Code 2020
 * Day 4: Passport Processing
 * https://adventofcode.com/2020/day/4
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic -Wno-multichar 04.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -Wno-multichar -DTIMER ../startstoptimer.c 04.c
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 16.7 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 33.8 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 42.7 µs
 */

#include <stdio.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2020-04-input.txt"
#define FSIZE 24000  // needed for my input: 21282 (+2 '\0')

// Birth Year, Issue Year, Expiration Year,
//   Height, Hair Color, Eye Color, Passport ID
// Optional so left out: Country ID
typedef enum field {
    BYR, IYR, EYR, HGT, HCL, ECL, PID
} Field;

static char input[FSIZE];

// Parse non-negative decimal number in ASCII,
// advance char pointer, stop at first non-digit
static int parseint(const char **s)
{
    int x = 0;
    while (**s >= '0' && **s <= '9')
        x = x * 10 + (*(*s)++ & 15);
    return x;
}

// Skip faulty data, return zero
static unsigned err(const char **s)
{
    for (; **s != '\n' && **s != ' '; (*s)++);
    return 0;
}

// Validate number in range
static unsigned year(const char **s, const int min, const int max)
{
    (*s) += 4;  // skip key
    const int yr = parseint(s);  // stop at first non-digit
    if (**s == '\n' || **s == ' ')
        return min <= yr && yr <= max;
    return err(s);
}

// Four digits; at least 1920 and at most 2002
static unsigned byr(const char **s)
{
    return year(s, 1920, 2002) << BYR;
}

// Four digits; at least 2010 and at most 2020
static unsigned iyr(const char **s)
{
    return year(s, 2010, 2020) << IYR;
}

// Four digits; at least 2020 and at most 2030
static unsigned eyr(const char **s)
{
    return year(s, 2020, 2030) << EYR;
}

// Exactly one of: amb blu brn gry grn hzl oth
static unsigned ecl(const char **s)
{
    (*s) += 4;  // skip key
    unsigned val = 0;
    while (**s != '\n' && **s != ' ')
        val = val << 8 | *(*s)++;
    switch (val) {
        // Multichar constants are compiler extension (here: little-endian)
        case 'amb': /* fall-through */
        case 'blu': /* fall-through */
        case 'brn': /* fall-through */
        case 'grn': /* fall-through */
        case 'gry': /* fall-through */
        case 'hzl': /* fall-through */
        case 'oth': return 1U << ECL;
    }
    return 0;
}

// '#' followed by exactly six characters 0-9 or a-f
static unsigned hcl(const char **s)
{
    (*s) += 4;  // skip key
    if (**s != '#')
        return err(s);
    int count = 0;
    for ((*s)++; (**s >= '0' && **s <= '9') || (**s >= 'a' && **s <= 'f'); count++, (*s)++);
    if (**s == '\n' || **s == ' ')
        return count == 6 ? 1U << HCL : 0;
    return err(s);
}

// Number followed by either cm or in:
// - if cm, the number must be at least 150 and at most 193
// - if in, the number must be at least 59 and at most 76
static unsigned hgt(const char **s)
{
    (*s) += 4;  // skip key
    const int h = parseint(s);
    unsigned unit = 0;
    for (; **s != '\n' && **s != ' '; (*s)++)
        unit = unit << 8 | **s;
    switch (unit) {
        case 'cm': return h >= 150 && h <= 193 ? 1U << HGT : 0;
        case 'in': return h >=  59 && h <=  76 ? 1U << HGT : 0;
    }
    return 0;
}

// Nine-digit number, including leading zeroes
static unsigned pid(const char **s)
{
    (*s) += 4;  // skip key
    int count = 0;
    for (; **s >= '0' && **s <= '9'; count++, (*s)++);
    if (**s == '\n' || **s == ' ')
        return count == 9 ? 1U << PID : 0;
    return err(s);
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    fread(input, 1, FSIZE, f);  // read char-by-char to max size
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    int valid1 = 0, valid2 = 0;
    unsigned present = 0, correct = 0;
    for (const char *c = input; *c; ) {
        switch (*c << 8 | *(c + 1)) {  // *(i16*)c is technically UB, and it reverses the case constants
            // Multichar constants are compiler extension (here: little-endian)
            case 'by': present |= 1U << BYR; correct |= byr(&c); break;
            case 'ec': present |= 1U << ECL; correct |= ecl(&c); break;
            case 'ey': present |= 1U << EYR; correct |= eyr(&c); break;
            case 'hc': present |= 1U << HCL; correct |= hcl(&c); break;
            case 'hg': present |= 1U << HGT; correct |= hgt(&c); break;
            case 'iy': present |= 1U << IYR; correct |= iyr(&c); break;
            case 'pi': present |= 1U << PID; correct |= pid(&c); break;
            default: for (c += 4; *c != '\n' && *c != ' '; c++); break;  // skip optional 'cid' key/val
        }
        c++;  // skip space or newline to next field, or to empty line
        if (*c == '\0' || *c == '\n') {  // end of passport
            valid1 += present == 127U;
            valid2 += correct == 127U;
            present = correct = 0;  // reset for next password
            c++;  // skip null or newline (input[] must be at least 2 longer than data)
        }
    }
    printf("%d %d\n", valid1, valid2);  // 245 133

#ifdef TIMER
    printf("Time: %.0f ns\n", stoptimer_ns());
#endif
}
