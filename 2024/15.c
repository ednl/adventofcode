#include <stdio.h>
#include <stdbool.h>

#define EXAMPLE 0
#if EXAMPLE == 1
    #define FNAME "../aocinput/2024-15-example1.txt"
    #define N 8
    #define L 1
    #define M 15
#elif EXAMPLE == 2
    #define FNAME "../aocinput/2024-15-example2.txt"
    #define N 10
    #define L 10
    #define M 70
#else
    #define FNAME "../aocinput/2024-15-input.txt"
    #define N 50
    #define L 20
    #define M 1000
#endif
#define MOVES (L * (M + 1))
#define FLOOR '.'
#define WALL  '#'
#define BOX   'O'
#define ROBOT '@'

typedef struct pos {
    int x, y;
} Pos;

typedef enum dir {
    RIGHT, DOWN, LEFT, UP
} Dir;

// Same order as 'enum dir'
static const Pos step[] = {{1,0},{0,1},{-1,0},{0,-1}};

static char map[N][N + 1];
static char move[MOVES];
static Pos robot;

static void show(void)
{
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j)
            putchar(map[i][j]);
        putchar('\n');
    }
    putchar('\n');
}

static inline Pos add(const Pos a, const Pos b)
{
    return (Pos){a.x + b.x, a.y + b.y};
}

static inline void add_r(Pos *const a, const Pos b)
{
    a->x += b.x;
    a->y += b.y;
}

static inline char peek(const Pos pos)
{
    return map[pos.y][pos.x];
}

static inline void poke(const Pos pos, const char tile)
{
    map[pos.y][pos.x] = tile;
}

static Pos findrobot(void)
{
    for (int i = 1; i < N - 1; ++i)  // skip border
        for (int j = 1; j < N - 1; ++j)
            if (map[i][j] == ROBOT)
                return (Pos){j, i};
    return (Pos){0};
}

static bool shift(const Pos pos, const Dir dir)
{
    const Pos next = add(pos, step[dir]);
    if (peek(next) == WALL)
        return false;
    if (peek(next) == FLOOR || shift(next, dir)) {
        poke(next, peek(pos));
        poke(pos, FLOOR);
        return true;
    }
    return false;
}

static int gps(void)
{
    int sum = 0;
    for (int i = 1; i < N - 1; ++i)  // skip border
        for (int j = 1; j < N - 1; ++j)
            if (map[i][j] == BOX)
                sum += i * 100 + j;
    return sum;
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    fread(map, sizeof map, 1, f);
    fgetc(f);  // empty line
    fread(move, sizeof move, 1, f);

    robot = findrobot();
    for (int i = 0; i < MOVES; ++i)
        switch (move[i]) {
            case '>': if (shift(robot, RIGHT)) add_r(&robot, step[RIGHT]); break;
            case 'v': if (shift(robot, DOWN )) add_r(&robot, step[DOWN ]); break;
            case '<': if (shift(robot, LEFT )) add_r(&robot, step[LEFT ]); break;
            case '^': if (shift(robot, UP   )) add_r(&robot, step[UP   ]); break;
        }
    show();
    printf("Part 1: %d\n", gps());  // example 1: 2028, example 2: 10092, input: 1497888
    return 0;
}
