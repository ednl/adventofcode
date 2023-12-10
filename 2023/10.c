#include <stdio.h>    // fopen, fclose, fgets, printf
#include <stdbool.h>  // bool

#define NAME "../aocinput/2023-10-input.txt"
#define N 140

typedef struct vec {
    int x, y;
} Vec;

static char field[N][N + 2];  // room for '\n\0'

static bool equal(const Vec a, const Vec b)
{
    return a.x == b.x && a.y == b.y;
}

// Get pipe at position pos
static char getpipe(const Vec pos)
{
    return field[pos.y][pos.x];
}

// Set pipe at position pos
static void setpipe(const Vec pos, const char pipe)
{
    field[pos.y][pos.x] = pipe;
}

// Find start ('S')
static Vec startpos(void)
{
    for (int y = 0; y < N; ++y)
        for (int x = 0; x < N; ++x)
            if (getpipe((Vec){x, y}) == 'S')
                return (Vec){x, y};
    return (Vec){-1, -1};
}

// Return next position from current position and direction
static Vec nextpos(const Vec pos, const char dir)
{
    switch (dir) {
        case 'N': return (Vec){pos.x    , pos.y - 1};
        case 'E': return (Vec){pos.x + 1, pos.y    };
        case 'S': return (Vec){pos.x    , pos.y + 1};
        case 'W': return (Vec){pos.x - 1, pos.y    };
    }
    return pos;
}

// Set S to appropriate pipe bend
// Return: direction to start (NESW)
static char startdir(const Vec pos)
{
    if (pos.x < 0 || pos.y < 0 || pos.x >= N || pos.y >= N || getpipe(pos) != 'S')
        return '\0';
    int connections = 0;  // bits set for NESW (N = LSB, W = MSB)
    if (pos.y > 0) {
        const char p = getpipe(nextpos(pos, 'N'));  // North
        if (p == '|' || p == '7' || p == 'F') connections |= (1 << 0);
    }
    if (pos.x < N - 1) {
        const char p = getpipe(nextpos(pos, 'E'));  // East
        if (p == '-' || p == 'J' || p == '7') connections |= (1 << 1);
    }
    if (pos.y < N - 1) {
        const char p = getpipe(nextpos(pos, 'S'));  // South
        if (p == '|' || p == 'J' || p == 'L') connections |= (1 << 2);
    }
    if (pos.x > 0) {
        const char p = getpipe(nextpos(pos, 'W'));  // West
        if (p == '-' || p == 'L' || p == 'F') connections |= (1 << 3);
    }
    switch (connections) {
        case  3: setpipe(pos, 'L'); return 'N';  // 0b0011 = NE -> L
        case  5: setpipe(pos, '|'); return 'N';  // 0b0101 = NS -> |
        case  9: setpipe(pos, 'J'); return 'N';  // 0b1001 = NW -> J
        case  6: setpipe(pos, 'F'); return 'E';  // 0b0110 = ES -> F
        case 10: setpipe(pos, '-'); return 'E';  // 0b1010 = EW -> -
        case 12: setpipe(pos, '7'); return 'S';  // 0b1100 = SW -> 7
    }
    return '\0';
}

static bool move(Vec* pos, char* dir)
{
    const Vec next = nextpos(*pos, *dir);
    switch (((*dir) << 8) | getpipe(next)) {
        case 'N|':
        case 'EJ':
        case 'WL': *pos = next; *dir = 'N'; return true;
        case 'E-':
        case 'SL':
        case 'NF': *pos = next; *dir = 'E'; return true;
        case 'E7':
        case 'S|':
        case 'WF': *pos = next; *dir = 'S'; return true;
        case 'N7':
        case 'SJ':
        case 'W-': *pos = next; *dir = 'W'; return true;
    }
    return false;
}

int main(void)
{
    FILE* f = fopen(NAME, "r");
    if (!f)
        return 1;
    for (int i = 0; i < N; ++i)
        fgets(field[i], sizeof *field, f);
    fclose(f);

    const Vec start = startpos();  // my input: (118,102)
    char dir = startdir(start);

    Vec pos = start;
    int len = move(&pos, &dir);
    while (!equal(pos, start) && move(&pos, &dir))
        ++len;

    printf("Part 1: %d\n", len / 2);  // 7005
    return 0;
}
