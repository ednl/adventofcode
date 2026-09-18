/**
 * Advent of Code 2023
 * Day 18: Lavaduct Lagoon
 * https://adventofcode.com/2023/day/18
 * By: E. Dronkert https://github.com/ednl
 *
 * Theory:
 *     https://en.wikipedia.org/wiki/Shoelace_formula
 *     https://en.wikipedia.org/wiki/Pick%27s_theorem
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 18.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 18.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz)    : 0.93 µs
 *     Apple M1 Mac Mini 2020 (3.2 GHz) :    ? µs
 *     Raspberry Pi 5 (2.4 GHz)         :    ? µs
 */

#include <stdio.h>
#include <stdint.h>    // int64_t, uint8_t
#include <inttypes.h>  // PRIu64
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2023-18-input.txt"
#define FSIZE ((1<<13)|(1<<12))  // 12288, needed for my input: 10759

typedef struct lava { int64_t area, border, ypos; } Lava;

static const int hexval[] = {
    ['0']=0, ['1']=1, ['2']= 2, ['3']= 3, ['4']= 4, ['5']= 5, ['6']= 6, ['7']= 7,
    ['8']=8, ['9']=9, ['a']=10, ['b']=11, ['c']=12, ['d']=13, ['e']=14, ['f']=15,
};

static char input[FSIZE];
static Lava part1, part2;

// Value of decimal number in range 2..11
static int dec(const char **s)
{
    int x;
    if (*(*s + 3) == ' ') {
        x = *(*s + 2) & 15;  // 2..9
        *s += 6;  // skip "X x (#"
    } else {
        x = 10 | (*(*s + 3) & 1);  // 10,11
        *s += 7;  // skip "X xx (#"
    }
    return x;
}

// Value of 5-digit lowercase hex number
static int hex(const char *const s)
{
    return hexval[(uint8_t)*s     ] << 16
        | hexval[(uint8_t)*(s + 1)] << 12
        | hexval[(uint8_t)*(s + 2)] << 8
        | hexval[(uint8_t)*(s + 3)] << 4
        | hexval[(uint8_t)*(s + 4)];
}
// Shoelace formula: A = 1/2 . sum((y[i] + y[i+1]).(x[i] - x[i+1]))
// For two points on horizontal line: y[i] = y[i+1], so y[i] + y[i+1] = 2y
// For two points on vertical line  : x[i] = x[i+1], so x[i] - x[i+1] = 0
// Bring the 1/2 in the sum: A = sum(y.dx) for horizontal lines only.
//   'R': area -= ypos * len
//   'L': area += ypos * len
static void horz(const char **s)
{
    // Part 1
    const int dir = 1 - (**s & 3);  // 'R'=-1, 'L'=+1
    int len = dec(s);
    part1.area += part1.ypos * len * dir;
    part1.border += len;
    // Part 2
    len = hex(*s);
    part2.area += part2.ypos * len * (1 - (*(*s + 5) & 3));  // '0'=R=-1, '2'=L=+1
    part2.border += len;
}

// See `horz()` but now vertical
//   'D': ypos += len
//   'U': ypos -= len
static void vert(const char **s)
{
    // Part 1
    const int dir = 1 - (**s << 1 & 2);  // 'D'=+1, 'U'=-1
    int len = dec(s);
    part1.ypos += len * dir;
    part1.border += len;
    // Part 2
    len = hex(*s);
    part2.ypos += len * (2 - (*(*s + 5) & 3));  // '1'=D=+1, '3'=U=-1
    part2.border += len;
}

// Pick's theorem: i = A - b/2 + 1, but add border b
// A can be negative, depending on orientation of contour
// (but positive for both parts of my input)
static int64_t pick(const Lava lava)
{
    return (lava.area > 0 ? lava.area : -lava.area) + (lava.border >> 1) + 1;
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) { fputs("File not found: "FNAME, stderr); return 1; }
    fread(input, 1, FSIZE, f);  // read single bytes until EOF
    fclose(f);

#ifdef TIMER
starttimer();
for (unsigned TIMERLOOP = 1000; TIMERLOOP--; ) {
    part1 = part2 = (Lava){0};
#endif

    for (const char *c = input; *c; ) {
        horz(&c); c += 8;
        vert(&c); c += 8;
    }
    printf("%"PRIu64" %"PRIu64"\n", pick(part1), pick(part2));  // 46334 102000662718092

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
