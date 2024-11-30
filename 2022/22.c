/**
 * Advent of Code 2022
 * Day 22: Monkey Map
 * https://adventofcode.com/2022/day/22
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -O3 -march=native -Wall -Wextra 22.c ../startstoptimer.c
 *    gcc   -std=gnu17 -O3 -march=native -Wall -Wextra 22.c ../startstoptimer.c
 * Get minimum runtime:
 *     m=999999;for((i=0;i<1000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * Minimum runtime:
 *     Mac Mini 2020 (M1 3.2 GHz)          :   ? µs
 *     iMac 2013 (i5 Haswell 4570 3.2 GHz) :   ? µs
 *     Raspberry Pi 5 (2.4 GHz)            :   ? µs
 *     Raspberry Pi 4 (1.8 GHz)            :   ? µs
 */

#include <stdio.h>
#include <string.h>  // memset

#define EXAMPLE 0
#if EXAMPLE == 1
    #define NAME "../aocinput/2022-22-example.txt"
    #define COLS 16
    #define ROWS 12
    #define TURN 6
#else
    #define NAME "../aocinput/2022-22-input.txt"
    #define COLS 150
    #define ROWS 200
    #define TURN 2000
#endif

#define BORDER ' '
#define OPEN '.'
#define WALL '#'

// Order of directions 0..3 is positively oriented
// like exp(n.pi/2) but with inverted y-axis.
#define DIRSIZE 4
typedef enum dir {
    RIGHT, DOWN, LEFT, UP
} Dir;
#if EXAMPLE
static const char *dirtile = ">v<^";
#endif

typedef struct pos {
    int row, col;
} Pos;

// Inclusive range
typedef struct range {
    int min, max;
} Range;

typedef struct segm {
    char turn;
    int  dist;
} Segm;

static char map[ROWS][COLS + 2];
static Range colrange[ROWS];  // every row [0..ROWS-1] has a range of cols
static Range rowrange[COLS];  // every col [0..COLS-1] has a range of rows
static Segm segm[TURN];

// Turn left or right from currently facing direction.
// L (e.g. from East to North) is negative because y-axis is inverted.
static Dir rotate(const Dir facing, const char letter)
{
    switch (letter) {
        case 'L': return (facing - 1 + DIRSIZE) % DIRSIZE;
        case 'R': return (facing + 1) % DIRSIZE;
        default : return facing;
    }
}

static Pos right(Pos pos, int dist)
{
    const Range *const r = &colrange[pos.row];
    while (dist--) {
        const int t = pos.col == r->max ? r->min : pos.col + 1;
        if (map[pos.row][t] == WALL)
            break;
    #if EXAMPLE
        map[pos.row][pos.col] = dirtile[RIGHT];
    #endif
        pos.col = t;
    }
    return pos;
}

static Pos left(Pos pos, int dist)
{
    const Range *const r = &colrange[pos.row];
    while (dist--) {
        const int t = pos.col == r->min ? r->max : pos.col - 1;
        if (map[pos.row][t] == WALL)
            break;
    #if EXAMPLE
        map[pos.row][pos.col] = dirtile[LEFT];
    #endif
        pos.col = t;
    }
    return pos;
}

static Pos down(Pos pos, int dist)
{
    const Range *const r = &rowrange[pos.col];
    while (dist--) {
        const int t = pos.row == r->max ? r->min : pos.row + 1;
        if (map[t][pos.col] == WALL)
            break;
    #if EXAMPLE
        map[pos.row][pos.col] = dirtile[DOWN];
    #endif
        pos.row = t;
    }
    return pos;
}

static Pos up(Pos pos, int dist)
{
    const Range *const r = &rowrange[pos.col];
    while (dist--) {
        const int t = pos.row == r->min ? r->max : pos.row - 1;
        if (map[t][pos.col] == WALL)
            break;
    #if EXAMPLE
        map[pos.row][pos.col] = dirtile[UP];
    #endif
        pos.row = t;
    }
    return pos;
}

// Same order as 'enum dir'
static Pos (*go[DIRSIZE])(Pos, int) = {right, down, left, up};

int main(void)
{
    FILE *f = fopen(NAME, "r");
    if (!f)
        return 1;

    // Init, read map
    memset(map, BORDER, sizeof map);
    for (int i = 0; i < ROWS; ++i)
        fgets(map[i], sizeof map[0], f);

    // Read instructions
    int first = 0;
    fscanf(f, "%d", &first);  // also skips empty line
    for (int i = 0; i < TURN; ++i)
        fscanf(f, "%c%d", &segm[i].turn, &segm[i].dist);
    fclose(f);

    // Determine (inclusive) colrange parameters for every row
    for (int i = 0, j; i < ROWS; ++i) {
        for (j = 0; map[i][j] == BORDER; ++j);
        const int k = j++;
        for (; map[i][j] == OPEN || map[i][j] == WALL; ++j);
        colrange[i] = (Range){k, j - 1};
        // Erase line ending from fgets
        map[i][j    ] = BORDER;
        map[i][j + 1] = BORDER;
        // String terminator
        map[i][COLS] = '\0';
    }

    // Determine (inclusive) rowrange parameters for every col
    for (int j = 0, i; j < COLS; ++j) {
        for (i = 0; map[i][j] == BORDER; ++i);
        const int k = i++;
        for (; map[i][j] == OPEN || map[i][j] == WALL; ++i);
        rowrange[j] = (Range){k, i - 1};
    }

    // Start at first OPEN position on first row
    Dir dir = RIGHT;
    Pos pos = {0, 0};
    for (; map[pos.row][pos.col] != OPEN; ++pos.col);

    // First segment
    for (int i = 0; i < first && map[pos.row][pos.col + 1] == OPEN; ++i, ++pos.col) {
    #if EXAMPLE
        map[pos.row][pos.col] = dirtile[RIGHT];
    #endif
    }

    // All other segments: turn and go
    for (int i = 0; i < TURN; ++i) {
        dir = rotate(dir, segm[i].turn);
        pos = (*go[dir])(pos, segm[i].dist);
    }

#if EXAMPLE
    map[pos.row][pos.col] = dirtile[dir];
    for (int i = 0; i < ROWS; ++i)
        printf("%s\n", map[i]);
#endif

    printf("Part 1: %d\n", (pos.row + 1) * 1000 + (pos.col + 1) * 4 + dir);  // example=6032, input=93226
    return 0;
}
