/**
 * Advent of Code 2022
 * Day 2: Rock Paper Scissors
 * https://adventofcode.com/2022/day/2
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 02.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 02.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 0.77 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :    ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : 3.11 µs
 */

#include <stdio.h>
#include <stdint.h>  // uint32_t
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2022-02-input.txt"
#define LINES 2500

#define ROCK 1
#define PAPR 2
#define SCIS 3
#define LOSS 0
#define DRAW 3
#define WIN  6

// 'A' = 0x41 = 0b01000001, 'X' = 0x58 = 0b01011000, space = 0x20, newline = 0x0a
// so "A X" with newline = 4 bytes = [0x41, 0x20, 0x58, 0x0a] = 0x0a582041 (little-endian)
// Bits 17 and 16 are: 00 (X), 01 (Y) or 10 (Z)
// Bits  1 and  0 are: 01 (A), 10 (B) or 11 (C)
// Make 4-bit index (values 0-11 used) from bits 17,16,1,0:
//   --, AX, BX, CX,
//   --, AY, BY, CY,
//   --, AZ, BZ, CZ,

// Part 1: X = rock, Y = paper, Z = scissors
static const int score1[] = {
    0, ROCK + DRAW, ROCK + LOSS, ROCK + WIN ,
    0, PAPR + WIN , PAPR + DRAW, PAPR + LOSS,
    0, SCIS + LOSS, SCIS + WIN , SCIS + DRAW};

// Part 2: X = loss, Y = draw, Z = win
static const int score2[] = {
    0, SCIS + LOSS, ROCK + LOSS, PAPR + LOSS,
    0, ROCK + DRAW, PAPR + DRAW, SCIS + DRAW,
    0, PAPR + WIN , SCIS + WIN , ROCK + WIN };

static uint32_t data[LINES];

int main(void)
{
    FILE *f = fopen(FNAME, "rb");  // fread requires binary mode
    if (!f) return 1;
    fread(data, sizeof data, 1, f);  // read whole file as 1 block
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    int sum1 = 0, sum2 = 0;
    for (int i = 0; i < LINES; ++i) {
        // A=1/B=2/C=3 + X=0/Y=4/Z=8
        const uint32_t hash = (data[i] & 0x3U) | (data[i] >> 14 & 0xCU);
        sum1 += score1[hash];  // part 1
        sum2 += score2[hash];  // part 2
    }
    printf("%d %d\n", sum1, sum2);  // 11767 13886

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
