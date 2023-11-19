/**
 * Advent of Code 2022
 * Day 2: Rock Paper Scissors
 * https://adventofcode.com/2022/day/2
 * By: E. Dronkert https://github.com/ednl
 */

#include <stdio.h>
#include <stdint.h>

#define LINES 2500
#define ROCK     1
#define PAPER    2
#define SCISSORS 3
#define LOSS     0
#define DRAW     3
#define WIN      6

// 'A' = 0x41 = 0b01000001, 'X' = 0x58 = 0b01011000, space = 0x20, newline = 0x0a
// so "A X" with newline = 4 bytes = [0x41, 0x20, 0x58, 0x0a] = 0x0a582041 (little-endian)
// Bits 17 and 16 are: 00 (X), 01 (Y) or 10 (Z)
// Bits  1 and  0 are: 01 (A), 10 (B) or 11 (C)
// Make 4-bit index (values 0-15) from bits 17,16,1,0:
//   --, AX, BX, CX, --, AY, BY, CY, --, AZ, BZ, CZ, --, --, --, --
static int score1[16] = {
    0, ROCK     + DRAW, ROCK     + LOSS, ROCK     + WIN ,
    0, PAPER    + WIN , PAPER    + DRAW, PAPER    + LOSS,
    0, SCISSORS + LOSS, SCISSORS + WIN , SCISSORS + DRAW};
static int score2[16] = {
    0, SCISSORS + LOSS, ROCK     + LOSS, PAPER    + LOSS,
    0, ROCK     + DRAW, PAPER    + DRAW, SCISSORS + DRAW,
    0, PAPER    + WIN , SCISSORS + WIN , ROCK     + WIN };
static uint32_t data[LINES];

int main(void)
{
    FILE *f = fopen("../aocinput/2022-02-input.txt", "rb");
    fread(data, sizeof *data, LINES, f);
    fclose(f);

    int hash = 0, sum1 = 0, sum2 = 0;
    for (int i = 0; i < LINES; ++i) {
        hash = (data[i] & 0x3) | ((data[i] & 0x30000) >> 14);
        sum1 += score1[hash];
        sum2 += score2[hash];
    }
    printf("%d\n%d\n", sum1, sum2);  // 11767 13886
    return 0;
}
