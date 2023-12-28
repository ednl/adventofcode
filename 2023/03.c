/**
 * Advent of Code 2023
 * Day 3: Gear Ratios
 * https://adventofcode.com/2023/day/3
 * By: E. Dronkert https://github.com/ednl
 */

#include <stdio.h>    // fopen, fclose, fgets, printf
#include <ctype.h>    // isdigit
#include <string.h>   // memset
#include <stdbool.h>  // bool

#define EXAMPLE 0
#if EXAMPLE == 1
#define FNAME "../aocinput/2023-03-example.txt"
#define N 10
#else
#define FNAME "../aocinput/2023-03-input.txt"
#define N 140
#endif

// Engine schematic square grid with padding and room for end of line '\n\0'
static char schematic[N + 2][N + 4];

static bool issymbol(const char c)
{
    return c != '.' && !isdigit(c);
}

// Check for any symbol around number
//   Pre: schematic[r][c0..c1] are digits
//   Return: at least 1 symbol found = true/false
static bool symboladjacent(const int r, const int c0, const int c1)
{
    for (int j = c0 - 1; j <= c1 + 1; ++j)
        if (issymbol(schematic[r - 1][j]) || issymbol(schematic[r + 1][j]))
            return true;
    if (issymbol(schematic[r][c0 - 1]) || issymbol(schematic[r][c1 + 1]))
        return true;
    return false;
}

// Convert to number from digit(s) forward
//   Pre: schematic[r][c] is digit (might be more after)
//   Post: schematic[r][c] points to last consecutive digit
//   Return: number
static int parse_from(const int r, int* const c)
{
    int n = schematic[r][*c] & 15;
    while (*c < N && isdigit(schematic[r][*c + 1]))
        n = n * 10 + (schematic[r][++(*c)] & 15);
    return n;
}

// Check part number
//   Pre: schematic[r][c] is digit, might be start of part number
//   Post: schematic[r][c] is first char AFTER all consecutive digits
//   Return: part number or zero
static int partnumber(const int r, int* const c)
{
    int k = *c;  // save first col index of number
    int n = parse_from(r, c);  // *c is now last col index of number
    return symboladjacent(r, k, *c) ? n : 0;  // valid part number?
}

// Convert to number from range of digits & around
//   Pre: schematic[r][c0..c1] are digits (might be more before or after)
//   Return: number composed of all consecutive digits
static int parse_around(const int r, int c0, int c1)
{
    while (c0 > 1 && isdigit(schematic[r][c0 - 1]))
        --c0;
    while (c1 < N && isdigit(schematic[r][c1 + 1]))
        ++c1;
    int n = 0;
    while (c0 <= c1)
        n = n * 10 + (schematic[r][c0++] & 15);
    return n;
}

// Calculate gear ratio
// Pre: schematic[r][c] = '*' (possible gear)
// Return: product of exactly 2 adjacent numbers, or zero
static int gearratio(const int r, const int c)
{
    // Find pattern of digits around gear
    int pat[3] = {0};
    for (int i = r - 1, k = 0; i <= r + 1; ++i, ++k)
        for (int j = c - 1; j <= c + 1; ++j)
            pat[k] = pat[k] << 1 | isdigit(schematic[i][j]);

    // Count separate numbers around gear
    int nums = 0;
    for (int i = 0; i < 3; ++i)
        switch (pat[i]) {
            case  0: break;
            case  5: nums += 2; break;
            default: nums += 1; break;
        }
    if (nums != 2)
        return 0;

    // Parse 2 numbers and return product
    int gear[2] = {0};
    for (int i = 0, k = r - 1, g = 0; i < 3; ++i, ++k)
        switch (pat[i]) {
            case 0: continue;  // no numbers on this line                 000
            case 1: gear[g++] = parse_around(k, c + 1, c + 1); break;  // 001
            case 2: gear[g++] = parse_around(k, c    , c    ); break;  // 010
            case 3: gear[g++] = parse_around(k, c    , c + 1); break;  // 011
            case 4: gear[g++] = parse_around(k, c - 1, c - 1); break;  // 100
            case 5: gear[g++] = parse_around(k, c - 1, c - 1);
                    gear[g++] = parse_around(k, c + 1, c + 1); break;  // 101
            case 6: gear[g++] = parse_around(k, c - 1, c    ); break;  // 110
            case 7: gear[g++] = parse_around(k, c - 1, c + 1); break;  // 111
        }
    return gear[0] * gear[1];
}

int main(void)
{
    FILE* f = fopen(FNAME, "r");
    if (!f) { fputs("File not found.\n", stderr); return 1; }

    memset(schematic, '.', sizeof schematic);  // init including padding
    for (int i = 1; i <= N; ++i) {
        fgets(schematic[i] + 1, N + 2, f);
        schematic[i][N + 1] = '.';  // reset padding at end of line
    }
    fclose(f);

    int sum1 = 0;
    for (int i = 1; i <= N; ++i)
        for (int j = 1; j <= N; ++j)
            if (isdigit(schematic[i][j]))  // start of possible part number
                sum1 += partnumber(i, &j);
    printf("%u\n", sum1);  // example: 4361, input: 536202

    int sum2 = 0;
    for (int i = 1; i <= N; ++i)
        for (int j = 1; j <= N; ++j)
            if (schematic[i][j] == '*')  // possible gear
                sum2 += gearratio(i, j);
    printf("%u\n", sum2);  // example: 467835, input: 78272573
    return 0;
}
